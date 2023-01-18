// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include "coroutine.h"

future promise::get_return_object()
{
	return {coroutine::from_promise(*this)};
}

final_awaitable promise::final_suspend() noexcept
{
	return {};
}
