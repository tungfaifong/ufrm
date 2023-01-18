// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GATEWAY_MGR_H
#define UFRM_GATEWAY_MGR_H

#include "toml++/toml.h"
#include "usrv/unit.h"

#include "common.h"
#include "protocol/ssgwgm.pb.h"

using namespace usrv;

struct Gateway
{
	NETID net_id;
	uint32_t peers_num;
};

class GatewayMgr : public Unit, public std::enable_shared_from_this<GatewayMgr>
{
public:
	GatewayMgr(PROCID id, toml::table & config);
	virtual ~GatewayMgr() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	bool SendToGateway(PROCID proc_id, SSGWGMID id, SSGWGMPkgBody * body);

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	void _OnGatewayRecv(NETID net_id, PROCID proc_id, SSGWGMID id, const SSGWGMPkgBody & body);

private:
	void _OnGWInit(NETID net_id, PROCID proc_id, const SSGWGMInit & req);
	void _OnGWHeartBeatReq(PROCID proc_id, const SSGWGMHeartBeatReq & req);

private:
	PROCID _id;
	toml::table & _config;

	std::map<NETID, PROCID> _gateway_nid2pid;
	std::map<PROCID, Gateway> _gateways;

};

#endif // UFRM_GATEWAY_MGR_H
