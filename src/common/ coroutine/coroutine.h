// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_H
#define UFRM_COROUTINE_H

#include <coroutine>
#include <functional>
#include <memory>

using COROID = size_t;
enum CORORESULT
{
	CR_SUCCESS = 0,
	CR_TIMEOUT,
};

struct promise;

struct coroutine : std::coroutine_handle<promise>
{
	using promise_type = struct promise;

	COROID id;
	CORORESULT result;
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
	bool await_ready() { return false; }
	void await_suspend(std::coroutine_handle<promise> h)
	{
		coro = h.promise().coro;
		func();
	}
	CORORESULT await_resume()
	{
		auto result = coro->result;
		coro = nullptr;
		return result;
	}

	std::shared_ptr<coroutine> coro;
	std::function<void()> func;
};

// UFRM_COROUTINE_H
