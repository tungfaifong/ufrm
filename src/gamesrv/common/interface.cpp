#include "interface.h"

#include "usrv/unit_manager.h"

#include "define.h"
#include "gamesrv.h"

void SendToClient(ROLEID role_id, uint32_t id, const char * data, uint16_t size)
{
	auto gamesrv = std::dynamic_pointer_cast<GameSrv>(UnitManager::Instance()->Get("GAMESRV"));
	PKG_CREATE(body, CSPkgBody);
	body->ParseFromArray(body, size);
	gamesrv->SendToClient(role_id, (CSID)id, body);
}

void SendToProxy(uint16_t node_type, NODEID node_id, uint32_t id, const char * data, uint16_t size, NODEID proxy_id, uint16_t logic_type, uint16_t msg_type, size_t rpc_id)
{
	auto gamesrv = std::dynamic_pointer_cast<GameSrv>(UnitManager::Instance()->Get("GAMESRV"));
	PKG_CREATE(body, SSPkgBody);
	body->ParseFromArray(body, size);
	gamesrv->SendToProxy((NODETYPE)node_type, node_id, (SSID)id, body, proxy_id, (SSPkgHead::LOGICTYPE)logic_type, (SSPkgHead::MSGTYPE)msg_type, rpc_id);
}

void BroadcastToProxy(uint16_t node_type, uint32_t id, const char * data, uint16_t size, NODEID proxy_id, uint16_t logic_type)
{
	auto gamesrv = std::dynamic_pointer_cast<GameSrv>(UnitManager::Instance()->Get("GAMESRV"));
	PKG_CREATE(body, SSPkgBody);
	body->ParseFromArray(body, size);
	gamesrv->BroadcastToProxy((NODETYPE)node_type, (SSID)id, body, proxy_id, (SSPkgHead::LOGICTYPE)logic_type);
}
