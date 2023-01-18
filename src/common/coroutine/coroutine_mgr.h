// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_MGR_H
#define UFRM_COROUTINE_MGR_H

#include "usrv/util/time.h"
#include "usrv/util/object_map.hpp"
#include "usrv/util/singleton.hpp"

#include "coroutine.h"

using namespace usrv;

using COROID = size_t;

enum class CORORESULT
{
	SUCCESS = 0,
	TIMEOUT,
};

class CoroutineMgr : public Singleton<CoroutineMgr>, public std::enable_shared_from_this<CoroutineMgr>
{
public:
	static constexpr size_t ALLOC_NUM = 1024;
	static constexpr intvl_t TIMEOUT = 5000;

	struct CoroObj
	{
		std::coroutine_handle<> coro;
		COROID id;
		std_clock_t timeout;
		CORORESULT result;
		std::string data;
	};

	CoroutineMgr() = default;
	~CoroutineMgr() = default;

	bool Start();
	void Stop();

public:
	std::shared_ptr<CoroutineMgr::CoroObj> Insert(std::coroutine_handle<> & coro);
	void Resume(COROID coro_id, CORORESULT result, std::string && data);

private:
	void _CheckTimeout();

private:
	ObjectMap<CoroObj> _coro_objs {ALLOC_NUM};
	TIMERID _timer_handler {INVALID_TIMER_ID};
};

struct awaitable_func
{
	bool await_ready() { return false; }
	auto await_suspend(std::coroutine_handle<> caller)
	{
		coro_obj = CoroutineMgr::Instance()->Insert(caller);
		func(coro_obj->id);
	}
	auto await_resume()
	{
		auto result = coro_obj->result;
		auto data = std::move(coro_obj->data);
		coro_obj = nullptr;
		return std::pair<CORORESULT, std::string>(result, data);
	}

	std::function<void(COROID)> func;
	std::shared_ptr<CoroutineMgr::CoroObj> coro_obj;
};

#endif // UFRM_COROUTINE_MGR_H
