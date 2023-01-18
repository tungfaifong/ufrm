// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_H
#define UFRM_COROUTINE_H

#include <coroutine>
#include <functional>
#include <memory>

#include "usrv/util/time.h"

using namespace usrv;

using COROID = size_t;
enum class CORORESULT
{
	SUCCESS = 0,
	TIMEOUT,
};

struct promise;

struct coroutine : std::coroutine_handle<promise>
{
	using promise_type = struct promise;

	COROID id;
	std_clock_t timeout;
	CORORESULT result;
	std::string data;
};

struct promise
{
	coroutine get_return_object() { return {coroutine::from_promise(*this)}; }
	std::suspend_always initial_suspend() noexcept { return {}; }
	std::suspend_never final_suspend() noexcept { return {}; }
	void return_void() { coro = nullptr; }
	void unhandled_exception() { coro = nullptr; }

	std::shared_ptr<coroutine> coro;
};

struct awaitable
{
	awaitable(std::function<void(COROID)> func) : coro(nullptr), func(func) {}
	bool await_ready() { return false; }
	void await_suspend(std::coroutine_handle<promise> h)
	{
		coro = h.promise().coro;
		func(coro->id);
	}
	auto await_resume()
	{
		auto result = coro->result;
		auto data = std::move(coro->data);
		coro = nullptr;
		return std::pair<CORORESULT, std::string>(result, data);
	}

	std::shared_ptr<coroutine> coro;
	std::function<void(COROID)> func;
};

#endif // UFRM_COROUTINE_H
