// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lb_client.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "define.h"

LBClient::LBClient(NODETYPE node_type, NODEID node_id, IP ip, PORT port, NODEID srv_node_id, IP srv_ip, PORT srv_port, uint32_t timeout) : 
	_config(node_type, node_id, ip, port, srv_node_id, srv_ip, srv_port, timeout)
{

}

bool LBClient::Init(std::shared_ptr<ServerUnit> server, std::function<uint32_t()> load)
{
	_server = server;
	_load = load;
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

void LBClient::GetAllNodes(NODETYPE node_type)
{
	
}

future<LBClient::Node> LBClient::GetLeastLoadNode(NODETYPE node_type)
{
	PKG_CREATE(body, SSLCLSPkgBody);
	body->mutable_get_least_load_node_req()->set_node_type(node_type);
	auto [result, data] = co_await _RpcLBSrv(SSID_LC_LS_GET_LEAST_LOAD_NODE_REQ, body);
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("LBClient::GetLeastLoadNode timeout");
		co_return Node{DEFAULT_IP, DEFAULT_PORT};
	}
	SSLCLSPkgBody rsp_body;
	rsp_body.MergeFromString(data);
	auto rsp = rsp_body.get_least_load_node_rsp();
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
		LOGGER_WARN("LBClient::OnRecv WARN: invalid node_type:{} node_id:{} id:{}", head.from_node_type(), head.from_node_id(), head.id());
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
	switch(body.change_type())
	{
	case SSLSLCPublish::REGISTER:
	case SSLSLCPublish::CHANGE:
		{
			_nodes[node_type][node_id] = Node {body.node().ip(), (PORT)body.node().port()};
		}
		break;
	case SSLSLCPublish::UNREGISTER:
		{
			if(_nodes[node_type].find(node_id) == _nodes[node_type].end())
			{
				return;
			}
			_nodes[node_type].erase(node_id);
		}
		break;
	default:
		break;
	}
}

void LBClient::_SendToLBSrv(SSLCLSID id, SSLCLSPkgBody * body, MSGTYPE msg_type /* = MSGT_NORMAL */, size_t rpc_id /* = -1 */)
{
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_from_node_type(_config.node_type);
	head->set_from_node_id(_config.node_id);
	head->set_to_node_type(LBSRV);
	head->set_to_node_id(_config.srv_node_id);
	head->set_id(id);
	head->set_msg_type(msg_type);
	head->set_rpc_id(rpc_id);
	pkg.mutable_body()->set_allocated_lcls_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("LBClient::_SendToLBSrv ERROR: pkg size too long, id:{} size:{}", id, size);
		return;
	}
	_server->Send(_srv_net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("LBClient::_SendToLBSrv msg_type:{} id:{} rpc_id:{}", msg_type, id, rpc_id);
}

awaitable_func LBClient::_RpcLBSrv(SSLCLSID id, SSLCLSPkgBody * body)
{
	return awaitable_func([this, id, body](COROID coro_id){ _SendToLBSrv(id, body, MSGT_RPCREQ, coro_id); });
}

bool LBClient::_Connect()
{
	_srv_net_id = _server->Connect(_config.srv_ip, _config.srv_port, _config.timeout);
	if(_srv_net_id == INVALID_NET_ID)
	{
		LOGGER_ERROR("LBClient::_Connect Connect to LBSrv failed, srv_ip:{} srv_port:{}", _config.srv_ip, _config.srv_port);
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
		LOGGER_WARN("LBClient::_CoroHeartBeat timeout");
		co_return;
	}
	LOGGER_INFO("LBClient::_CoroHeartBeat RSP success");
}