// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lb_client.h"

#include "usrv/interfaces/logger_interface.h"

LBClient::LBClient(NODETYPE node_type, NODEID node_id) : _node_type(node_type), _node_id(node_id)
{

}

bool LBClient::Init(NODEID srv_node_id, std::shared_ptr<ServerUnit> server)
{
	_srv_node_id = srv_node_id;
	_server = server;
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
	auto body = std::make_unique<SSLCLSPkgBody>();
	auto node_init = body->mutable_node_init();
	node_init->set_node_type(node_type);
	node_init->set_ip(ip);
	node_init->set_port(port);
	_SendToLBSrv(SSID_LC_LS_NODE_INIT, std::move(body));
}

void LBClient::GetAllNodes(NODETYPE node_type)
{
	
}

void LBClient::GetLeastLoadNode(NODETYPE node_type)
{

}

bool LBClient::_SendToLBSrv(SSLCLSID id, std::unique_ptr<SSLCLSPkgBody> && body)
{
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_from_node_type(_node_type);
	head->set_from_node_id(_node_id);
	head->set_to_node_type(LBSRV);
	head->set_to_node_id(_srv_node_id);
	head->set_id(id);
	head->set_msg_type(NORMAL);
	head->set_rpc_id(0);
	pkg.mutable_body()->set_allocated_lcls_body(body.release());
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("LBClient::_SendToLBSrv ERROR: pkg size too long, id:{} size:{}", id, size);
		return false;
	}
	_server->Send(_srv_net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	return true;
}