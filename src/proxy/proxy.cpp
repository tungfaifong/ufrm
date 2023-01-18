// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "proxy.h"

#include "magic_enum.hpp"
#include "usrv/interfaces/logger_interface.h"
#include "usrv/interfaces/server_interface.h"
#include "usrv/interfaces/timer_interface.h"

#include "coroutine/coroutine_mgr.h"
#include "protocol/ss.pb.h"
#include "define.h"

Proxy::Proxy(NODEID id, toml::table & config) : _id(id), _config(config), 
	_lb_client(PROXY, _id, _config["Proxy"]["ip"].value_or(DEFAULT_IP), _config["Proxy"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["id"].value_or(INVALID_NODE_ID), _config["LBSrv"]["ip"].value_or(DEFAULT_IP), _config["LBSrv"]["port"].value_or(DEFAULT_PORT), \
	_config["LBSrv"]["timeout"].value_or(0))
{

}

bool Proxy::Init()
{
	if(_id == INVALID_NODE_ID)
	{
		LOGGER_ERROR("proxy id is INVALID");
		return false;
	}

	_server = std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"));

	_server->OnConn([self = shared_from_this()](NETID net_id, IP ip, PORT port){ self->_OnServerConn(net_id, ip, port); });
	_server->OnRecv([self = shared_from_this()](NETID net_id, char * data, uint16_t size){ self->_OnServerRecv(net_id, data, size); });
	_server->OnDisc([self = shared_from_this()](NETID net_id){ self->_OnServerDisc(net_id); });

	_lb_client.Init(_server, [self = shared_from_this()](){ return 0; });

	return true;
}

bool Proxy::Start()
{
	_server->Listen(_config["Proxy"]["port"].value_or(DEFAULT_PORT));
	if(!CoroutineMgr::Instance()->Start())
	{
		return false;
	}
	if(!_lb_client.Start())
	{
		return false;
	}
	return true;
}

bool Proxy::Update(intvl_t interval)
{
	return false;
}

void Proxy::Stop()
{
	CoroutineMgr::Instance()->Stop();
}

void Proxy::Release()
{
	_lb_client.Release();
	Unit::Release();
}

void Proxy::_OnServerConn(NETID net_id, IP ip, PORT port)
{
	LOGGER_INFO("onconnect success net_id:{} ip:{} port:{}", net_id, ip, port);
}

void Proxy::_OnServerRecv(NETID net_id, char * data, uint16_t size)
{
	SSPkg pkg;
	pkg.ParseFromArray(data, size);
	auto head = pkg.head();
	auto body = pkg.body();
	LOGGER_TRACE("recv msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()), SSID_Name(head.id()), head.rpc_id());
	switch(head.proxy_type())
	{
		case SSPkgHead::END:
			{
				switch(head.msg_type())
				{
				case SSPkgHead::NORMAL:
					{
						_OnServerHandeNormal(net_id, head, body);
					}
					break;
				case SSPkgHead::RPCREQ:
					{
						_OnServerHanleRpcReq(net_id, head, body.pcpx_body());
					}
					break;
				case SSPkgHead::RPCRSP:
					{
						_OnServerHanleRpcRsp(net_id, head, body);
					}
					break;
				default:
					LOGGER_WARN("invalid node_type:{} node_id:{} msg_type:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), ENUM_NAME(head.msg_type()));
					break;
				}
			}
			break;
		case SSPkgHead::FORWARD:
			{
				_SendToNode(head.to_node_type(), head.to_node_id(), pkg);
			}
			break;
		case SSPkgHead::BROADCAST:
			{
				_SendToNodes(head.to_node_type(), pkg);
			}
			break;
		default:
			LOGGER_WARN("invalid proxy_type:{}", ENUM_NAME(head.proxy_type()));
			break;
	}
	
}

void Proxy::_OnServerDisc(NETID net_id)
{
	auto [node_type, node_id] = _nid2node[net_id];
	_UnregisterNode(node_type, node_id);
	LOGGER_INFO("ondisconnect success net_id:{} node_type:{} node_id:{}", net_id, ENUM_NAME(node_type), node_id);
}

void Proxy::_SendToPXClient(NETID net_id, SSID id, SSPCPXPkgBody * body, SSPkgHead::MSGTYPE msg_type /* = SSPkgHead::NORMAL */, size_t rpc_id /* = -1 */)
{
	auto [node_type, node_id] = _nid2node[net_id];
	SEND_SSPKG(_server, net_id, PROXY, _id, node_type, node_id, id, msg_type, rpc_id, SSPkgHead::END, mutable_body()->set_allocated_pcpx_body, body);
}

void Proxy::_SendToNode(NODETYPE node_type, NODEID node_id, SSPkg & pkg)
{
	if(_nodes[node_type].find(node_id) == _nodes[node_type].end())
	{
		LOGGER_ERROR("node:type:{} id:{} is invalid", ENUM_NAME(node_type), node_id);
		return;
	}
	auto net_id = _nodes[node_type][node_id];
	auto head = pkg.mutable_head();
	head->set_proxy_type(SSPkgHead::END);
	auto size = pkg.ByteSizeLong();
	if(size > UINT16_MAX)
	{
		LOGGER_ERROR("pkg size too long, id:{} size:{}", SSID_Name(head->id()), size);
		return;
	}
	_server->Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
	LOGGER_TRACE("send msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(node_type), node_id, ENUM_NAME(head->msg_type()), SSID_Name(head->id()), head->rpc_id());
}

void Proxy::_SendToNodes(NODETYPE node_type, SSPkg & pkg)
{
	auto head = pkg.mutable_head();
	head->set_proxy_type(SSPkgHead::END);
	for(const auto & [node_id, net_id] : _nodes[node_type])
	{
		head->set_to_node_id(node_id);
		auto size = pkg.ByteSizeLong();
		if(size > UINT16_MAX)
		{
			LOGGER_ERROR("pkg size too long, id:{} size:{}", SSID_Name(head->id()), size);
			continue;
		}
		_server->Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);
		LOGGER_TRACE("send msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(node_type), node_id, ENUM_NAME(head->msg_type()), SSID_Name(head->id()), head->rpc_id());
	}
}

void Proxy::_OnServerHandeNormal(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	switch (head.from_node_type())
	{
	case LBSRV:
		{
			_lb_client.OnRecv(net_id, head, body.lcls_body());
		}
		break;
	default:
		{
			_OnRecvPXClient(net_id, head, body.pcpx_body());
		}
		break;
	}
}

void Proxy::_OnServerHanleRpcReq(NETID net_id, const SSPkgHead & head, const SSPCPXPkgBody & body)
{
	PKG_CREATE(rsp_body, SSPCPXPkgBody);
	SSID id;
	switch(head.id())
	{
	case SSID_PC_PX_HEART_BEAT_REQ:
		{
			_OnHeartBeatReq(net_id, body.heart_beat_req(), id, rsp_body);
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), head.id());
		break;
	}
	_SendToPXClient(net_id, id, rsp_body, SSPkgHead::RPCRSP, head.rpc_id());
}

void Proxy::_OnServerHanleRpcRsp(NETID net_id, const SSPkgHead & head, const SSPkgBody & body)
{
	CoroutineMgr::Instance()->Resume(head.rpc_id(), CORORESULT::SUCCESS, std::move(body.SerializeAsString()));
}

void Proxy::_OnRecvPXClient(NETID net_id, const SSPkgHead & head, const SSPCPXPkgBody & body)
{
	switch(head.id())
	{
	case SSID_PC_PX_NODE_REGISTER:
		{
			_OnNodeRegister(net_id, head, body.node_register());
		}
		break;
	case SSID_PC_PX_NODE_UNREGISTER:
		{
			_OnNodeUnregister(net_id, head, body.node_unregister());
		}
		break;
	default:
		LOGGER_WARN("invalid node_type:{} node_id:{} id:{}", ENUM_NAME(head.from_node_type()), head.from_node_id(), head.id());
		break;
	}
}

void Proxy::_OnNodeRegister(NETID net_id, const SSPkgHead & head, const SSPCPXNodeRegister & body)
{
	auto node_type = head.from_node_type();
	auto node_id = head.from_node_id();
	_nid2node[net_id] = std::pair<NODETYPE, NODEID>(node_type, node_id);
	_nodes[node_type][node_id] = net_id;
	LOGGER_INFO("success net_id:{} node_type:{} node_id:{}", net_id, ENUM_NAME(node_type), node_id);
}

void Proxy::_OnNodeUnregister(NETID net_id, const SSPkgHead & head, const SSPCPXNodeUnregister & body)
{
	_UnregisterNode(head.from_node_type(), head.from_node_id());
}

void Proxy::_OnHeartBeatReq(NETID net_id, const SSPCPXHeartBeatReq & body, SSID & id, SSPCPXPkgBody * rsp_body)
{
	id = SSID_PX_PC_HEART_BEAT_RSP;
}

void Proxy::_UnregisterNode(NODETYPE node_type, NODEID node_id)
{
	if(node_type < 0 || node_type >= NODETYPE_ARRAYSIZE)
	{
		return;
	}
	if(_nodes[node_type].find(node_id) == _nodes[node_type].end())
	{
		return;
	}
	auto net_id = _nodes[node_type][node_id];
	_nodes[node_type].erase(node_id);
	_nid2node.erase(net_id);
	LOGGER_INFO("net_id:{} node_type:{} node_id:{}", net_id, ENUM_NAME(node_type), node_id);
}
