// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gateway.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"
#include "define.h"

Gateway::Gateway(NODEID id, toml::table & config) : _id(id), _config(config), 
	_lb_client(GATEWAY, _id, _config["Gateway"]["ip"].value_or(DEFAULT_IP), _config["Gateway"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0))
{

}

bool Gateway::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("gateway id is INVALID");
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
		[self = shared_from_this()](){ return 1; }, // TODO
		[self = shared_from_this()](NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port){
			self->_OnNodePublish(node_type, node_id, publish_type, ip, port);
		}); 

	return true;
}

bool Gateway::Start()
{	
	if(!CoroutineMgr::Instance()->Start())
	{
		return false;
	}
	if(!_lb_client.Start())
	{
		return false;
	}
	CO_SPAWN(_ConnectToGameSrvs());
	return true;
}

bool Gateway::Update(intvl_t interval)
{
	return false;
}

void Gateway::Stop()
{
	CoroutineMgr::Instance()->Stop();
}

void Gateway::Release()
{
	_server = nullptr;
	_iserver = nullptr;
	_lb_client.Release();
	Unit::Release();
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
	LOGGER_INFO("onconnect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void Gateway::_OnIServerRecv(NETID net_id, char * data, uint16_t size)
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
			_OnIServerHandeNormal(net_id, head, body);
		}
		break;
	case MSGT_RPCRSP:
		{
			_OnIServerHanleRpcRsp(net_id, head, body);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} msg_type:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()));
		break;
	}
}

void Gateway::_OnIServerDisc(NETID net_id)
{
	if(_nid2gamesrv.find(net_id) != _nid2gamesrv.end())
	{
		_gamesrvs.erase(_nid2gamesrv[net_id]);
		_nid2gamesrv.erase(net_id);
	}

	LOGGER_INFO("ondisconnect success net_id:{}", net_id);
}

void Gateway::_SendToGameSrv(NODEID node_id, SSID id, SSGWGSPkgBody * body, MSGTYPE msg_type /* = MSGT_NORMAL */, size_t rpc_id /* = -1 */)
{
	if(_gamesrvs.find(node_id) == _gamesrvs.end())
	{
		LOGGER_ERROR("gamesrv:{} is invalid", node_id);
		return;
	}
	auto net_id = _gamesrvs[node_id];
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_from_node_type(GATEWAY);
	head->set_from_node_id(_id);
	head->set_to_node_type(GAMESRV);
	head->set_to_node_id(node_id);
	head->set_id(id);
	head->set_msg_type(msg_type);
	head->set_rpc_id(rpc_id);
	pkg.mutable_body()->set_allocated_gwgs_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("pkg size too long, id:{} size:{}", ENUM_NAME(id), size);
		return;
	}
	_iserver->Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("send msg msg_type:{} id:{} rpc_id:{}", ENUM_NAME(msg_type), ENUM_NAME(id), rpc_id);
}

awaitable_func Gateway::_RpcGameSrv(NODEID node_id, SSID id, SSGWGSPkgBody * body)
{
	return awaitable_func([this, node_id, id, body](COROID coro_id){ _SendToGameSrv(node_id, id, body, MSGT_RPCREQ, coro_id); });
}

void Gateway::_OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, body.lcls_body());
		}
		break;
	case GAMESRV:
		{
			
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id());
		break;
	}
}

void Gateway::_OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, std::move(body.SerializePartialAsString()));
}

future<> Gateway::_ConnectToGameSrvs()
{
	auto game_srvs = co_await _lb_client.GetAllNodes(GAMESRV);
	for(const auto & [node_id, game_srv] : game_srvs)
	{
		_ConnectToGameSrv(node_id, game_srv.ip, game_srv.port);
	}
	_HeartBeat();
}

void Gateway::_OnNodePublish(NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port)
{
	if(node_type != GAMESRV)
	{
		return;
	}
	switch(publish_type)
	{
	case SSLSLCPublish::REGISTER:
		{
			_ConnectToGameSrv(node_id, ip, port);
		}
		break;
	case SSLSLCPublish::CHANGE:
		{
			_DisconnectToGameSrv(node_id);
			_ConnectToGameSrv(node_id, ip, port);
		}
		break;
	case SSLSLCPublish::UNREGISTER:
		{
			_DisconnectToGameSrv(node_id);
		}
		break;
	default:
		break;
	}
}

void Gateway::_ConnectToGameSrv(NODEID node_id, IP ip, PORT port)
{
	auto net_id = _iserver->Connect(ip, port, _config["GameSrv"]["timeout"].value_or(0));
	if(net_id == INVALID_NET_ID)
	{
		LOGGER_WARN("Connect to GameSrv failed, srv_ip:{} srv_port:{}", ip, port);
		return;
	}
	_nid2gamesrv[net_id] = node_id;
	_gamesrvs[node_id] = net_id;

	PKG_CREATE(body, SSGWGSPkgBody);
	_SendToGameSrv(node_id, SSID_GW_GS_INIT, body);
}

void Gateway::_DisconnectToGameSrv(NODEID node_id)
{
	if(_gamesrvs.find(node_id) == _gamesrvs.end())
	{
		return;
	}
	_iserver->Disconnect(_gamesrvs[node_id]);
}

void Gateway::_HeartBeat()
{
	for(auto & [node_id, net_id] : _gamesrvs)
	{
		CO_SPAWN(_CoroHeartBeat(node_id));
	}
	_timer_heart_beat = timer::CreateTimer(HEART_BEAT_INTERVAL, [this](){ _HeartBeat(); });
}

future<> Gateway::_CoroHeartBeat(NODEID node_id)
{
	PKG_CREATE(body, SSGWGSPkgBody);
	auto [result, data] = co_await _RpcGameSrv(node_id, SSID_GW_GS_HEART_BEAT_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("heart beat timeout");
		co_return;
	}
	LOGGER_INFO("heart beat RSP success");
}
