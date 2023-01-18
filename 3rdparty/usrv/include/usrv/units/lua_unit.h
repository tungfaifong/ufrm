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
	using ExposeFunc = std::function<void(luabridge::Namespace)>;

	LuaUnit(const std::string & path, ExposeFunc func = nullptr);
	virtual ~LuaUnit() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	lua_State * GetLuaState();
	luabridge::LuaRef GetGlobal(const char * name);
	luabridge::Namespace GetGlobalNamespace();

	void OnConn(NETID net_id, const IP & ip, PORT port);
	void OnRecv(NETID net_id, const char * data, uint16_t size);
	void OnDisc(NETID net_id);
	static void OnException(const luabridge::LuaException & e);

	bool InitFunc(std::shared_ptr<luabridge::LuaRef> & func, const char * func_name);

private:
	void _Expose();

private:
	std::string _file;
	lua_State * _lua_state;
	std::shared_ptr<luabridge::LuaRef> _start;
	std::shared_ptr<luabridge::LuaRef> _update;
	std::shared_ptr<luabridge::LuaRef> _stop;
	std::shared_ptr<luabridge::LuaRef> _on_conn;
	std::shared_ptr<luabridge::LuaRef> _on_recv;
	std::shared_ptr<luabridge::LuaRef> _on_disc;
	ExposeFunc _expose;
};

NAMESPACE_CLOSE

#endif // USRV_LUA_UNIT_H
