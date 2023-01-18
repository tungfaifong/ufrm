// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_COROUTINE_MGR_H
#define UFRM_COROUTINE_MGR_H

#include "corouine.h"
#include "usrv/util/object_map.hpp"

using namespace usrv;

class CoroutineMgr
{
public:
	CoroutineMgr() = default;
	~CoroutineMgr() = default;

public:
	void Spawn(std::function<coroutine()> func);
	void Resume(COROID coro_id);
	void Update();

private:
	void _Resume(std::shared_ptr<coroutine> coro, CORORESULT result);

private:
	ObjectMap<coroutine> _coroutines;
};

// UFRM_COROUTINE_MGR_H
