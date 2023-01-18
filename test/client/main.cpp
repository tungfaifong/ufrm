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

	void SendToServer(CSID id, CSPkgBody * body);

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
};

bool Client::Init()
{
	auto server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	server->OnConn([](NETID net_id, IP ip, PORT port){
		LOGGER_INFO("conn: net_id:{} ip:{} port:{}", net_id, ip, port);
	});

	server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size) {
		self->OnRecv(net_id, data, size);
	});

	server->OnDisc([](NETID net_id){
		LOGGER_INFO("conn: net_id:{}", net_id);
	});

	return true;
}

bool Client::Start()
{
	_server_net_id = server::Connect("127.0.0.1", 6666, 1000);
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
	auto body = pkg.body();
	LOGGER_TRACE("recv server msg net_id:{} id:{}", net_id, ENUM_NAME(head.id()));
	switch (head.id())
	{
	case SCID_AUTH_RSP:
		{
			OnAuthRsp(body.auth_rsp());
		}
		break;
	case SCID_LOGIN_RSP:
		{
			OnLoginRsp(body.login_rsp());
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

void Client::SendToServer(CSID id, CSPkgBody * body)
{
	CSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_id(id);
	pkg.set_allocated_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("pkg size too long, id:{} size:{}", ENUM_NAME(id), size);
		return;
	}
	server::Send(_server_net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("send msg id:{}", ENUM_NAME(id));
}

void Client::AuthReq()
{
	PKG_CREATE(body, CSPkgBody);
	auto req =body->mutable_auth_req();
	req->set_role_id(_role_id);
	req->set_game_id(1);
	SendToServer(CSID_AUTH_REQ, body);
}

void Client::OnAuthRsp(const SCAuthRsp & rsp)
{
	LoginReq();
}

void Client::LoginReq()
{
	PKG_CREATE(body, CSPkgBody);
	auto req = body->mutable_login_req();
	SendToServer(CSID_LOGIN_REQ, body);
}

void Client::OnLoginRsp(const SCLoginRsp & rsp)
{
	HeartBeat();
}

void Client::HeartBeat()
{
	PKG_CREATE(body, CSPkgBody);
	auto req =body->mutable_heart_beat_req();
	SendToServer(CSID_HEART_BEAT_REQ, body);
	_start = std::chrono::steady_clock::now();
	timer::CreateTimer(1000, [self = shared_from_this()](){ self->HeartBeat(); });
}

void Client::OnHeartBeatRsp()
{
	_end = std::chrono::steady_clock::now();
	++g_TotalCnt;
	g_TotalDelay += std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
	if(g_TotalCnt == g_ClientNum)
	{
		LOGGER_INFO("heart beat avg delay:{}", (float)g_TotalDelay/g_TotalCnt);

		g_TotalCnt = 0;
		g_TotalDelay = 0;
	}
}

int main(int argc, char * argv[])
{
	g_ClientNum = atoi(argv[1]);
	UnitManager::Instance()->Init(10);
	UnitManager::Instance()->Register("LOGGER", std::move(std::make_shared<LoggerUnit>(LoggerUnit::LEVEL::INFO, "/logs/client.log", 1 Mi)));
	UnitManager::Instance()->Register("SERVER", std::move(std::make_shared<ServerUnit>(1 Ki, 1 Ki, 4 Mi)));
	UnitManager::Instance()->Register("TIMER", std::move(std::make_shared<TimerUnit>(1 Ki, 1 Ki)));
	for(int i = 0; i < g_ClientNum; ++i)
	{
		auto key = "CLIENT#"+std::to_string(i+1);
		if(!UnitManager::Instance()->Register(key.c_str(), std::move(std::make_shared<Client>(i+1))))
		{
			LOGGER_ERROR("key:{} error", key);
		}
	}

	auto mgr = UnitManager::Instance();

	UnitManager::Instance()->Run();

	return true;
}