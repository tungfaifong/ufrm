// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gateway.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "protocol/ss.pb.h"

Gateway::Gateway(PROCID id, toml::table & config) : _id(id), _config(config)
{

}

bool Gateway::Init()
{
	if(_id == INVALID_PROC_ID)
	{
		LOGGER_ERROR("Gateway::Init ERROR: gateway id is INVALID");
		return false;
	}

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));
	_iserver = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("ISERVER"));

	_server->OnConn(std::bind(&Gateway::_OnServerConn, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_server->OnRecv(std::bind(&Gateway::_OnServerRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_server->OnDisc(std::bind(&Gateway::_OnServerDisc, shared_from_this(), std::placeholders::_1));
	_iserver->OnConn(std::bind(&Gateway::_OnServerConn, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_iserver->OnRecv(std::bind(&Gateway::_OnIServerRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	_iserver->OnDisc(std::bind(&Gateway::_OnServerDisc, shared_from_this(), std::placeholders::_1));

	return true;
}

bool Gateway::Start()
{
	server::Listen(_config["Gateway"]["port"].value_or(DEFAULT_PORT));
	if(!_ConnectToGatewayMgr())
	{
		return false;
	}
	if(!_ConnectToGamesrvs())
	{
		return false;
	}
	_HeartBeat();
	return true;
}

bool Gateway::Update(intvl_t interval)
{
	return false;
}

void Gateway::Stop()
{
}

void Gateway::Release()
{
	_server = nullptr;
	_iserver = nullptr;
}

bool Gateway::SendToGatewayMgr(SSGWGMID id, SSGWGMPkgBody * body)
{
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_type(SS_GWGM);
	head->set_id(id);
	head->set_proc_type(GATEWAY);
	head->set_proc_id(_id);
	pkg.mutable_body()->set_allocated_gwgm_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("Gateway::SendToGatewayMgr ERROR: pkg size too long, id:{} size:{}", id, size);
		return false;
	}
	iserver::Send(_gateway_mgr_net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	return true;
}

bool Gateway::SendToGamesrv(PROCID proc_id, SSGWGSID id, SSGWGSPkgBody * body)
{
	auto game_net_id = _gamesrv_net_ids.find(proc_id);
	if(game_net_id == _gamesrv_net_ids.end())
	{
		LOGGER_ERROR("Gateway::SendToGamesrv ERROR: proc_id invalid, proc_id:{} id:{}", proc_id, id);
		return false;
	}
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_type(SS_GWGS);
	head->set_id(id);
	head->set_proc_type(GATEWAY);
	head->set_proc_id(proc_id);
	pkg.mutable_body()->set_allocated_gwgs_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("Gateway::SendToGamesrv ERROR: pkg size too long, proc_id:{} id:{} size:{}", proc_id, id, size);
		return false;
	}
	iserver::Send(game_net_id->second, pkg.SerializeAsString().c_str(), (uint16_t)size);
	return true;
}

bool Gateway::_ConnectToGatewayMgr()
{
	auto gateway_mgr_ip = _config["GatewayMgr"]["ip"].value_or(DEFAULT_IP);
	auto gateway_mgr_port = _config["GatewayMgr"]["port"].value_or(DEFAULT_PORT);
	_gateway_mgr_net_id = iserver::Connect(gateway_mgr_ip, gateway_mgr_port, _config["GatewayMgr"]["timeout"].value_or(1000));
	if(_gateway_mgr_net_id == INVALID_NET_ID)
	{
		LOGGER_ERROR("Gateway::Start ERROR: gateway mgr connect failed ip:{} port:{}", gateway_mgr_ip, gateway_mgr_port);
		return false;
	}
	SSGWGMPkgBody body;
	SendToGatewayMgr(SSID_GW_GM_INIT, &body);
	LOGGER_INFO("Gateway::_ConnectToGatewayMgr success net_id:{}", _gateway_mgr_net_id);
	return true;
}

bool Gateway::_ConnectToGamesrvs()
{
	for(auto & gamesrv : *_config["Gamesrv"].as_array())
	{
		auto tbl = *gamesrv.as_table();
		auto id = tbl["id"].value_or(0);
		auto ip = tbl["ip"].value_or(DEFAULT_IP);
		auto port = tbl["port"].value_or(DEFAULT_PORT);
		auto net_id = iserver::Connect(ip, port, tbl["timeout"].value_or(1000));
		if(net_id == INVALID_NET_ID)
		{
			LOGGER_ERROR("Gateway::Start ERROR: gamesrv connect failed ip:{} port:{}", ip, port);
			// return false;
		}
		_gamesrv_net_ids[id] = net_id;
		SSGWGSPkgBody body;
		SendToGamesrv(id, SSID_GW_GS_INIT, &body);
		LOGGER_INFO("Gateway::_ConnectToGamesrvs success id:{} net_id:{}", id, net_id);
	}

	return true;
}

void Gateway::_OnServerConn(NETID net_id, IP ip, PORT port)
{

}

void Gateway::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{

}

void Gateway::_OnServerDisc(NETID net_id)
{

}

void Gateway::_OnIServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("Gateway::_OnIServerConn success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void Gateway::_OnIServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	switch(head.type())
	{
		case SS_GWGM:
		{
			_OnGatewayMgrRecv((SSGWGMID)head.id(), body.gwgm_body());
		}
		break;
		case SS_GWGS:
		{
			_OnGamesrvRecv(head.proc_id(), (SSGWGSID)head.id(), body.gwgs_body());
		}
		break;
		default:
		{
			LOGGER_WARN("Gateway::_OnIServerRecv WARN: invalid ss_type:{}", head.type());
		}
		break;
	}
}

void Gateway::_OnIServerDisc(NETID net_id)
{
	LOGGER_INFO("Gateway::_OnIServerDisc success net_id:{}", net_id);
}

void Gateway::_OnGatewayMgrRecv(SSGWGMID id, const SSGWGMPkgBody & body)
{
	switch(id)
	{
		case SSID_GM_GW_HEAT_BEAT_RSP:
		{
			_OnGMHeartBeatRsp();
		}
		break;
		default:
		{
			LOGGER_WARN("Gateway::_OnGatewayMgrRecv WARN: invalid ss_id:{}", id);
		}
		break;
	}
}

void Gateway::_OnGamesrvRecv(PROCID proc_id, SSGWGSID id, const SSGWGSPkgBody & body)
{
	switch(id)
	{
		case SSID_GS_GW_HEAT_BEAT_RSP:
		{
			_OnGSHeartBeatRsp();
		}
		break;
		default:
		{
			LOGGER_WARN("Gateway::_OnGamesrvRecv WARN: invalid ss_id:{}", id);
		}
		break;
	}
}

void Gateway::_HeartBeat()
{
	SSGWGMPkgBody gm_body;
	auto gm_heart_beat = gm_body.mutable_heart_beat_req();
	gm_heart_beat->set_peers_num(_server->PeersNum());
	SendToGatewayMgr(SSID_GW_GM_HEAT_BEAT_REQ, &gm_body);

	SSGWGSPkgBody gs_body;
	for(auto & [proc_id, net_id] : _gamesrv_net_ids)
	{
		SendToGamesrv(net_id, SSID_GW_GS_HEAT_BEAT_REQ, &gs_body);
	}

	timer::CreateTimer(30 * SEC2MILLISEC, std::bind(&Gateway::_HeartBeat, shared_from_this()));
}

void Gateway::_OnGMHeartBeatRsp()
{

}

void Gateway::_OnGSHeartBeatRsp()
{

}
