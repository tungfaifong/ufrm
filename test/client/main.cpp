// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/unit_manager.h"
#include "usrv/units/server_unit.h"

#include "protocol/cs.pb.h"
#include "define.h"

using namespace usrv;

class Client : public Unit, public std::enable_shared_from_this<Client>
{
public:
	Client() = default;
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

private:
	NETID _server_net_id = INVALID_NET_ID;
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
	return true;
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
	req->set_role_id(1);
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
	SendToServer(CSID_LOGIN_REQ, body);
}

void Client::OnLoginRsp(const SCLoginRsp & rsp)
{

}

int main()
{
	UnitManager::Instance()->Init(10);
	UnitManager::Instance()->Register("LOGGER", std::move(std::make_shared<LoggerUnit>(LoggerUnit::Level::TRACE, "/logs/client.log", 1 Mi)));
	UnitManager::Instance()->Register("SERVER", std::move(std::make_shared<ServerUnit>(1 Ki, 1 Ki, 4 Mi)));
	UnitManager::Instance()->Register("CLIENT", std::move(std::make_shared<Client>()));

	UnitManager::Instance()->Run();

	return true;
}