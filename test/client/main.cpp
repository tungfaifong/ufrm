// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"
#include "usrv/unit_manager.h"
#include "usrv/units/server_unit.h"

#include "protocol/cs.pb.h"
#include "common.h"
#include "define.h"

using namespace usrv;

class Client;

uint64_t g_TotalDelay = 0;
uint32_t g_TotalCnt = 0;
int32_t g_ClientNum = 0;
std::unordered_map<NETID, std::shared_ptr<Client>> g_Clients;

class Client : public Unit, public std::enable_shared_from_this<Client>
{
public:
	Client(USERID user_id, uint32_t req_num):_user_id(user_id), _req_num(req_num) {}
	~Client() = default;

	virtual bool Start();

	void OnRecv(NETID net_id, char * data, uint16_t size);

	void SendToServer(CSID id, google::protobuf::Message * body);

	void AuthReq();
	void OnAuthRsp(const SCAuthRsp & rsp);

	void GetRolesReq();
	void OnGetRolesRsp(const SCGetRolesRsp & rsp);

	void CreateRoleReq();
	void OnCreateRoleRsp(const SCCreateRoleRsp & rsp);

	void LoginReq(ROLEID role_id);
	void OnLoginRsp(const SCLoginRsp & rsp);

	void HeartBeat();
	void OnHeartBeatRsp();

private:
	NETID _net_id = INVALID_NET_ID;
	USERID _user_id;
	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _end;
	uint32_t _req_num;
};

bool Client::Start()
{
	_net_id = server::Connect("127.0.0.1", 6666, 1000);
	if(_net_id == INVALID_NET_ID)
	{
		LOGGER_ERROR("client connect failed.");
		return false;
	}
	g_Clients[_net_id] = shared_from_this();
	AuthReq();
	return true;
}

void Client::OnRecv(NETID net_id, char * data, uint16_t size)
{
	CSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	LOGGER_TRACE("recv server msg net_id:{} id:{}", net_id, ENUM_NAME(head.id()));
	switch (head.id())
	{
	case SCID_AUTH_RSP:
		{
			UNPACK(SCAuthRsp, body, pkg.data());
			OnAuthRsp(body);
		}
		break;
	case SCID_GET_ROLES_RSP:
		{
			UNPACK(SCGetRolesRsp, body, pkg.data());
			OnGetRolesRsp(body);
		}
		break;
	case SCID_CREATE_ROLE_RSP:
		{
			UNPACK(SCCreateRoleRsp, body, pkg.data());
			OnCreateRoleRsp(body);
		}
		break;
	case SCID_LOGIN_RSP:
		{
			UNPACK(SCLoginRsp, body, pkg.data());
			OnLoginRsp(body);
		}
		break;
	case SCID_HEART_BEAT_RSP:
		{
			OnHeartBeatRsp();
		}
		break;
	default:
		break;
	}
}

void Client::SendToServer(CSID id, google::protobuf::Message * body)
{
	CSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_id(id);
	body->SerializeToString(pkg.mutable_data());
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("pkg size too long, id:{} size:{}", ENUM_NAME(id), size);
		return;
	}
	server::Send(_net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("send msg id:{}", ENUM_NAME(id));
}

void Client::AuthReq()
{
	CSAuthReq body;
	body.set_user_id(_user_id);
	SendToServer(CSID_AUTH_REQ, &body);
}

void Client::OnAuthRsp(const SCAuthRsp & rsp)
{
	GetRolesReq();
	LOGGER_INFO("OnAuthRsp:{} {}", _user_id, rsp.result());
}

void Client::GetRolesReq()
{
	CSGetRolesReq body;
	SendToServer(CSID_GET_ROLES_REQ, &body);
}

void Client::OnGetRolesRsp(const SCGetRolesRsp & rsp)
{
	if(rsp.role_ids_size() == 0)
	{
		CreateRoleReq();
	}
	else
	{
		LoginReq(rsp.role_ids(0));
	}
}

void Client::CreateRoleReq()
{
	CSCreateRoleReq body;
	auto name = "testrole" + std::to_string(_user_id);
	body.set_name(name);
	SendToServer(CSID_CREATE_ROLE_REQ, &body);
}

void Client::OnCreateRoleRsp(const SCCreateRoleRsp & rsp)
{
	if(rsp.result() == SCCreateRoleRsp::SUCCESS)
	{
		LoginReq(rsp.role_id());
	}
	else
	{
		LOGGER_ERROR("create role failed.");
	}
}

void Client::LoginReq(ROLEID role_id)
{
	CSLoginReq body;
	body.set_role_id(role_id);
	SendToServer(CSID_LOGIN_REQ, &body);
}

void Client::OnLoginRsp(const SCLoginRsp & rsp)
{
	for(uint32_t i = 0; i < _req_num; ++i)
	{
		HeartBeat();
	}
	LOGGER_INFO("OnLoginRsp:{} {}", _user_id, rsp.result());
}

void Client::HeartBeat()
{
	CSHeartBeatReq body;
	SendToServer(CSID_HEART_BEAT_REQ, &body);
	_start = std::chrono::steady_clock::now();
}

void Client::OnHeartBeatRsp()
{
	// _end = std::chrono::steady_clock::now();
	// LOGGER_INFO("heart beat avg delay:{}", std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count());
	++g_TotalCnt;
	timer::CreateTimer(500, [self = shared_from_this()](){self->HeartBeat();});
}

int main(int argc, char * argv[])
{
	g_ClientNum = atoi(argv[1]);
	auto req_num = atoi(argv[2]);
	auto time = atoi(argv[3]) * 1000;
	UnitManager::Instance()->Init(10);
	UnitManager::Instance()->Register("LOGGER", std::move(std::make_shared<LoggerUnit>(LoggerUnit::LEVEL::INFO, "/logs/client.log", 1 Mi)));
	UnitManager::Instance()->Register("TIMER", std::move(std::make_shared<TimerUnit>(1 Ki, 1 Ki)));
	UnitManager::Instance()->Register("SERVER", std::move(std::make_shared<ServerUnit>(1 Ki, 1 Ki, 512 Ki)));
	for(int i = 0; i < g_ClientNum; ++i)
	{
		auto client_key = "CLIENT#"+std::to_string(i+1);
		if(!UnitManager::Instance()->Register(client_key.c_str(), std::move(std::make_shared<Client>(i+1, req_num))))
		{
			LOGGER_ERROR("key:{} error", client_key);
		}
	}

	auto server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	server->OnConn([](NETID net_id, IP ip, PORT port){
		LOGGER_INFO("conn: net_id:{} ip:{} port:{}", net_id, ip, port);
	});

	server->OnRecv([](NETID net_id, char * data, uint16_t size) {
		auto client = g_Clients.find(net_id);
		if(client != g_Clients.end())
		{
			client->second->OnRecv(net_id, data, size);
		}
	});

	server->OnDisc([](NETID net_id){
		LOGGER_INFO("conn: net_id:{}", net_id);
	});

	timer::CreateTimer(time, [](){UnitManager::Instance()->SetExit(true);});

	UnitManager::Instance()->Run();

	LOGGER_INFO("qps:{}", g_TotalCnt / (time / 1000));

	return true;
}