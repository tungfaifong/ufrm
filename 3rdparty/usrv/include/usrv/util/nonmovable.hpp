// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_NONMOVABLE_HPP
#define USRV_NONMOVABLE_HPP

#include "common.h"

NAMESPACE_OPEN

class nonmovable
{
public:
	nonmovable(const nonmovable&&) = delete;
	const nonmovable& operator=(const nonmovable&&) = delete;

protected:
	nonmovable() = default;
	~nonmovable() = default;
}; 

NAMESPACE_CLOSE

#endif // USRV_NONMOVABLE_HPP
