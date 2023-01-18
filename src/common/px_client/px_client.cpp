// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "px_client.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "define.h"

PXClient::PXClient(NODETYPE node_type, NODEID node_id, uint32_t timeout, LBClient & lb_client) : 
	_config(node_type, node_id, timeout), _lb_client(lb_client)
{

}

bool PXClient::Init(std::shared_ptr<ServerUnit> server)
{
	_server = server;
	return true;
}

bool PXClient::Start()
{
	CO_SPAWN(_ConnectToProxys());
	return true;
}

void PXClient::Release()
{
	_server = nullptr;
}

void PXClient::SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	if(proxy_id == INVALID_NODE_ID)
	{
		proxy_id = _GetConsistentHashProxy();
	}
	_SendToProxy(node_type, node_id, id, body, proxy_id, SSPkgHead::FORWARD, msg_type, rpc_id);
}

void PXClient::BroadcastToProxy(NODETYPE node_type, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */)
{
	if(proxy_id == INVALID_NODE_ID)
	{
		proxy_id = _GetConsistentHashProxy();
	}
	_SendToProxy(node_type, INVALID_NODE_ID, id, body, proxy_id, SSPkgHead::BROADCAST);
}

void PXClient::OnDisconnect(NETID net_id)
{
	if(_nid2proxy.find(net_id) != _nid2proxy.end())
	{
		auto node_id = _nid2proxy[net_id];
		_proxys.erase(node_id);
		_nid2proxy.erase(net_id);
		_consistent_hash.RemoveNode(node_id);
	}
}

void PXClient::OnNodePublish(NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port)
{
	if(node_type != PROXY)
	{
		return;
	}
	switch(publish_type)
	{
	case SSLSLCPublish::REGISTER:
		{
			if(_ConnectToProxy(node_id, ip, port))
			{
				_consistent_hash.AddNode(node_id);
			}
		}
		break;
	case SSLSLCPublish::CHANGE:
		{
			_DisconnectToProxy(node_id);
			if(_ConnectToProxy(node_id, ip, port))
			{
				_consistent_hash.AddNode(node_id);
			}
		}
		break;
	case SSLSLCPublish::UNREGISTER:
		{
			_DisconnectToProxy(node_id);
		}
		break;
	default:
		break;
	}
}

void PXClient::_SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, SSPkgBody * body, NODEID proxy_id /* = INVALID_NODE_ID */, SSPkgHead::PROXYTYPE proxy_type /* = SSPkgHead::END */, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	if(_proxys.find(proxy_id) == _proxys.end())
	{
		LOGGER_ERROR("proxy:{} is invalid", proxy_id);
		return;
	}
	auto net_id = _proxys[proxy_id];
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_from_node_type(_config.node_type);
	head->set_from_node_id(_config.node_id);
	head->set_to_node_type(node_type);
	head->set_to_node_id(node_id);
	head->set_id(id);
	head->set_msg_type(msg_type);
	head->set_rpc_id(rpc_id);
	head->set_proxy_type(proxy_type);
	pkg.set_allocated_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("pkg size too long, id:{} size:{}", SSID_Name(id), size);
		return;
	}
	_server->Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("send msg msg_type:{} id:{} rpc_id:{}", ENUM_NAME(msg_type), SSID_Name(id), rpc_id);
}

awaitable_func PXClient::_RpcProxy(NODEID node_id, SSID id, SSPkgBody * body)
{
	return awaitable_func([this, node_id, id, body](COROID coro_id){ _SendToProxy(PROXY, node_id, id, body, node_id, SSPkgHead::END, SSPkgHead::RPCREQ, coro_id); });
}

future<> PXClient::_ConnectToProxys()
{
	auto proxys = co_await _lb_client.GetAllNodes(PROXY);
	for(const auto & [node_id, proxy] : proxys)
	{
		if(_ConnectToProxy(node_id, proxy.ip, proxy.port))
		{
			_consistent_hash.AddNode(node_id);
		}
	}
	_HeartBeat();
	_lb_client.Subscribe(PROXY);
}

bool PXClient::_ConnectToProxy(NODEID node_id, IP ip, PORT port)
{
	auto net_id = _server->Connect(ip, port, _config.timeout);
	if(net_id == INVALID_NET_ID)
	{
		LOGGER_WARN("Connect to Proxy failed, srv_ip:{} srv_port:{}", ip, port);
		return false;
	}
	_nid2proxy[net_id] = node_id;
	_proxys[node_id] = net_id;

	PKG_CREATE(body, SSPkgBody);
	_SendToProxy(PROXY, node_id, SSID_PC_PX_NODE_REGISTER, body, node_id);

	return true;
}

bool PXClient::_DisconnectToProxy(NODEID node_id)
{
	if(_proxys.find(node_id) == _proxys.end())
	{
		return false;
	}
	_server->Disconnect(_proxys[node_id]);
	return true;
}

void PXClient::_HeartBeat()
{
	for(auto & [node_id, net_id] : _proxys)
	{
		CO_SPAWN(_CoroHeartBeat(node_id));
	}
	_timer_heart_beat = timer::CreateTimer(HEART_BEAT_INTERVAL, [this](){ _HeartBeat(); });
}

future<> PXClient::_CoroHeartBeat(NODEID node_id)
{
	PKG_CREATE(body, SSPkgBody);
	auto [result, data] = co_await _RpcProxy(node_id, SSID_PC_PX_HEART_BEAT_REQ, body);
	if(result == CoroResult::TIMEOUT)
	{
		LOGGER_WARN("heart beat timeout");
		co_return;
	}
	LOGGER_INFO("heart beat RSP success");
}

NODEID PXClient::_GetConsistentHashProxy()
{
	return _consistent_hash.GetNode(fmt::format("{}#{}", ENUM_NAME(_config.node_type), std::to_string(_config.node_id)));
}
