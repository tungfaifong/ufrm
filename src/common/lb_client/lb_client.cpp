// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lb_client.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/timer_interface.h"

LBClient::LBClient(NODETYPE node_type, NODEID node_id) : _node_type(node_type), _node_id(node_id)
{

}

bool LBClient::Init(NODEID srv_node_id, std::shared_ptr<ServerUnit> server, std::function<uint32_t()> load)
{
	_srv_node_id = srv_node_id;
	_server = server;
	_load = load;
	return true;
}

bool LBClient::Start()
{
	_HeartBeat();
	return true;
}

bool LBClient::Connect(IP srv_ip, PORT srv_port, uint32_t timeout)
{
	_srv_net_id = _server->Connect(srv_ip, srv_port, timeout);
	if(_srv_net_id == INVALID_NET_ID)
	{
		return false;
	}
	return true;
}

void LBClient::RegisterToLBSrv(NODETYPE node_type, IP ip, PORT port)
{
	CREATE_PKG(body, SSLCLSPkgBody);
	auto node_register = body->mutable_node_register();
	node_register->set_node_type(node_type);
	node_register->set_ip(ip);
	node_register->set_port(port);
	_SendToLBSrv(SSID_LC_LS_NODE_REGISTER, body);
}

void LBClient::GetAllNodes(NODETYPE node_type)
{
	
}

void LBClient::GetLeastLoadNode(NODETYPE node_type)
{

}

NETID LBClient::SrvNetId()
{
	return _srv_net_id;
}

void LBClient::_SendToLBSrv(SSLCLSID id, SSLCLSPkgBody * body, MSGTYPE msg_type /* = MSGT_NORMAL */, size_t rpc_id /* = -1 */)
{
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_from_node_type(_node_type);
	head->set_from_node_id(_node_id);
	head->set_to_node_type(LBSRV);
	head->set_to_node_id(_srv_node_id);
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
}

void LBClient::_HeartBeat()
{
	CoroutineMgr::Instance()->Spawn(std::bind(&LBClient::_CoroHeartBeat, shared_from_this()));
	_timer_heart_beat = timer::CreateTimer(HEART_BEAT_INTERVAL, std::bind(&LBClient::_HeartBeat, shared_from_this()));
}

coroutine LBClient::_CoroHeartBeat()
{
	CREATE_PKG(body, SSLCLSPkgBody);
	body->mutable_heart_beat_req()->set_load(_load());
	auto [result, data] = co_await awaitable(std::bind(&LBClient::_SendToLBSrv, shared_from_this(), SSID_LC_LS_HEAT_BEAT_REQ, body, MSGT_RPCREQ, std::placeholders::_1));
	if(result == CORORESULT::TIMEOUT)
	{
		LOGGER_WARN("LBClient::_CoroHeartBeat timeout");
		co_return;
	}
	LOGGER_INFO("LBClient::_CoroHeartBeat RSP success");
}