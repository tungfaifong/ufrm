// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_MGR_H
#define UFRM_COROUTINE_MGR_H

#include "corouine.h"
#include "usrv/util/time.h"
#include "usrv/util/object_map.hpp"

using namespace usrv;

class CoroutineMgr
{
public:
	static constexpr size_t ALLOC_NUM = 1024;
	static constexpr intvl_t TIMEOUT = 5000;

	CoroutineMgr() = default;
	~CoroutineMgr() = default;

public:
	bool Start();
	void Stop();

	void Spawn(std::function<coroutine()> func);
	void Resume(COROID coro_id);
	void Update();

private:
	void _CheckTimeout();

private:
	ObjectMap<coroutine> _coroutines {TIMEOUT};
	TIMERID _timer_handler {INVALID_TIMER_ID};
};

// UFRM_COROUTINE_MGR_H
