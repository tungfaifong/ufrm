// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_NONCOPYABLE_HPP
#define USRV_NONCOPYABLE_HPP

#include "common.h"

NAMESPACE_OPEN

class noncopyable
{
public:
	noncopyable(const noncopyable&) = delete;
	const noncopyable& operator=(const noncopyable&) = delete;

protected:
	noncopyable() = default;
	~noncopyable() = default;
}; 

NAMESPACE_CLOSE

#endif // USRV_NONCOPYABLE_HPP
