// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lb_client.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "define.h"

LBClient::LBClient(NODETYPE node_type, NODEID node_id, IP ip, PORT port, NODEID srv_node_id, IP srv_ip, PORT srv_port, uint32_t timeout) : 
	_config(node_type, node_id, ip, port, srv_node_id, srv_ip, srv_port, timeout)
{

}

bool LBClient::Init(std::shared_ptr<ServerUnit> server, LoadFunc load, OnNodePublishFunc on_node_publish /* = nullptr */)
{
	_server = server;
	_load = load;
	_on_node_publish = on_node_publish;
	return true;
}

bool LBClient::Start()
{
	if(!_Connect())
	{
		return false;
	}
	RegisterToLBSrv(_config.node_type, _config.node_id, _config.ip, _config.port);
	_HeartBeat();
	return true;
}

void LBClient::Release()
{
	_server = nullptr;
	_load = nullptr;
	_on_node_publish = nullptr;
}

void LBClient::RegisterToLBSrv(NODETYPE node_type, NODEID node_id, IP ip, PORT port)
{
	PKG_CREATE(body, SSLCLSPkgBody);
	auto node_register = body->mutable_node_register();
	node_register->mutable_node()->set_node_type(node_type);
	node_register->mutable_node()->set_node_id(node_id);
	node_register->mutable_node()->set_ip(ip);
	node_register->mutable_node()->set_port(port);
	_SendToLBSrv(SSID_LC_LS_NODE_REGISTER, body);
}

void LBClient::UnregisterToLBSrv(NODETYPE node_type, NODEID node_id)
{
	PKG_CREATE(body, SSLCLSPkgBody);
	auto node_unregister = body->mutable_node_unregister();
	node_unregister->set_node_type(node_type);
	node_unregister->set_node_id(node_id);
	_SendToLBSrv(SSID_LC_LS_NODE_UNREGISTER, body);
}

void LBClient::Subscribe(NODETYPE node_type)
{
	PKG_CREATE(body, SSLCLSPkgBody);
	auto subscribe = body->mutable_subscribe();
	subscribe->set_node_type(node_type);
	_SendToLBSrv(SSID_LC_LS_SUBSCRIBE, body);
}

future<const std::unordered_map<NODEID, LBClient::Node> &> LBClient::GetAllNodes(NODETYPE node_type)
{
	if(_nodes[node_type].empty())
	{
		PKG_CREATE(body, SSLCLSPkgBody);
		body->mutable_get_all_nodes_req()->set_node_type(node_type);
		auto [result, data] = co_await _RpcLBSrv(SSID_LC_LS_GET_ALL_NODES_REQ, body);
		if(result == CORORESULT::TIMEOUT)
		{
			LOGGER_WARN("get all nodes timeout");
			co_return _nodes[node_type];
		}
		SSPkgBody rsp_body;
		rsp_body.ParseFromString(data);
		auto rsp = rsp_body.lcls_body().get_all_nodes_rsp();
		for(auto & node : rsp.nodes())
		{
			_nodes[node_type][node.node_id()] = Node{node.ip(), (PORT)node.port()};
		}
	}
	co_return _nodes[node_type];
}

future<LBClient::Node> LBClient::GetLeastLoadNode(NODETYPE node_type)
{
	PKG_CREATE(body, SSLCLSPkgBody);
	body->mutable_get_least_load_node_req()->set_node_type(node_type);
	auto [result, data] = co_await _RpcLBSrv(SSID_LC_LS_GET_LEAST_LOAD_NODE_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("get least load node timeout");
		co_return Node{DEFAULT_IP, DEFAULT_PORT};
	}
	SSPkgBody rsp_body;
	rsp_body.ParseFromString(data);
	auto rsp = rsp_body.lcls_body().get_least_load_node_rsp();
	co_return Node{rsp.node().ip(), (PORT)rsp.node().port()};
}

void LBClient::OnRecv(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body)
{
	switch(head.id())
	{
	case SSID_LS_LC_PUBLISH:
		{
			_OnPublish(net_id, head, body.publish());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), SSID_Name(head.id()));
		break;
	}
}

void LBClient::_OnPublish(NETID net_id, const SSPkgHead & head, const SSLSLCPublish & body)
{
	auto node_type = body.node().node_type();
	auto node_id = body.node().node_id();
	if(node_type < 0 || node_type >= NODETYPE_ARRAYSIZE)
	{
		return;
	}
	auto publish_type = body.publish_type();
	auto ip = body.node().ip();
	auto port = (PORT)body.node().port();
	switch(publish_type)
	{
	case SSLSLCPublish::REGISTER:
		{
			_nodes[node_type][node_id] = Node {ip, port};
		}
		break;
	case SSLSLCPublish::CHANGE:
		{
			_nodes[node_type][node_id].ip = ip;
			_nodes[node_type][node_id].port = port;
		}
		break;
	case SSLSLCPublish::UNREGISTER:
		{
			if(_nodes[node_type].find(node_id) == _nodes[node_type].end())
			{
				break;
			}
			ip = _nodes[node_type][node_id].ip;
			port = _nodes[node_type][node_id].port;
			_nodes[node_type].erase(node_id);
		}
		break;
	default:
		break;
	}
	if(_on_node_publish)
	{
		_on_node_publish(node_type, node_id, publish_type, ip, port);
	}
}

void LBClient::_SendToLBSrv(SSID id, SSLCLSPkgBody * body, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	SEND_SSPKG(_server, _srv_net_id, _config.node_type, _config.node_id, LBSRV, _config.srv_node_id, id, msg_type, rpc_id, SSPkgHead::END, SSPkgHead::CPP, mutable_body()->set_allocated_lcls_body, body);
}

awaitable_func LBClient::_RpcLBSrv(SSID id, SSLCLSPkgBody * body)
{
	return awaitable_func([this, id, body](COROID coro_id){ _SendToLBSrv(id, body, SSPkgHead::RPCREQ, coro_id); });
}

bool LBClient::_Connect()
{
	_srv_net_id = _server->Connect(_config.srv_ip, _config.srv_port, _config.timeout);
	if(_srv_net_id == INVALID_NET_ID)
	{
		LOGGER_ERROR("Connect to LBSrv failed, srv_ip:{} srv_port:{}", _config.srv_ip, _config.srv_port);
		return false;
	}
	return true;
}

void LBClient::_HeartBeat()
{
	CO_SPAWN(_CoroHeartBeat());
	_timer_heart_beat = timer::CreateTimer(HEART_BEAT_INTERVAL, [this](){ _HeartBeat(); });
}

future<> LBClient::_CoroHeartBeat()
{
	PKG_CREATE(body, SSLCLSPkgBody);
	body->mutable_heart_beat_req()->set_load(_load());
	auto [result, data] = co_await _RpcLBSrv(SSID_LC_LS_HEART_BEAT_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("heart beat timeout");
		co_return;
	}
}