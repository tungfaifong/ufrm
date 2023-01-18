// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lbsrv.h"

#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "protocol/ss.pb.h"

LBSrv::LBSrv(NODEID id, toml::table & config) : _id(id), _config(config)
{

}

bool LBSrv::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("LBSrv::Init ERROR: gateway_mgr id is INVALID");
		return false;
	}

	auto server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	server->OnConn(std::bind(&LBSrv::_OnServerConn, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server->OnRecv(std::bind(&LBSrv::_OnServerRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	server->OnDisc(std::bind(&LBSrv::_OnServerDisc, shared_from_this(), std::placeholders::_1));

	return true;
}

bool LBSrv::Start()
{
	server::Listen(_config["LBSrv"]["port"].value_or(DEFAULT_PORT));
	return true;
}

bool LBSrv::Update(intvl_t interval)
{
	return false;
}

void LBSrv::Stop()
{
}

void LBSrv::Release()
{
}

void LBSrv::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("LBSrv::_OnServerConn success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void LBSrv::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	switch(head.msg_type())
	{
	case MSGT_NORMAL:
		{
			_OnServerHandeNormal(net_id, head, body.lcls_body());
		}
		break;
	case MSGT_RPCREQ:
		{
			_OnServerHanleRpcReq(net_id, head, body.lcls_body());
		}
		break;
	default:
		break;
	}
	
}

void LBSrv::_OnServerDisc(NETID net_id)
{
	auto [node_type, node_id] = _nid2node[net_id];
	_nid2node.erase(net_id);
	_nodes[node_type].erase(node_id);
	LOGGER_INFO("LBSrv::_OnServerDisc success net_id:{} node_type:{} node_id:{}", net_id, node_type, node_id);
}

bool LBSrv::_SendToLBClient(NETID net_id, SSLCLSID id, SSLCLSPkgBody * body, MSGTYPE msg_type /* = MSGT_NORMAL */, size_t rpc_id /* = -1 */)
{
	SSPkg pkg;
	auto [node_type, node_id] = _nid2node[net_id];
	auto head = pkg.mutable_head();
	head->set_from_node_type(LBSRV);
	head->set_from_node_id(_id);
	head->set_to_node_type(node_type);
	head->set_to_node_id(node_id);
	head->set_id(id);
	head->set_msg_type(msg_type);
	head->set_rpc_id(rpc_id);
	pkg.mutable_body()->set_allocated_lcls_body(body);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("LBClient::_SendToLBClient ERROR: pkg size too long, id:{} size:{}", id, size);
		return false;
	}
	server::Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	return true;
}

void LBSrv::_OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body)
{
	switch(head.id())
	{
	case SSID_LC_LS_NODE_REGISTER:
		{
			_OnNodeRegister(net_id, head, body.node_register());
		}
		break;
	default:
		break;
	}
}

void LBSrv::_OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body)
{
	CREATE_PKG(rsp_body, SSLCLSPkgBody);
	SSLCLSID id;
	switch(head.id())
	{
	case SSID_LC_LS_NODE_REGISTER:
		{
			_OnHeartBeatReq(net_id, body.heart_beat_req(), id, rsp_body);
		}
		break;
	default:
		break;
	}
	_SendToLBClient(net_id, id, rsp_body, MSGT_RPCRSP, head.rpc_id());
}

void LBSrv::_OnNodeRegister(NETID net_id, const SSPkgHead & head, const SSLCLSNodeRegister & body)
{
	_nid2node[net_id] = std::pair<NODETYPE, NODEID>(head.from_node_type(), head.from_node_id());
	_nodes[body.node_type()][head.from_node_id()] = Node {net_id, body.ip(), (PORT)body.port(), 0};
	LOGGER_INFO("LBSrv::_OnNodeRegister success net_id:{} node_type:{} node_id:{} ip:{} port:{}", net_id, head.from_node_type(), head.from_node_id(), body.ip(), body.port());
}

void LBSrv::_OnHeartBeatReq(NETID net_id, const SSLCLSHeartBeatReq & body, SSLCLSID & id, SSLCLSPkgBody * rsp_body)
{
	auto [node_type, node_id] = _nid2node[net_id];
	_nodes[node_type][node_id].load = body.load();
	id = SSID_LS_LC_HEAT_BEAT_RSP;
}
