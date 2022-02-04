// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_UNIT_H
#define USRV_UNIT_H

#include <memory>

#include "util/common.h"

NAMESPACE_OPEN

class UnitManager;

class Unit
{
public:
	Unit() = default;
	virtual ~Unit() = default;

	virtual void OnRegister(const std::shared_ptr<UnitManager> & mgr) { _mgr = mgr; }
	virtual bool Init() { return true; }
	virtual bool Start() { return true; }
	virtual bool Update(intvl_t interval) { return false; }
	virtual void Stop() {}
	virtual void Release() { _mgr = nullptr; }

protected:
	std::shared_ptr<UnitManager> _mgr;
};

NAMESPACE_CLOSE

#endif // USRV_UNIT_H
