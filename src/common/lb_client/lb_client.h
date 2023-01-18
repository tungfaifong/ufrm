// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_LB_CLIENT_H
#define UFRM_LB_CLIENT_H

#include "usrv/units/server_unit.h"

#include "common.h"
#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"

using namespace usrv;

class LBClient : public std::enable_shared_from_this<LBClient>
{
public:
	static constexpr intvl_t HEART_BEAT_INTERVAL = 30000;

	LBClient(NODETYPE node_type, NODEID node_id);
	~LBClient() = default;

	bool Init(NODEID srv_node_id, std::shared_ptr<ServerUnit> server, std::function<uint32_t()> load);
	bool Start();

public:
	bool Connect(IP srv_ip, PORT srv_port, uint32_t timeout);
	void RegisterToLBSrv(NODETYPE node_type, IP ip, PORT port);
	void GetAllNodes(NODETYPE node_type);
	void GetLeastLoadNode(NODETYPE node_type);

	NETID SrvNetId();

private:
	void _SendToLBSrv(SSLCLSID id, SSLCLSPkgBody * body, MSGTYPE msg_type = MSGT_NORMAL, size_t rpc_id = -1);

	void _HeartBeat();
	coroutine _CoroHeartBeat();

private:
	NODETYPE _node_type;
	NODEID _node_id;
	std::shared_ptr<ServerUnit> _server;
	NODEID _srv_node_id;
	NETID _srv_net_id { INVALID_NET_ID };
	TIMERID _timer_heart_beat {INVALID_TIMER_ID};
	std::function<uint32_t()> _load;
};

#endif // UFRM_LB_CLIENT_H
