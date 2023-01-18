// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_TIMER_INTERFACE_H
#define USRV_TIMER_INTERFACE_H

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "interfaces/logger_interface.h"
#include "units/lua_unit.h"
#include "units/timer_unit.h"
#include "util/common.h"
#include "unit_manager.h"

NAMESPACE_OPEN

namespace timer
{

inline TIMERID CreateTimer(intvl_t time, std::function<void()> && callback)
{
	return std::dynamic_pointer_cast<TimerUnit>(UnitManager::Instance()->Get("TIMER"))->CreateTimer(time, std::move(callback));
}

inline bool CallTimer(TIMERID id)
{
	return std::dynamic_pointer_cast<TimerUnit>(UnitManager::Instance()->Get("TIMER"))->CallTimer(id);
}

inline bool RemoveTimer(TIMERID id)
{
	return std::dynamic_pointer_cast<TimerUnit>(UnitManager::Instance()->Get("TIMER"))->RemoveTimer(id);
}

inline TIMERID LuaCreateTimer(intvl_t time, luabridge::LuaRef callback)
{
	auto cb = [callback](){
		try
		{
			callback(); 
		}
		catch(const luabridge::LuaException & e)
		{
			LuaUnit::OnException(e);
		}
	};
	return CreateTimer(time, cb);
}

}

NAMESPACE_CLOSE

#endif // USRV_TIMER_INTERFACE_H
