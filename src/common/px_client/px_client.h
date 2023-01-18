// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_PX_CLIENT_H
#define UFRM_PX_CLIENT_H

#include "usrv/units/server_unit.h"

#include "consistent_hash/consistent_hash.hpp"
#include "coroutine/coroutine_mgr.h"
#include "lb_client/lb_client.h"
#include "protocol/ss.pb.h"
#include "common.h"

using namespace usrv;

class PXClient
{
public:
	struct Config
	{
		NODETYPE node_type;
		NODEID node_id;
		uint32_t timeout;
	};

	static constexpr intvl_t HEART_BEAT_INTERVAL = 30000;

	PXClient(NODETYPE node_type, NODEID node_id, uint32_t timeout, LBClient & lb_client);
	~PXClient() = default;

	bool Init(std::shared_ptr<ServerUnit> server);
	bool Start();
	void Release();

public:
	void SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	void BroadcastToProxy(NODETYPE node_type, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);
	awaitable_func RpcProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP);

	void OnDisconnect(NETID net_id);
	void OnNodePublish(NODETYPE node_type, NODEID node_id, SSLSLCPublish::PUBLISHTYPE publish_type, IP ip, PORT port);

private:
	void _SendToProxy(NODETYPE node_type, NODEID node_id, SSID id, google::protobuf::Message * body, NODEID proxy_id = INVALID_NODE_ID, SSPkgHead::PROXYTYPE proxy_type = SSPkgHead::END, SSPkgHead::LOGICTYPE logic_type = SSPkgHead::CPP, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	awaitable_func _RpcProxy(NODEID node_id, SSID id, google::protobuf::Message * body);

	future<> _ConnectToProxys();
	bool _ConnectToProxy(NODEID node_id, IP ip, PORT port);
	bool _DisconnectToProxy(NODEID node_id);

	void _HeartBeat();
	future<> _CoroHeartBeat(NODEID node_id);

private:
	NODEID _GetConsistentHashProxy();

private:
	Config _config;
	std::shared_ptr<ServerUnit> _server;
	LBClient & _lb_client;
	TIMERID _timer_heart_beat {INVALID_TIMER_ID};

	std::unordered_map<NETID, NODEID> _nid2proxy;
	std::unordered_map<NODEID, NETID> _proxys;

	ConsistentHash _consistent_hash;
};

#endif // UFRM_PX_CLIENT_H
