// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_H
#define UFRM_COROUTINE_H

#include <coroutine>
#include <functional>
#include <memory>

#include "usrv/util/time.h"

using namespace usrv;

template<typename T>
struct future;

template<typename T>
struct final_awaitable;

template<typename T>
struct promise
{
	future<T> get_return_object();
	std::suspend_always initial_suspend() noexcept { return {}; }
	final_awaitable<T> final_suspend() noexcept;
	void unhandled_exception() {}
	void return_value(T value) noexcept
	{
		value = value;
	}
	T result() { return value; }

	std::coroutine_handle<promise<T>> caller {nullptr};
	T value;
};

template<>
struct promise<void>
{
	future<void> get_return_object();
	std::suspend_always initial_suspend() noexcept { return {}; }
	final_awaitable<void> final_suspend() noexcept;
	void unhandled_exception() {}
	void return_void() {}
	void result() {}

	std::coroutine_handle<promise<void>> caller {nullptr};
};

template<typename T>
struct awaitable
{
	bool await_ready() { return false; }
	auto await_suspend(std::coroutine_handle<promise<T>> caller)
	{
		coro.promise().caller = caller;
		return coro;
	}
	auto await_resume()
	{
		return coro.promise().result();
	}

	std::coroutine_handle<promise<T>> coro;
};

template<typename T>
struct final_awaitable
{
	bool await_ready() noexcept { return false; }
	std::coroutine_handle<> await_suspend(std::coroutine_handle<promise<T>> coro) noexcept
	{
		if(!coro.promise().caller)
		{
			return std::noop_coroutine();
		}
		return coro.promise().caller;
	}
	void await_resume() noexcept {}
};

template<typename T = void>
struct future
{
	using promise_type = struct promise<T>;
	
	auto operator co_await()
	{
		return awaitable<T>{ coro };
	}

	std::coroutine_handle<promise<T>> coro;
};

template<typename T>
future<T> promise<T>::get_return_object()
{
	return {std::coroutine_handle<promise<T>>::from_promise(*this)};
}

template<typename T>
final_awaitable<T> promise<T>::final_suspend() noexcept
{
	return {};
}

inline future<void> promise<void>::get_return_object()
{
	return {std::coroutine_handle<promise<void>>::from_promise(*this)};
}

inline final_awaitable<void> promise<void>::final_suspend() noexcept
{
	return {};
}

#endif // UFRM_COROUTINE_H
