// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "commonsrv.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/pblua.h"
#include "protocol/ss.pb.h"
#include "define.h"

CommonSrv::CommonSrv(NODETYPE type, NODEID id, toml::table & config) : _type(type), _id(id), _config(config), 
	_lb_client(_type, _id, _config["CommonSrv"]["ip"].value_or(DEFAULT_IP), _config["CommonSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0)), 
	_px_client(_type, _id, _config["Proxy"]["timeout"].value_or(0), _lb_client)
{

}

bool CommonSrv::Init()
{
	if(_type == INVALID_NODE_TYPE || _id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("commonsrv type or id is INVALID");
		return false;
	}

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	_server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	_server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	_server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });

	_lb_client.Init(_server,
		[self = shared_from_this()](){ return 0; },
		[self = shared_from_this()](NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port) {
			self->_px_client.OnNodePublish(node_type, node_id, publish_type, ip, port);
		});
	
	_px_client.Init(_server);

	return true;
}

bool CommonSrv::Start()
{
	_server->Listen(_config["CommonSrv"]["port"].value_or(DEFAULT_PORT));
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

bool CommonSrv::Update(intvl_t interval)
{
	return false;
}

void CommonSrv::Stop()
{
	CoroutineMgr::Instance()->Stop();
}

void CommonSrv::Release()
{
	_server = nullptr;
	_px_client.Release();
	_lb_client.Release();
	Unit::Release();
}

void CommonSrv::SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	_px_client.SendToProxy(node_type, node_id, id, body, proxy_id, logic_type, msg_type, rpc_id);
}

void CommonSrv::BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::LOGICTYPE logic_type /* = SSPkgHead::CPP */)
{
	_px_client.BroadcastToProxy(node_type, id, body, proxy_id, logic_type);
}

void CommonSrv::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("onconnect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void CommonSrv::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	TraceMsg("recv ss", &pkg);
	if(head.logic_type() == SSPkgHead::CPP || head.logic_type() == SSPkgHead::BOTH)
	{
		switch (head.msg_type())
		{
		case SSPkgHead::NORMAL:
			{
				_OnServerHandeNormal(net_id, head, pkg.data());
			}
			break;
		case SSPkgHead::RPCREQ:
			{
				_OnServerHanleRpcReq(net_id, head, pkg.data());
			}
			break;
		case SSPkgHead::RPCRSP:
			{
				_OnServerHanleRpcRsp(net_id, head, pkg.data());
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

void CommonSrv::_OnServerDisc(NETID net_id)
{
	_px_client.OnDisconnect(net_id);

	LOGGER_INFO("ondisconnect success net_id:{}", net_id);
}

void CommonSrv::_OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, data);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void CommonSrv::_OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const std::string & data)
{
}

void CommonSrv::_OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const std::string & data)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, data);
}
