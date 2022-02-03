// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_LUA_UNIT_H
#define USRV_LUA_UNIT_H

#include <functional>

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"

#include "unit.h"

NAMESPACE_OPEN

class LuaUnit : public Unit
{
public:
	using ExposeFunc = std::function<void(const luabridge::Namespace &)>;

	LuaUnit(const std::string & path, ExposeFunc func = nullptr);
	virtual ~LuaUnit() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	lua_State * GetLuaState();
	inline luabridge::LuaRef GetGlobal(const char * name);
	inline luabridge::Namespace GetGlobalNamespace();

	void OnRecvFunc(NETID net_id, char * data, uint16_t size);
	static void OnException(const luabridge::LuaException & e);

private:
	bool _InitFunc(std::shared_ptr<luabridge::LuaRef> & func, const char * func_name);
	void _Expose();

private:
	std::string _file;
	lua_State * _lua_state;
	std::shared_ptr<luabridge::LuaRef> _start;
	std::shared_ptr<luabridge::LuaRef> _update;
	std::shared_ptr<luabridge::LuaRef> _stop;
	std::shared_ptr<luabridge::LuaRef> _on_recv;
	ExposeFunc _expose;
};
	
NAMESPACE_CLOSE

#endif // USRV_LUA_UNIT_H
