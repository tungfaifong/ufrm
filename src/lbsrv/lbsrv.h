// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_LBSRV_H
#define UFRM_LBSRV_H

#include <unordered_map>

#include "toml++/toml.h"
#include "usrv/unit.h"

#include "common.h"
#include "protocol/ss.pb.h"

using namespace usrv;

struct Node
{
	NETID net_id;
	IP ip;
	PORT port;
	uint32_t load;
};

class LBSrv : public Unit, public std::enable_shared_from_this<LBSrv>
{
public:
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

	bool _SendToLBClient(NETID net_id, SSLCLSID id, SSLCLSPkgBody * body, MSGTYPE msg_type = MSGT_NORMAL, size_t rpc_id = -1);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);
	void _OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);

private:
	void _OnNodeRegister(NETID net_id, const SSPkgHead & head, const SSLCLSNodeRegister & body);
	void _OnHeartBeatReq(NETID net_id, const SSLCLSHeartBeatReq & body, SSLCLSID & id, SSLCLSPkgBody * rsp_body);

private:
	NODEID _id;
	toml::table & _config;

	std::unordered_map<NETID, std::pair<NODETYPE, NODEID>> _nid2node;
	std::unordered_map<NODEID, Node> _nodes[NODETYPE_ARRAYSIZE];
};

#endif // UFRM_LBSRV_H
