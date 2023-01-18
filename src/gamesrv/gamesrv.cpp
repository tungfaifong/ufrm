// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gamesrv.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"
#include "define.h"

GameSrv::GameSrv(NODEID id, toml::table & config) : _id(id), _config(config), 
	_lb_client(GAMESRV, _id, _config["GameSrv"]["ip"].value_or(DEFAULT_IP), _config["GameSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0))
{

}

bool GameSrv::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("gamesrv id is INVALID");
		return false;
	}

	auto server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });

	_lb_client.Init(server, [self = shared_from_this()](){ return 1; }); // TODO

	return true;
}

bool GameSrv::Start()
{	
	if(!CoroutineMgr::Instance()->Start())
	{
		return false;
	}
	if(!_lb_client.Start())
	{
		return false;
	}
	return true;
}

bool GameSrv::Update(intvl_t interval)
{
	return false;
}

void GameSrv::Stop()
{
	CoroutineMgr::Instance()->Stop();
}

void GameSrv::Release()
{
	_lb_client.Release();
	Unit::Release();
}

void GameSrv::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("onconnect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void GameSrv::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	LOGGER_TRACE("recv msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()), ENUM_NAME(head.id()), head.rpc_id());
	switch (head.msg_type())
	{
	case MSGT_NORMAL:
		{
			_OnServerHandeNormal(net_id, head, body);
		}
		break;
	case MSGT_RPCRSP:
		{
			_OnServerHanleRpcRsp(net_id, head, body);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} msg_type:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()));
		break;
	}
}

void GameSrv::_OnServerDisc(NETID net_id)
{
	LOGGER_INFO("ondisconnect success net_id:{}", net_id);
}

void GameSrv::_OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, body.lcls_body());
		}
		break;
	case GATEWAY:
		{
			
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void GameSrv::_OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, std::move(body.SerializePartialAsString()));
}
