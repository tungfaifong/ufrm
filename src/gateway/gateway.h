// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GATEWAY_H
#define UFRM_GATEWAY_H

#include <map>
#include <vector>

#include "toml++/toml.h"
#include "usrv/unit.h"
#include "usrv/units/server_unit.h"

#include "common.h"
#include "protocol/ssgwgm.pb.h"
#include "protocol/ssgwgs.pb.h"

using namespace usrv;

class Gateway : public Unit, public std::enable_shared_from_this<Gateway>
{
public:
	Gateway(PROCID id, toml::table & config);
	virtual ~Gateway() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	bool SendToGatewayMgr(SSGWGMID id, std::unique_ptr<SSGWGMPkgBody> && body);
	bool SendToGamesrv(PROCID proc_id, SSGWGSID id, std::unique_ptr<SSGWGSPkgBody> && body);

private:
	bool _ConnectToGatewayMgr();
	bool _ConnectToGamesrvs();

	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);
	void _OnIServerConn(NETID net_id, IP ip, PORT port);
	void _OnIServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnIServerDisc(NETID net_id);

	void _OnGatewayMgrRecv(SSGWGMID id, const SSGWGMPkgBody & body);
	void _OnGamesrvRecv(PROCID proc_id, SSGWGSID id, const SSGWGSPkgBody & body);

private:
	void _HeartBeat();
	void _OnGMHeartBeatRsp();
	void _OnGSHeartBeatRsp();

private:
	PROCID _id;
	toml::table & _config;

	NETID _gateway_mgr_net_id;
	std::map<PROCID, NETID> _gamesrv_net_ids;

	std::shared_ptr<ServerUnit> _server;
	std::shared_ptr<ServerUnit> _iserver;
};

#endif // UFRM_GATEWAY_H
