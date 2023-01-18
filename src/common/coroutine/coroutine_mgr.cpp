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
	_coro_objs.Clear();
}

std::shared_ptr<CoroutineMgr::CoroObj> CoroutineMgr::Insert(std::coroutine_handle<> & coro)
{
	auto coro_obj = std::make_shared<CoroObj>();
	coro_obj->coro = coro;
	coro_obj->timeout = StdNow() + ms_t(TIMEOUT);
	auto id = _coro_objs.Insert(std::move(coro_obj));
	_coro_objs[id]->id = id;
	return _coro_objs[id];
}

void CoroutineMgr::Resume(COROID coro_id, CORORESULT result, std::string && data)
{
	auto coro_obj = _coro_objs[coro_id];
	if(!coro_obj)
	{
		return;
	}
	coro_obj->result = result;
	coro_obj->data = std::move(data);
	coro_obj->coro.resume();
	if(coro_obj->coro.done())
	{
		_coro_objs.Erase(coro_id);
	}
}

void CoroutineMgr::_CheckTimeout()
{
	auto now = StdNow();
	for(auto iter = _coro_objs.begin(); iter != _coro_objs.end();)
	{
		auto coro_obj = iter->second;
		if(now >= coro_obj->timeout)
		{
			coro_obj->result = CORORESULT::TIMEOUT;
			coro_obj->coro.resume();
			iter = _coro_objs.Erase(iter);
		}
		else
		{
			++iter;
		}
	}
	_timer_handler = timer::CreateTimer(TIMEOUT, [self = shared_from_this()](){ self->_CheckTimeout(); });
}
