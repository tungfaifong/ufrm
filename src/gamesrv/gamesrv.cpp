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
	_config["LBSrv"]["timeout"].value_or(0)), 
	_px_client(GAMESRV, _id, _config["Proxy"]["timeout"].value_or(0), _lb_client)
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

	_lb_client.Init(server,
		[self = shared_from_this()](){ return 1; }, // TODO
		[self = shared_from_this()](NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port){
			self->_px_client.OnNodePublish(node_type, node_id, publish_type, ip, port);
		});

	_px_client.Init(server);

	return true;
}

bool GameSrv::Start()
{
	server::Listen(_config["GameSrv"]["port"].value_or(DEFAULT_PORT));
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
	_px_client.Release();
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
	LOGGER_TRACE("recv msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()), SSID_Name(head.id()), head.rpc_id());
	switch (head.msg_type())
	{
	case SSPkgHead::NORMAL:
		{
			_OnServerHandeNormal(net_id, head, body);
		}
		break;
	case SSPkgHead::RPCREQ:
		{
			_OnServerHanleRpcReq(net_id, head, body);
		}
		break;
	case SSPkgHead::RPCRSP:
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
	if(_nid2gateway.find(net_id) != _nid2gateway.end())
	{
		_gateways.erase(_nid2gateway[net_id]);
		_nid2gateway.erase(net_id);
	}

	_px_client.OnDisconnect(net_id);

	LOGGER_INFO("ondisconnect success net_id:{}", net_id);
}

void GameSrv::_SendToGateway(NODEID node_id, SSID id, SSGWGSPkgBody * body, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	if(_gateways.find(node_id) == _gateways.end())
	{
		LOGGER_ERROR("gateway:{} is invalid", node_id);
		return;
	}
	auto net_id = _gateways[node_id];
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_from_node_type(GAMESRV);
	head->set_from_node_id(_id);
	head->set_to_node_type(GATEWAY);
	head->set_to_node_id(node_id);
	head->set_id(id);
	head->set_msg_type(msg_type);
	head->set_rpc_id(rpc_id);
	head->set_proxy_type(SSPkgHead::END);
	pkg.mutable_body()->set_allocated_gwgs_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("pkg size too long, id:{} size:{}", SSID_Name(id), size);
		return;
	}
	server::Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("send msg msg_type:{} id:{} rpc_id:{}", ENUM_NAME(msg_type), SSID_Name(id), rpc_id);
}

void GameSrv::_SendToClient(ROLEID role_id, CSID id, CSPkgBody * body)
{
	if(_roles.find(role_id) == _roles.end())
	{
		return;
	}
	PKG_CREATE(pkg, SSGWGSPkgBody);
	pkg->mutable_forward_cs_pkg()->set_role_id(role_id);
	pkg->mutable_forward_cs_pkg()->set_game_id(_id);
	pkg->mutable_forward_cs_pkg()->mutable_cs_pkg()->mutable_head()->set_id(id);
	pkg->mutable_forward_cs_pkg()->mutable_cs_pkg()->set_allocated_body(body);
	_SendToGateway(_roles[role_id], SSID_GS_GW_FORWAR_SC_PKG, pkg);
}

void GameSrv::_SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	_px_client.SendToProxy(node_type, node_id, id, body, proxy_id, msg_type, rpc_id);
}

void GameSrv::_BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */)
{
	_px_client.BroadcastToProxy(node_type, id, body, proxy_id);
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
			_OnRecvGateway(net_id, head, body.gwgs_body());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void GameSrv::_OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	case GATEWAY:
		{
			_OnRecvGatewayRpc(net_id, head, body.gwgs_body());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void GameSrv::_OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CoroResult::SUCCESS, std::move(body.SerializePartialAsString()));
}

void GameSrv::_OnRecvGateway(NETID net_id, const SSPkgHead & head, const SSGWGSPkgBody & body)
{
	switch(head.id())
	{
	case SSID_GW_GS_INIT:
		{
			_OnGatewayInit(net_id, head, body.init());
		}
		break;
	case SSID_GW_GS_FORWAR_CS_PKG:
		{
			_OnRecvClient(net_id, body.forward_cs_pkg());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), SSID_Name(head.id()));
		break;
	}
}

void GameSrv::_OnRecvGatewayRpc(NETID net_id, const SSPkgHead & head, const SSGWGSPkgBody & body)
{
	PKG_CREATE(rsp_body, SSGWGSPkgBody);
	SSID id;
	switch(head.id())
	{
	case SSID_GW_GS_HEART_BEAT_REQ:
		{
			_OnGatewayHeartBeatReq(net_id, head, body.heart_beat_req(), id, rsp_body);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), SSID_Name(head.id()));
		break;
	}
	_SendToGateway(head.from_node_id(), id, rsp_body, SSPkgHead::RPCRSP, head.rpc_id());
}

void GameSrv::_OnRecvClient(NETID net_id, const SSGWGSForwardCSPkg & pkg)
{
	auto gateway_node_id = _nid2gateway[net_id];
	switch(pkg.cs_pkg().head().id())
	{
	case CSID_LOGIN_REQ:
		{
			_roles[pkg.role_id()] = gateway_node_id;
			PKG_CREATE(cs_body, CSPkgBody);
			cs_body->mutable_login_rsp();
			_SendToClient(pkg.role_id(), SCID_LOGIN_RSP, cs_body);
		}
		break;
	case CSID_LOGOUT_REQ:
		{
			if(_roles.find(pkg.role_id()) != _roles.end())
			{
				_roles.erase(pkg.role_id());
			}
		}
		break;
	case CSID_HEART_BEAT_REQ:
		{
			PKG_CREATE(cs_body, CSPkgBody);
			cs_body->mutable_heart_beat_rsp();
			_SendToClient(pkg.role_id(), SCID_HEART_BEAT_RSP, cs_body);
		}
		break;
	default:
		break;
	}
	auto lua = std::dynamic_pointer_cast<LuaUnit>(UnitManager::Instance()->Get("LUA"));
	lua->OnRecv(net_id, pkg.SerializePartialAsString().c_str(), (uint16_t)pkg.ByteSizeLong());
}

void GameSrv::_OnGatewayInit(NETID net_id, const SSPkgHead & head, const SSGWGSInit & body)
{
	auto node_id = head.from_node_id();
	_nid2gateway[net_id] = node_id;
	_gateways[node_id] = net_id;
}

void GameSrv::_OnGatewayHeartBeatReq(NETID net_id, const SSPkgHead & head, const SSGWGSHertBeatReq & body, SSID & id, SSGWGSPkgBody * rsp_body)
{
	id = SSID_GS_GW_HEART_BEAT_RSP;
}
