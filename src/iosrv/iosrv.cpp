// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "iosrv.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"
#include "define.h"

IOSrv::IOSrv(NODEID id, toml::table & config) : _id(id), _config(config), 
	_lb_client(GATEWAY, _id, _config["IOSrv"]["ip"].value_or(DEFAULT_IP), _config["IOSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0)), 
	_px_client(GATEWAY, _id, _config["Proxy"]["timeout"].value_or(0), _lb_client),
	_db_client(_px_client)
{

}

bool IOSrv::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("iosrv id is INVALID");
		return false;
	}

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));
	_iserver = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("ISERVER"));

	_server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	_server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	_server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });
	_iserver->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnIServerConn(net_id, ip, port); });
	_iserver->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnIServerRecv(net_id, data, size); });
	_iserver->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnIServerDisc(net_id); });

	_lb_client.Init(_iserver,
		[self = shared_from_this()](){ return 0; },
		[self = shared_from_this()](NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port) {
			self->_px_client.OnNodePublish(node_type, node_id, publish_type, ip, port);
		});
	
	_px_client.Init(_iserver);

	return true;
}

bool IOSrv::Start()
{
	_server->Listen(_config["IOSrv"]["port"].value_or(DEFAULT_PORT));
	if(!CoroutineMgr::Instance()->Start())
	{
		return false;
	}
	if(!_lb_client.Start())
	{
		return false;
	}
	if(!_px_client.Start())
	{
		return false;
	}
	return true;
}

bool IOSrv::Update(intvl_t interval)
{
	return false;
}

void IOSrv::Stop()
{
	CoroutineMgr::Instance()->Stop();
}

void IOSrv::Release()
{
	_server = nullptr;
	_iserver = nullptr;
	_px_client.Release();
	_lb_client.Release();
	Unit::Release();
}

void IOSrv::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("on client connect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void IOSrv::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	CSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	LOGGER_TRACE("recv client msg net_id:{} id:{}", net_id, CSID_Name(head.id()));
}

void IOSrv::_OnServerDisc(NETID net_id)
{
	LOGGER_INFO("on client disconnect success net_id:{}", net_id);
}

void IOSrv::_OnIServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("onconnect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void IOSrv::_OnIServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	LOGGER_TRACE("recv msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()), SSID_Name(head.id()), head.rpc_id());
	switch (head.msg_type())
	{
	case SSPkgHead::NORMAL:
		{
			_OnIServerHandeNormal(net_id, head, body);
		}
		break;
	case SSPkgHead::RPCREQ:
		{
			_OnIServerHanleRpcReq(net_id, head, body);
		}
		break;
	case SSPkgHead::RPCRSP:
		{
			_OnIServerHanleRpcRsp(net_id, head, body);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} msg_type:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()));
		break;
	}
}

void IOSrv::_OnIServerDisc(NETID net_id)
{
	_px_client.OnDisconnect(net_id);

	LOGGER_INFO("ondisconnect success net_id:{}", net_id);
}

void IOSrv::_SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	_px_client.SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id);
}

void IOSrv::_BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */)
{
	_px_client.BroadcastToProxy(node_type, id, body, proxy_id, logic_type);
}

void IOSrv::_OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, body.lcls_body());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void IOSrv::_OnIServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void IOSrv::_OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, std::move(body.SerializeAsString()));
}
