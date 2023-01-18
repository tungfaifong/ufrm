// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_H
#define UFRM_COROUTINE_H

#include <coroutine>
#include <functional>
#include <memory>

#include "usrv/util/time.h"

using namespace usrv;

struct promise;
struct final_awaitable;
struct future;

using coroutine = std::coroutine_handle<promise>;

struct promise
{
	future get_return_object();
	std::suspend_always initial_suspend() noexcept { return {}; }
	final_awaitable final_suspend() noexcept;
	void return_void() {}
	void unhandled_exception() {}

	coroutine caller {nullptr};
};

struct awaitable
{
	bool await_ready() { return false; }
	auto await_suspend(coroutine caller)
	{
		coro.promise().caller = caller;
		return coro;
	}
	auto await_resume() {}

	coroutine coro;
};

struct final_awaitable
{
	bool await_ready() noexcept { return false; }
	std::coroutine_handle<> await_suspend(coroutine coro) noexcept
	{
		if(!coro.promise().caller)
		{
			return std::noop_coroutine();
		}
		return coro.promise().caller;
	}
	void await_resume() noexcept {}
};

struct future
{
	using promise_type = struct promise;
	
	auto operator co_await()
	{
		return awaitable{ coro };
	}

	coroutine coro;
};

#endif // UFRM_COROUTINE_H
