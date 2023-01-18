// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_LB_CLIENT_H
#define UFRM_LB_CLIENT_H

#include "usrv/units/server_unit.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"
#include "common.h"

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
		NODETYPE node_type;
		NODEID node_id;
		IP ip;
		PORT port;
	};

	static constexpr intvl_t HEART_BEAT_INTERVAL = 30000;

	using LoadFunc = std::function<uint32_t()>;
	using OnNodePublishFunc = std::function<void(NODETYPE, NODEID, SSLSLCPublish::PUBLISHTYPE, IP, PORT)>;

	LBClient(NODETYPE node_type, NODEID node_id, IP ip, PORT port, NODEID srv_node_id, IP srv_ip, PORT srv_port, uint32_t timeout);
	~LBClient() = default;

	bool Init(std::shared_ptr<ServerUnit> server, LoadFunc load, OnNodePublishFunc on_node_publish = nullptr);
	bool Start();
	void Release();

public:
	void RegisterToLBSrv(NODETYPE node_type, NODEID node_id, IP ip, PORT port);
	void UnregisterToLBSrv(NODETYPE node_type, NODEID node_id);
	void Subscribe(NODETYPE node_type);

	future<const std::unordered_map<NODEID, Node> &> GetAllNodes(NODETYPE node_type);
	future<Node> GetLeastLoadNode(NODETYPE node_type);

	void OnRecv(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);

private:
	void _OnPublish(NETID net_id, const SSPkgHead & head, const SSLSLCPublish & body);

private:
	void _SendToLBSrv(SSID id, SSLCLSPkgBody * body, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	awaitable_func _RpcLBSrv(SSID id, SSLCLSPkgBody * body);

	bool _Connect();
	void _HeartBeat();
	future<> _CoroHeartBeat();

private:
	Config _config;
	std::shared_ptr<ServerUnit> _server;
	NETID _srv_net_id { INVALID_NET_ID };
	TIMERID _timer_heart_beat {INVALID_TIMER_ID};
	LoadFunc _load;
	std::unordered_map<NODEID, Node> _nodes[NODETYPE_ARRAYSIZE];
	OnNodePublishFunc _on_node_publish;
};

#endif // UFRM_LB_CLIENT_H
