// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gateway.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "protocol/ss.pb.h"

Gateway::Gateway(NODEID id, toml::table & config) : _id(id), _config(config)
{

}

bool Gateway::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("Gateway::Init ERROR: gateway id is INVALID");
		return false;
	}

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));
	_iserver = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("ISERVER"));

	_server->OnConn(std::bind(&Gateway::_OnServerConn, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_server->OnRecv(std::bind(&Gateway::_OnServerRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_server->OnDisc(std::bind(&Gateway::_OnServerDisc, shared_from_this(), std::placeholders::_1));
	_iserver->OnConn(std::bind(&Gateway::_OnServerConn, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_iserver->OnRecv(std::bind(&Gateway::_OnIServerRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_iserver->OnDisc(std::bind(&Gateway::_OnServerDisc, shared_from_this(), std::placeholders::_1));

	return true;
}

bool Gateway::Start()
{
	return true;
}

bool Gateway::Update(intvl_t interval)
{
	return false;
}

void Gateway::Stop()
{
}

void Gateway::Release()
{
	_server = nullptr;
	_iserver = nullptr;
}

void Gateway::_OnServerConn(NETID net_id, IP ip, PORT port)
{

}

void Gateway::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{

}

void Gateway::_OnServerDisc(NETID net_id)
{

}

void Gateway::_OnIServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("Gateway::_OnIServerConn success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void Gateway::_OnIServerRecv(NETID net_id, char * data, uint16_t size)
{
}

void Gateway::_OnIServerDisc(NETID net_id)
{
	LOGGER_INFO("Gateway::_OnIServerDisc success net_id:{}", net_id);
}
