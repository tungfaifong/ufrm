// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_LB_CLIENT_H
#define UFRM_LB_CLIENT_H

#include "usrv/units/server_unit.h"

#include "common.h"
#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"

using namespace usrv;

class LBClient
{
public:
	struct Config
	{
		NODETYPE node_type;
		NODEID node_id;
		IP ip;
		PORT port;
		NODEID srv_node_id;
		IP srv_ip;
		PORT srv_port;
		uint32_t timeout;
	};

	struct Node
	{
		IP ip;
		PORT port;
	};

	static constexpr intvl_t HEART_BEAT_INTERVAL = 30000;

	LBClient(NODETYPE node_type, NODEID node_id, IP ip, PORT port, NODEID srv_node_id, IP srv_ip, PORT srv_port, uint32_t timeout);
	~LBClient() = default;

	bool Init(std::shared_ptr<ServerUnit> server, std::function<uint32_t()> load);
	bool Start();
	void Release();

public:
	void RegisterToLBSrv(NODETYPE node_type, NODEID node_id, IP ip, PORT port);
	void UnregisterToLBSrv(NODETYPE node_type, NODEID node_id);
	void Subscribe(NODETYPE node_type);
	void GetAllNodes(NODETYPE node_type);
	coroutine GetLeastLoadNode(NODETYPE node_type);

	void OnRecv(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);

private:
	void _OnPublish(NETID net_id, const SSPkgHead & head, const SSLSLCPublish & body);

private:
	void _SendToLBSrv(SSLCLSID id, SSLCLSPkgBody * body, MSGTYPE msg_type = MSGT_NORMAL, size_t rpc_id = -1);
	awaitable _RpcLBSrv(SSLCLSID id, SSLCLSPkgBody * body);

	bool _Connect();
	void _HeartBeat();
	coroutine _CoroHeartBeat();

private:
	Config _config;
	std::shared_ptr<ServerUnit> _server;
	NETID _srv_net_id { INVALID_NET_ID };
	TIMERID _timer_heart_beat {INVALID_TIMER_ID};
	std::function<uint32_t()> _load;
	std::unordered_map<NODEID, Node> _nodes[NODETYPE_ARRAYSIZE];
};

#endif // UFRM_LB_CLIENT_H
