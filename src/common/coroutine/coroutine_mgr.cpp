// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "coroutine_mgr.h"

#include "usrv/interfaces/timer_interface.h"

bool CoroutineMgr::Start()
{
	_CheckTimeout();
	return true;
}

void CoroutineMgr::Stop()
{
	timer::RemoveTimer(_timer_handler);
	_timer_handler = INVALID_TIMER_ID;
}

void CoroutineMgr::Spawn(std::function<coroutine()> func)
{
	auto id = _coroutines.Insert(std::move(std::make_shared<coroutine>(func())));
	auto coro = _coroutines[id];
	coro->timeout = StdNow() + ms_t(TIMEOUT);
	coro->promise().coro = coro;
	coro->resume();
}

void CoroutineMgr::Resume(COROID coro_id, CORORESULT result, std::string && data)
{
	auto coro = _coroutines[coro_id];
	if(!coro)
	{
		return;
	}
	coro->result = result;
	coro->data = std::move(data);
	coro->resume();
	if(coro->done())
	{
		_coroutines.Erase(coro_id);
	}
}

void CoroutineMgr::_CheckTimeout()
{
	auto now = StdNow();
	for(auto iter = _coroutines.begin(); iter != _coroutines.end();)
	{
		auto coro = (*iter).second;
		if(now >= coro->timeout)
		{
			coro->result = CORORESULT::CR_TIMEOUT;
			coro->resume();
			iter = _coroutines.Erase(iter);
		}
		else
		{
			++iter;
		}
	}
	_timer_handler = timer::CreateTimer(TIMEOUT, std::bind(&CoroutineMgr::_CheckTimeout, shared_from_this()));
}
