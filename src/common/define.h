// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_DEFINE_H
#define UFRM_DEFINE_H

#define PKG_CREATE(pkg, PKG_TYPE) PKG_TYPE * pkg = new PKG_TYPE()

#define SEND_SSPKG(server, net_id, node_type, node_id, to_node_type, to_node_id, id, msg_type, rpc_id, proxy_type, logic_type, set_body, body) \
	SSPkg pkg;\
	auto head = pkg.mutable_head();\
	head->set_from_node_type(node_type);\
	head->set_from_node_id(node_id);\
	head->set_to_node_type(to_node_type);\
	head->set_to_node_id(to_node_id);\
	head->set_id(id);\
	head->set_msg_type(msg_type);\
	head->set_rpc_id(rpc_id);\
	head->set_proxy_type(proxy_type);\
	head->set_logic_type(logic_type);\
	pkg.set_body(body);\
	auto size = pkg.ByteSizeLong();\
	if(size > UINT16_MAX)\
	{\
		LOGGER_ERROR("pkg size too long, id:{} size:{}", SSID_Name(id), size);\
		return;\
	}\
	server->Send(net_id, pkg.SerializeAsString().c_str(), (uint16_t)size);\
	LOGGER_TRACE("send msg node_type:{} node_id:{} msg_type:{} id:{} rpc_id:{}", ENUM_NAME(node_type), node_id, ENUM_NAME(msg_type), SSID_Name(id), rpc_id)

#define CO_SPAWN(future) future.coro.resume()

#define ENUM_NAME(enum) magic_enum::enum_name(enum)

#endif // UFRM_DEFINE_H
