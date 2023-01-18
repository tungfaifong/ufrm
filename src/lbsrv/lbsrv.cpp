// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "lbsrv.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "protocol/ss.pb.h"
#include "define.h"

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

	server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });

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
	Unit::Release();
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
	LOGGER_TRACE("LBSrv::_OnServerRecv node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()), ENUM_NAME(SSLCLSID(head.id())), head.rpc_id());
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
		LOGGER_WARN("LBSrv::_OnServerRecv WARN: invalid node_type:{} node_id:{} msg_type:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()));
		break;
	}
}

void LBSrv::_OnServerDisc(NETID net_id)
{
	_Unsubscribe(net_id);
	auto [node_type, node_id] = _nid2node[net_id];
	_UnregisterNode(node_type, node_id);
	LOGGER_INFO("LBSrv::_OnServerDisc success net_id:{} node_type:{} node_id:{}", net_id, ENUM_NAME(node_type), node_id);
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
		LOGGER_ERROR("LBSrv::_SendToLBClient ERROR: pkg size too long, id:{} size:{}", ENUM_NAME(id), size);
		return false;
	}
	server::Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("LBSrv::_SendToLBClient node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(node_type), node_id, ENUM_NAME(msg_type), ENUM_NAME(id), rpc_id);
	return true;
}

bool LBSrv::_SendToLBClients(std::vector<NETID> net_ids, SSLCLSID id, SSLCLSPkgBody * body, MSGTYPE msg_type /* = MSGT_NORMAL */, size_t rpc_id /* = -1 */)
{
	SSPkg pkg;
	auto head = pkg.mutable_head();
	head->set_from_node_type(LBSRV);
	head->set_from_node_id(_id);
	head->set_id(id);
	head->set_msg_type(msg_type);
	head->set_rpc_id(rpc_id);
	pkg.mutable_body()->set_allocated_lcls_body(body);
	for(auto & net_id : net_ids)
	{
		auto [node_type, node_id] = _nid2node[net_id];
		head->set_to_node_type(node_type);
		head->set_to_node_id(node_id);
		auto size = pkg.ByteSizeLong();
		if(size > UINT16_MAX)
		{
			LOGGER_ERROR("LBSrv::_SendToLBClient ERROR: pkg size too long, id:{} size:{}", ENUM_NAME(id), size);
			continue;
		}
		server::Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
		LOGGER_TRACE("LBSrv::_SendToLBClient node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(node_type), node_id, ENUM_NAME(msg_type), ENUM_NAME(id), rpc_id);
	}
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
	case SSID_LC_LS_NODE_UNREGISTER:
		{
			_OnNodeUnregister(net_id, head, body.node_unregister());
		}
		break;
	case SSID_LC_LS_SUBSCRIBE:
		{
			_OnSubscribe(net_id, head, body.subscribe());
		}
		break;
	default:
		LOGGER_WARN("LBSrv::_OnServerHandeNormal WARN: invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), head.id());
		break;
	}
}

void LBSrv::_OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSLCLSPkgBody & body)
{
	PKG_CREATE(rsp_body, SSLCLSPkgBody);
	SSLCLSID id;
	switch(head.id())
	{
	case SSID_LC_LS_HEART_BEAT_REQ:
		{
			_OnHeartBeatReq(net_id, body.heart_beat_req(), id, rsp_body);
		}
		break;
	case SSID_LC_LS_GET_ALL_NODES_REQ:
		{
			_OnGetAllNodesReq(net_id, body.get_all_nodes_req(), id, rsp_body);
		}
		break;
	case SSID_LC_LS_GET_LEAST_LOAD_NODE_REQ:
		{
			_OnGetLeastLoadNodeReq(net_id, body.get_least_load_node_req(), id, rsp_body);
		}
		break;
	default:
		LOGGER_WARN("LBSrv::_OnServerHanleRpcReq WARN: invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), head.id());
		break;
	}
	_SendToLBClient(net_id, id, rsp_body, MSGT_RPCRSP, head.rpc_id());
}

void LBSrv::_OnNodeRegister(NETID net_id, const SSPkgHead & head, const SSLCLSNodeRegister & body)
{
	_nid2node[net_id] = std::pair<NODETYPE, NODEID>(body.node().node_type(), body.node().node_id());
	_nodes[body.node().node_type()][body.node().node_id()] = Node {net_id, body.node().ip(), (PORT)body.node().port(), 0};
	LOGGER_INFO("LBSrv::_OnNodeRegister success net_id:{} node_type:{} node_id:{} ip:{} port:{}", net_id, ENUM_NAME(body.node().node_type()), body.node().node_id(), body.node().ip(), body.node().port());
	_Publish(SSLSLCPublish::REGISTER, body.node().node_type(), body.node().node_id(), body.node().ip(), body.node().port());
}

void LBSrv::_OnNodeUnregister(NETID net_id, const SSPkgHead & head, const SSLCLSNodeUnregister & body)
{
	_UnregisterNode(body.node_type(), body.node_id());
}

void LBSrv::_OnHeartBeatReq(NETID net_id, const SSLCLSHeartBeatReq & body, SSLCLSID & id, SSLCLSPkgBody * rsp_body)
{
	auto [node_type, node_id] = _nid2node[net_id];
	_nodes[node_type][node_id].load = body.load();
	id = SSID_LS_LC_HEART_BEAT_RSP;
}

void LBSrv::_OnSubscribe(NETID net_id, const SSPkgHead & head, const SSLCLSSubscribe & body)
{
	auto & vec = _subscriber[body.node_type()];
	if(std::find(vec.begin(), vec.end(), net_id) != vec.end())
	{
		return;
	}
	vec.push_back(net_id);
}

void LBSrv::_OnGetAllNodesReq(NETID net_id, const SSLCLSGetAllNodesReq & body, SSLCLSID & id, SSLCLSPkgBody * rsp_body)
{
	auto node_type = body.node_type();
	id = SSID_LS_LC_GET_ALL_NODES_RSP;
	auto rsp = rsp_body->mutable_get_all_nodes_rsp();
	for(auto & [node_id, node] :_nodes[node_type])
	{
		auto rsp_node = rsp->add_nodes();
		rsp_node->set_node_type(node_type);
		rsp_node->set_node_id(node_id);
		rsp_node->set_ip(node.ip);
		rsp_node->set_port(node.port);
	}
}

void LBSrv::_OnGetLeastLoadNodeReq(NETID net_id, const SSLCLSGetLeastLoadNodeReq & body, SSLCLSID & id, SSLCLSPkgBody * rsp_body)
{
	auto node_type = body.node_type();
	NODEID min_node_id = INVALID_NODE_ID;
	uint32_t min_load = 0;
	for(auto & [node_id, node] : _nodes[node_type])
	{
		if(node.load > min_load)
		{
			min_node_id = node_id;
		}
	}
	IP ip = DEFAULT_IP;
	PORT port = DEFAULT_PORT;
	if(min_node_id != INVALID_NODE_ID)
	{
		auto & min_node = _nodes[node_type][min_node_id];
		ip = min_node.ip;
		port = min_node.port;
	}
	id = SSID_LS_LC_GET_LEAST_LOAD_NODE_RSP;
	auto rsp = rsp_body->mutable_get_least_load_node_rsp();
	rsp->mutable_node()->set_node_type(node_type);
	rsp->mutable_node()->set_node_id(min_node_id);
	rsp->mutable_node()->set_ip(ip);
	rsp->mutable_node()->set_port(port);
}

void LBSrv::_UnregisterNode(NODETYPE node_type, NODEID node_id)
{
	if(node_type < 0 || node_type >= NODETYPE_ARRAYSIZE)
	{
		return;
	}
	if(_nodes[node_type].find(node_id) == _nodes[node_type].end())
	{
		return;
	}
	auto net_id = _nodes[node_type][node_id].net_id;
	auto ip = _nodes[node_type][node_id].ip;
	auto port = _nodes[node_type][node_id].port;
	_nodes[node_type].erase(node_id);
	_nid2node.erase(net_id);
	LOGGER_INFO("LBSrv::_UnregisterNode success net_id:{} node_type:{} node_id:{}", net_id, ENUM_NAME(node_type), node_id);
	_Publish(SSLSLCPublish::UNREGISTER, node_type, node_id, ip, port);
}

void LBSrv::_Unsubscribe(NETID net_id)
{
	for(auto i = 0; i < NODETYPE_ARRAYSIZE; ++i)
	{
		auto & vec = _subscriber[i];
		auto iter = std::find(vec.begin(), vec.end(), net_id);
		if(iter != vec.end())
		{
			vec.erase(iter);
			LOGGER_INFO("LBSrv::_Unsubscribe success net_id:{} node_type:{}", net_id, ENUM_NAME(NODETYPE(i)));
		}
	}
}

void LBSrv::_Publish(SSLSLCPublish::CHANGETYPE change_type, NODETYPE node_type, NODEID node_id, IP ip, PORT port)
{
	PKG_CREATE(body, SSLCLSPkgBody);
	auto publish = body->mutable_publish();
	publish->set_change_type(change_type);
	publish->mutable_node()->set_node_type(node_type);
	publish->mutable_node()->set_node_id(node_id);
	publish->mutable_node()->set_ip(ip);
	publish->mutable_node()->set_port(port);
	_SendToLBClients(_subscriber[node_type], SSID_LS_LC_PUBLISH, body);
}
