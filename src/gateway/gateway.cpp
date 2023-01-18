// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gateway.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"

Gateway::Gateway(NODEID id, toml::table & config) : _id(id), _config(config), _lb_client(GATEWAY, _id)
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

	_lb_client.Init(_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _iserver, [self = shared_from_this()](){ return 0; });

	return true;
}

bool Gateway::Start()
{
	if(!_lb_client.Connect(_config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), _config["LBSrv"]["timeout"].value_or(0)))
	{
		return false;
	}
	_lb_client.RegisterToLBSrv(GATEWAY, _config["Gateway"]["ip"].value_or(DEFAULT_IP), _config["Gateway"]["port"].value_or(DEFAULT_PORT));
	if(!_lb_client.Start())
	{
		return false;
	}
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
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	switch (head.msg_type())
	{
	case MSGT_NORMAL:
		{
			_OnIServerHandeNormal(net_id, head, body.lcls_body());
		}
		break;
	case MSGT_RPCRSP:
		{
			_OnIServerHanleRpcRsp(net_id, head, body.lcls_body());
		}
		break;
	default:
		LOGGER_WARN("Gateway::_OnIServerRecv WARN: invalid node_type:{} node_id:{} msg_type:{}", head.from_node_type(), head.from_node_id(), head.msg_type());
		break;
	}
}

void Gateway::_OnIServerDisc(NETID net_id)
{
	LOGGER_INFO("Gateway::_OnIServerDisc success net_id:{}", net_id);
}

void Gateway::_OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body)
{
	switch (head.from_node_type())
	{
	case GAMESRV:
		{
			
		}
		break;
	default:
		LOGGER_WARN("Gateway::_OnIServerRecv WARN: invalid node_type:{} node_id:{}", head.from_node_type(), head.from_node_id());
		break;
	}
}

void Gateway::_OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, std::move(body.SerializePartialAsString()));
}
