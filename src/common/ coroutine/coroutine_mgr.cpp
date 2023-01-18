// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "coroutine_mgr.h"

void CoroutineMgr::Spawn(std::function<coroutine()> func)
{
	auto id = _coroutines.Insert(std::move(std::make_shared<coroutine>(func())));
	auto coro = _coroutines[id];
	coro->promise().coro = coro;
	coro->resume();
}

void CoroutineMgr::Resume(COROID coro_id, CORORESULT)
{
	auto coro = _coroutines[coro_id];
	if(!coro)
	{
		return;
	}
	_Resume(coro, CORORESULT::CR_SUCCESS);
}

void CoroutineMgr::Update()
{
	
}

void CoroutineMgr::_Resume(std::shared_ptr<coroutine> coro, CORORESULT result)
{
	coro->result = result;
	coro->resume();
	if(coro->done())
	{
		_coroutines.Erase(coro_id);
	}
}
