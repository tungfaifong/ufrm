// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef UFRM_GATEWAY_H
#define UFRM_GATEWAY_H

#include "usrv/unit.h"

using namespace usrv;

class Gateway : public Unit
{
public:
	Gateway() = default;
	virtual ~Gateway() = default;

	virtual bool Init() override final;
	virtual bool Start() override final;
	virtual bool Update(intvl_t interval) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

private:

};

#endif // UFRM_GATEWAY_H
