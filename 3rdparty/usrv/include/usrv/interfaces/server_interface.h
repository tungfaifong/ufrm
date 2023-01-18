// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_SERVER_INTERFACE_H
#define USRV_SERVER_INTERFACE_H

#include "units/server_unit.h"
#include "util/common.h"
#include "unit_manager.h"

NAMESPACE_OPEN

namespace server
{

inline void Listen(PORT port)
{
	std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"))->Listen(port);
}

inline NETID Connect(const IP & ip, PORT port, uint32_t timeout)
{
	return std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"))->Connect(ip, port, timeout);
}

inline void Disconnect(NETID net_id)
{
	std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"))->Disconnect(net_id);
}

inline bool Send(NETID net_id, const char * data, uint16_t size)
{
	return std::dynamic_pointer_cast<ServerUnit>(UnitManager::Instance()->Get("SERVER"))->Send(net_id, data, size);
}

}

NAMESPACE_CLOSE

#endif // USRV_SERVER_INTERFACE_H
