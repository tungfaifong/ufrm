// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_PROXY_H
#define UFRM_PROXY_H

#include <unordered_map>

#include "toml++/toml.h"
#include "usrv/unit.h"

#include "lb_client/lb_client.h"
#include "protocol/ss.pb.h"
#include "common.h"

using namespace usrv;

class Proxy : public Unit, public std::enable_shared_from_this<Proxy>
{
public:
	Proxy(NODEID id, toml::table & config);
	virtual ~Proxy() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	bool _SendToPXClient(NETID net_id, SSID id, SSPCPXPkgBody * body, SSPkgHead::MSGTYPE msg_type = SSPkgHead::NORMAL, size_t rpc_id = -1);
	bool _SendToNode(NODETYPE node_type, NODEID node_id, SSPkg & pkg);
	bool _SendToNodes(NODETYPE node_type, SSPkg & pkg);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPCPXPkgBody & body);
	void _OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);

	void _OnRecvPXClient(NETID net_id, const SSPkgHead & head, const SSPCPXPkgBody & body);

private:
	void _OnNodeRegister(NETID net_id, const SSPkgHead & head, const SSPCPXNodeRegister & body);
	void _OnNodeUnregister(NETID net_id, const SSPkgHead & head, const SSPCPXNodeUnregister & body);
	void _OnHeartBeatReq(NETID net_id, const SSPCPXHeartBeatReq & body, SSID & id, SSPCPXPkgBody * rsp_body);

private:
	void _UnregisterNode(NODETYPE node_type, NODEID node_id);

private:
	NODEID _id;
	toml::table & _config;

	LBClient _lb_client;

	std::unordered_map<NETID, std::pair<NODETYPE, NODEID>> _nid2node;
	std::unordered_map<NODEID, NETID> _nodes[NODETYPE_ARRAYSIZE];
};

#endif // UFRM_PROXY_H
