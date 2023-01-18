// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lbsrv.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "protocol/ss.pb.h"

LBSrv::LBSrv(NODEID id, toml::table & config) : _id(id), _config(config)
{

}

bool LBSrv::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("LBSrv::Init ERROR: gateway_mgr id is INVALID");
		return false;
	}

	auto server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	server->OnConn(std::bind(&LBSrv::_OnServerConn, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server->OnRecv(std::bind(&LBSrv::_OnServerRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server->OnDisc(std::bind(&LBSrv::_OnServerDisc, shared_from_this(), std::placeholders::_1));

	return true;
}

bool LBSrv::Start()
{
	server::Listen(_config["LBSrv"]["port"].value_or(DEFAULT_PORT));
	return true;
}

bool LBSrv::Update(intvl_t interval)
{
	return false;
}

void LBSrv::Stop()
{
}

void LBSrv::Release()
{
}

void LBSrv::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("LBSrv::_OnServerConn success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void LBSrv::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
}

void LBSrv::_OnServerDisc(NETID net_id)
{
}
