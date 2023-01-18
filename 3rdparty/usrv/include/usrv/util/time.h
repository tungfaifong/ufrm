// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_TIME_H
#define USRV_TIME_H

#include <chrono>

#include "common.h"

NAMESPACE_OPEN

using sys_clock_t = std::chrono::system_clock::time_point;
using std_clock_t = std::chrono::steady_clock::time_point;
using s_t = std::chrono::seconds;
using ms_t = std::chrono::milliseconds;
using ns_t = std::chrono::nanoseconds;

static constexpr uint32_t MINUTE_SEC = 60;
static constexpr uint32_t HOUR_SEC = 60 * MINUTE_SEC;
static constexpr uint32_t DAY_SEC = 24 * HOUR_SEC;
static constexpr uint32_t WEEK_SEC = 7 * DAY_SEC;
static constexpr uint32_t SEC2MILLISEC = 1000;

inline sys_clock_t SysNow()
{
	return std::chrono::system_clock::now();
}

inline std_clock_t StdNow()
{
	return std::chrono::steady_clock::now();
}

inline intvl_t Ns2Ms(ns_t ns)
{
	return std::chrono::duration_cast<ms_t>(ns).count();
}

NAMESPACE_CLOSE

#endif // USRV_TIME_H
