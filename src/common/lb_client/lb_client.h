// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_LB_CLIENT_H
#define UFRM_LB_CLIENT_H

#include "usrv/units/server_unit.h"

#include "protocol/ss.pb.h"

using namespace usrv;

class LBClient
{
public:
	LBClient() = default;
	~LBClient() = default;

	bool Init(std::shared_ptr<ServerUnit> server);

public:
	bool Connect(IP srv_ip, PORT srv_port, uint32_t timeout);
	void RegisterToLBSrv(NODETYPE node_type, IP ip, PORT port);
	void GetAllNodes(NODETYPE node_type);
	void GetLeastLoadNode(NODETYPE node_type);

	void OnRecv(SSPkg * pkg);

	NETID SrvNetId() { return _srv_net_id; }

private:
	bool _SendToLBSrv();

private:
	std::shared_ptr<ServerUnit> _server;
	NETID _srv_net_id { INVALID_NET_ID };
};

#endif // UFRM_LB_CLIENT_H
