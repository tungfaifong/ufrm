// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GATEWAY_H
#define UFRM_GATEWAY_H

#include <vector>

#include "toml++/toml.h"

#include "usrv/unit.h"

using namespace usrv;

class Gateway : public Unit, public std::enable_shared_from_this<Gateway>
{
public:
	Gateway(toml::table & config);
	virtual ~Gateway() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	void OnServerRecv(NETID net_id, char * data, uint16_t size);
	void OnIServerRecv(NETID net_id, char * data, uint16_t size);

private:
	toml::table & _config;
	NETID _gateway_mgr_net_id;
	std::vector<NETID> _gamesrv_net_ids;
};

#endif // UFRM_GATEWAY_H
