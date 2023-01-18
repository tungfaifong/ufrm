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

uint64_t g_TotalDelay = 0;
uint32_t g_TotalCnt = 0;
uint32_t g_ClientNum = 0;


class Client : public Unit, public std::enable_shared_from_this<Client>
{
public:
	Client(ROLEID role_id):_role_id(role_id) {}
	~Client() = default;

	virtual bool Init() override final;
	virtual bool Start();
	virtual bool Update(intvl_t interval);

	void OnRecv(NETID net_id, char * data, uint16_t size);

	void SendToServer(CSID id, google::protobuf::Message * body);

	void AuthReq();
	void OnAuthRsp(const SCAuthRsp & rsp);

	void LoginReq();
	void OnLoginRsp(const SCLoginRsp & rsp);

	void HeartBeat();
	void OnHeartBeatRsp();

private:
	NETID _server_net_id = INVALID_NET_ID;
	ROLEID _role_id;
	std::chrono::steady_clock::time_point _start;
	std::chrono::steady_clock::time_point _end;
	std::shared_ptr<ServerUnit> _server;
};

bool Client::Init()
{
	auto server_key = "SERVER#"+std::to_string(_role_id);

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get(server_key.c_str()));

	_server->OnConn([](NETID net_id, IP ip, PORT port){
		LOGGER_INFO("conn: net_id:{} ip:{} port:{}", net_id, ip, port);
	});

	_server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size) {
		self->OnRecv(net_id, data, size);
	});

	_server->OnDisc([](NETID net_id){
		LOGGER_INFO("conn: net_id:{}", net_id);
	});

	return true;
}

bool Client::Start()
{
	_server_net_id = _server->Connect("127.0.0.1", 6666, 1000);
	if(_server_net_id == INVALID_NET_ID)
	{
		LOGGER_ERROR("client connect failed.");
		return false;
	}
	AuthReq();
	return true;
}

bool Client::Update(intvl_t interval)
{
	return false;
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
	_server->Send(_server_net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("send msg id:{}", ENUM_NAME(id));
}

void Client::AuthReq()
{
	CSAuthReq body;
	body.set_role_id(_role_id);
	body.set_game_id(1);
	SendToServer(CSID_AUTH_REQ, &body);
}

void Client::OnAuthRsp(const SCAuthRsp & rsp)
{
	LoginReq();
	LOGGER_INFO("OnAuthRsp:{} {}", _role_id, rsp.result());
}

void Client::LoginReq()
{
	CSLoginReq body;
	SendToServer(CSID_LOGIN_REQ, &body);
}

void Client::OnLoginRsp(const SCLoginRsp & rsp)
{
	for(auto i = 0; i < 1000; ++i)
	{
		HeartBeat();
	}
	LOGGER_INFO("OnLoginRsp:{} {}", _role_id, rsp.result());
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
	HeartBeat();
}

int main(int argc, char * argv[])
{
	g_ClientNum = atoi(argv[1]);
	auto time = atoi(argv[2]) * 1000;
	UnitManager::Instance()->Init(10);
	UnitManager::Instance()->Register("LOGGER", std::move(std::make_shared<LoggerUnit>(LoggerUnit::LEVEL::INFO, "/logs/client.log", 1 Mi)));
	UnitManager::Instance()->Register("TIMER", std::move(std::make_shared<TimerUnit>(1 Ki, 1 Ki)));
	for(int i = 0; i < g_ClientNum; ++i)
	{
		auto server_key = "SERVER#"+std::to_string(i+1);
		auto client_key = "CLIENT#"+std::to_string(i+1);
		if(!UnitManager::Instance()->Register(server_key.c_str(), std::move(std::make_shared<ServerUnit>(512, 512, 1 Ki))) || !UnitManager::Instance()->Register(client_key.c_str(), std::move(std::make_shared<Client>(i+1))))
		{
			LOGGER_ERROR("key:{} error", server_key);
		}
	}

	timer::CreateTimer(time, [](){UnitManager::Instance()->SetExit(true);});

	UnitManager::Instance()->Run();

	LOGGER_INFO("total cnt:{}", g_TotalCnt);

	return true;
}