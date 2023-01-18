// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_MGR_H
#define UFRM_COROUTINE_MGR_H

#include "coroutine.h"
#include "usrv/util/time.h"
#include "usrv/util/object_map.hpp"
#include "usrv/util/singleton.hpp"

using namespace usrv;

class CoroutineMgr : public Singleton<CoroutineMgr>, public std::enable_shared_from_this<CoroutineMgr>
{
public:
	static constexpr size_t ALLOC_NUM = 1024;
	static constexpr intvl_t TIMEOUT = 5000;

	CoroutineMgr() = default;
	~CoroutineMgr() = default;

	bool Start();
	void Stop();

public:
	void Spawn(std::function<coroutine()> func);
	void Resume(COROID coro_id, CORORESULT result, std::string && data);
	void Update();

private:
	void _CheckTimeout();

private:
	ObjectMap<coroutine> _coroutines {ALLOC_NUM};
	TIMERID _timer_handler {INVALID_TIMER_ID};
};

#endif // UFRM_COROUTINE_MGR_H
