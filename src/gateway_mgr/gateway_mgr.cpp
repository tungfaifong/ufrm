// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "gateway_mgr.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "protocol/ss.pb.h"

GatewayMgr::GatewayMgr(PROCID id, toml::table & config) : _id(id), _config(config)
{

}

bool GatewayMgr::Init()
{
	if(_id == INVALID_PROC_ID)
	{
		LOGGER_ERROR("GatewayMgr::Init ERROR: gateway_mgr id is INVALID");
		return false;
	}

	auto server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	server->OnConn(std::bind(&GatewayMgr::_OnServerConn, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server->OnRecv(std::bind(&GatewayMgr::_OnServerRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server->OnDisc(std::bind(&GatewayMgr::_OnServerDisc, shared_from_this(), std::placeholders::_1));

	return true;
}

bool GatewayMgr::Start()
{
	server::Listen(_config["GatewayMgr"]["port"].value_or(DEFAULT_PORT));
	return true;
}

bool GatewayMgr::Update(intvl_t interval)
{
	return false;
}

void GatewayMgr::Stop()
{
}

void GatewayMgr::Release()
{
}

bool GatewayMgr::SendToGateway(PROCID proc_id, SSGWGMID id, std::unique_ptr<SSGWGMPkgBody> && body)
{
	auto gateway = _gateways.find(proc_id);
	if(gateway == _gateways.end())
	{
		LOGGER_ERROR("Gateway::SendToGateway ERROR: proc_id invalid, proc_id:{} id:{}", proc_id, id);
		return false;
	}
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_type(SS_GWGM);
	head->set_id(id);
	head->set_proc_type(GATEWAY_MGR);
	head->set_proc_id(_id);
	pkg.mutable_body()->set_allocated_gwgm_body(body.release());
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("Gateway::SendToGateway ERROR: pkg size too long, id:{} size:{}", id, size);
		return false;
	}
	server::Send(gateway->second.net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	return true;
}

void GatewayMgr::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("GatewayMgr::_OnServerConn success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void GatewayMgr::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	switch(head.type())
	{
		case SS_GWGM:
		{
			_OnGatewayRecv(net_id, head.proc_id(), (SSGWGMID)head.id(), body.gwgm_body());
		}
		break;
		default:
		{
			LOGGER_WARN("GatewayMgr::_OnServerRecv WARN: invalid ss_type:{}", head.type());
		}
		break;
	}
}

void GatewayMgr::_OnServerDisc(NETID net_id)
{
	LOGGER_INFO("GatewayMgr::_OnServerDisc success net_id:{}", net_id);

	auto gateway = _gateway_nid2pid.find(net_id);
	if(gateway == _gateway_nid2pid.end())
	{
		LOGGER_WARN("GatewayMgr::_OnServerDisc Find gateway failed net_id:{}", net_id);
		return;
	}
	auto proc_id = gateway->second;
	_gateway_nid2pid.erase(net_id);
	_gateways.erase(proc_id);
}

void GatewayMgr::_OnGatewayRecv(NETID net_id, PROCID proc_id, SSGWGMID id, const SSGWGMPkgBody & body)
{
	switch(id)
	{
		case SSID_GW_GM_INIT:
		{
			_OnGWInit(net_id, proc_id, body.init());
		}
		break;
		case SSID_GW_GM_HEAT_BEAT_REQ:
		{
			_OnGWHeartBeatReq(proc_id, body.heart_beat_req());
		}
		break;
		default:
		{
			LOGGER_WARN("Gateway::_OnGatewayRecv WARN: invalid ss_id:{}", id);
		}
		break;
	}
}

void GatewayMgr::_OnGWInit(NETID net_id, PROCID proc_id, const SSGWGMInit & req)
{
	_gateway_nid2pid[net_id] = proc_id;
	_gateways[proc_id] = Gateway{net_id, 0};
	LOGGER_INFO("GatewayMgr::_OnGWInit net_id:{} proc_id:{}", net_id, proc_id);
}

void GatewayMgr::_OnGWHeartBeatReq(PROCID proc_id, const SSGWGMHeartBeatReq & req)
{
	auto gateway = _gateways.find(proc_id);
	if(gateway == _gateways.end())
	{
		return;
	}
	gateway->second.peers_num = req.peers_num();

	auto body = std::make_unique<SSGWGMPkgBody>();
	SendToGateway(proc_id, SSID_GM_GW_HEAT_BEAT_RSP, std::move(body));
}
