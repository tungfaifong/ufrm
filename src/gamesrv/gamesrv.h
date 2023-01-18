// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GAMESRV_H
#define UFRM_GAMESRV_H

#include <map>
#include <vector>

#include "toml++/toml.h"
#include "usrv/units/server_unit.h"
#include "usrv/unit.h"

#include "lb_client/lb_client.h"
#include "protocol/sslcls.pb.h"
#include "common.h"

using namespace usrv;

class GameSrv : public Unit, public std::enable_shared_from_this<GameSrv>
{
public:
	GameSrv(NODEID id, toml::table & config);
	virtual ~GameSrv() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);

	void _OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);
	void _OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body);

private:
	NODEID _id;
	toml::table & _config;

	std::shared_ptr<ServerUnit> _server;

	LBClient _lb_client;
};

#endif // UFRM_GAMESRV_H
