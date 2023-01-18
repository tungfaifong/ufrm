// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lb_client.h"

bool LBClient::Init(std::shared_ptr<ServerUnit> server)
{
	_server = server;
	return true;
}

bool LBClient::Start(IP srv_ip, PORT srv_port, uint32_t timeout)
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

}

void LBClient::GetAllNodes(NODETYPE node_type)
{
	
}

void LBClient::GetLeastLoadNode(NODETYPE node_type)
{

}