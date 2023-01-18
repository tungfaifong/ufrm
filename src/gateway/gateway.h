// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GATEWAY_H
#define UFRM_GATEWAY_H

#include <map>
#include <vector>

#include "toml++/toml.h"
#include "usrv/units/server_unit.h"
#include "usrv/unit.h"

#include "lb_client/lb_client.h"
#include "protocol/sslcls.pb.h"
#include "common.h"

using namespace usrv;

class Gateway : public Unit, public std::enable_shared_from_this<Gateway>
{
public:
	Gateway(NODEID id, toml::table & config);
	virtual ~Gateway() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

private:
	void _OnServerConn(NETID net_id, IP ip, PORT port);
	void _OnServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnServerDisc(NETID net_id);
	void _OnIServerConn(NETID net_id, IP ip, PORT port);
	void _OnIServerRecv(NETID net_id, char * data, uint16_t size);
	void _OnIServerDisc(NETID net_id);

	void _OnIServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);
	void _OnIServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body);

private:
	NODEID _id;
	toml::table & _config;

	std::shared_ptr<ServerUnit> _server;
	std::shared_ptr<ServerUnit> _iserver;

	LBClient _lb_client;
};

#endif // UFRM_GATEWAY_H
