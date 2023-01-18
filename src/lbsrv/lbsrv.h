// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_LBSRV_H
#define UFRM_LBSRV_H

#include <unordered_map>

#include "toml++/toml.h"
#include "usrv/unit.h"

#include "protocol/ss.pb.h"
#include "common.h"

using namespace usrv;

class LBSrv : public Unit, public std::enable_shared_from_this<LBSrv>
{
public:
	struct Node
	{
		NETID net_id;
		IP ip;
		PORT port;
		uint32_t load;
	};

	LBSrv(NODEID id, toml::table & config);
	virtual ~LBSrv() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	bool _SendToLBClient(NETID net_id, SSID id, SSLCLSPkgBody * body, MSGTYPE msg_type = MSGT_NORMAL, size_t rpc_id = -1);
	bool _SendToLBClients(std::vector<NETID> net_ids, SSID id, SSLCLSPkgBody * body, MSGTYPE msg_type = MSGT_NORMAL, size_t rpc_id = -1);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);

private:
	void _OnNodeRegister(NETID net_id, const SSPkgHead & head, const SSLCLSNodeRegister & body);
	void _OnNodeUnregister(NETID net_id, const SSPkgHead & head, const SSLCLSNodeUnregister & body);
	void _OnHeartBeatReq(NETID net_id, const SSLCLSHeartBeatReq & body, SSID & id, SSLCLSPkgBody * rsp_body);
	void _OnSubscribe(NETID net_id, const SSPkgHead & head, const SSLCLSSubscribe & body);
	void _OnGetAllNodesReq(NETID net_id, const SSLCLSGetAllNodesReq & body, SSID & id, SSLCLSPkgBody * rsp_body);
	void _OnGetLeastLoadNodeReq(NETID net_id, const SSLCLSGetLeastLoadNodeReq & body, SSID & id, SSLCLSPkgBody * rsp_body);

private:
	void _UnregisterNode(NODETYPE node_type, NODEID node_id);
	void _Unsubscribe(NETID net_id);
	void _Publish(SSLSLCPublish::CHANGETYPE change_type, NODETYPE node_type, NODEID node_id, IP ip, PORT port);

private:
	NODEID _id;
	toml::table & _config;

	std::unordered_map<NETID, std::pair<NODETYPE, NODEID>> _nid2node;
	std::unordered_map<NODEID, Node> _nodes[NODETYPE_ARRAYSIZE];

	std::vector<NETID> _subscriber[NODETYPE_ARRAYSIZE];
};

#endif // UFRM_LBSRV_H
