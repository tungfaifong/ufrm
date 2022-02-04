// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gateway.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"

#include "common.h"

Gateway::Gateway(toml::table & config) : _config(config)
{

}

bool Gateway::Init()
{
	std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"))->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){
		self->OnServerRecv(net_id, data, size);
	});

	std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("ISERVER"))->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){
		self->OnIServerRecv(net_id, data, size);
	});

	return true;
}

bool Gateway::Start()
{
	server::Listen(_config["Gateway"]["port"].value_or(DEFAULT_PORT));
	auto gateway_mgr_ip = _config["GatewayMgr"]["ip"].value_or(DEFAULT_IP);
	auto gateway_mgr_port = _config["GatewayMgr"]["port"].value_or(DEFAULT_PORT);
	_gateway_mgr_net_id = iserver::Connect(gateway_mgr_ip, gateway_mgr_port, _config["GatewayMgr"]["timeout"].value_or(1000));
	if(_gateway_mgr_net_id == INVALID_NET_ID)
	{
		LOGGER_ERROR("Gateway::Start failed: gateway mgr connect failed ip:{} port:{}", gateway_mgr_ip, gateway_mgr_port);
		return false;
	}
	for(auto & gamesrv : *_config["Gamesrv"].as_array())
	{
		auto tbl = *gamesrv.as_table();
		auto ip = tbl["ip"].value_or(DEFAULT_IP);
		auto port = tbl["port"].value_or(DEFAULT_PORT);
		auto net_id = iserver::Connect(ip, port, tbl["timeout"].value_or(1000));
		if(net_id == INVALID_NET_ID)
		{
			LOGGER_ERROR("Gateway::Start failed: gamesrv connect failed ip:{} port:{}", ip, port);
			return false;
		}
		_gamesrv_net_ids.emplace_back(net_id);
	}

	return true;
}

bool Gateway::Update(intvl_t interval)
{
	return true;
}

void Gateway::Stop()
{
}

void Gateway::Release()
{
}

void Gateway::OnServerRecv(NETID net_id, char * data, uint16_t size)
{

}

void Gateway::OnIServerRecv(NETID net_id, char * data, uint16_t size)
{

}
