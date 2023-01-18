// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gamesrv.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/pblua.h"
#include "protocol/ss.pb.h"
#include "define.h"

GameSrv::GameSrv(NODEID id, toml::table & config) : _id(id), _config(config), 
	_lb_client(GAMESRV, _id, _config["GameSrv"]["ip"].value_or(DEFAULT_IP), _config["GameSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0)), 
	_px_client(GAMESRV, _id, _config["Proxy"]["timeout"].value_or(0), _lb_client),
	_db_client(_px_client)
{

}

bool GameSrv::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("gamesrv id is INVALID");
		return false;
	}

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	_server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	_server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	_server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });

	_lb_client.Init(_server,
		[self = shared_from_this()](){ return self->_roles.size(); },
		[self = shared_from_this()](NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port){
			self->_px_client.OnNodePublish(node_type, node_id, publish_type, ip, port);
		});

	_px_client.Init(_server);

	return true;
}

bool GameSrv::Start()
{
	_server->Listen(_config["GameSrv"]["port"].value_or(DEFAULT_PORT));
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
	if(!std::dynamic_pointer_cast<LuaUnit>(UnitManager::Instance()->Get("LUA"))->InitFunc(_lua_on_recv_pkg, "OnRecvPkg"))
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
	_lua_on_recv_pkg = nullptr;
}

void GameSrv::Release()
{
	_px_client.Release();
	_lb_client.Release();
	Unit::Release();
}

void GameSrv::SendToClient(ROLEID role_id, CSID id, CSPkgBody * body)
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

void GameSrv::SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	_px_client.SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id);
}

void GameSrv::BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */)
{
	_px_client.BroadcastToProxy(node_type, id, body, proxy_id, logic_type);
}

future<std::vector<std::unordered_map<std::string, variant_t>>> GameSrv::DBSelect(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::unordered_map<std::string, variant_t> & where)
{
	auto ret = co_await _db_client.Select(node_id, tb_name, column, where);
	co_return ret;
}

future<bool> GameSrv::DBInsert(NODEID node_id, const std::string & tb_name, const std::vector<std::string> & column, const std::vector<variant_t> & value)
{
	auto ret = co_await _db_client.Insert(node_id, tb_name, column, value);
	co_return ret;
}

future<bool> GameSrv::DBUpdate(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & value, const std::unordered_map<std::string, variant_t> & where)
{
	auto ret = co_await _db_client.Update(node_id, tb_name, value, where);
	co_return ret;
}

future<bool> GameSrv::DBDelete(NODEID node_id, const std::string & tb_name, const std::unordered_map<std::string, variant_t> & where)
{
	auto ret = co_await _db_client.Delete(node_id, tb_name, where);
	co_return ret;
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
	TRACE_MSG("recv ss", pkg);
	if(head.logic_type() == SSPkgHead::CPP || head.logic_type() == SSPkgHead::BOTH)
	{
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

	if(head.logic_type() == SSPkgHead::LUA || head.logic_type() == SSPkgHead::BOTH)
	{
		auto lua = std::dynamic_pointer_cast<LuaUnit>(UnitManager::Instance()->Get("LUA"));
		try
		{
			(*_lua_on_recv_pkg)(net_id, pblua::PB2LuaRef(pkg, lua->GetLuaState()));
		}
		catch(const luabridge::LuaException & e)
		{
			lua->OnException(e);
		}
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
	SEND_SSPKG(_server, net_id, GAMESRV, _id, GATEWAY, node_id, id, msg_type, rpc_id, SSPkgHead::END, SSPkgHead::CPP, mutable_body()->set_allocated_gwgs_body, body);
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
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, std::move(body.SerializeAsString()));
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
			if(_roles.find(pkg.role_id()) != _roles.end())
			{
				LOGGER_WARN("CSID_LOGIN_REQ role:{} repeated old_gateway_node_id:{} new_gateway_node_id:{}", pkg.role_id(), _roles[pkg.role_id()], gateway_node_id);
			}
			_roles[pkg.role_id()] = gateway_node_id;
		}
		break;
	case CSID_LOGOUT_REQ:
		{
			if(_roles.find(pkg.role_id()) != _roles.end())
			{
				_roles.erase(pkg.role_id());
			}
			else
			{
				LOGGER_WARN("CSID_LOGOUT_REQ invalid role:{}", pkg.role_id());
			}
		}
		break;
	default:
		break;
	}
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
