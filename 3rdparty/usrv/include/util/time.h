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

inline bool CheckSecond(int sec)
{
	return sec >= 0 && sec < 60;
}

inline bool CheckMinute(int min)
{
	return min >= 0 && min < 60;
}

inline bool CheckHour(int hour)
{
	return hour >= 0 && hour < 24;
}

inline bool CheckWDay(int wday)
{
	return wday >= 0 && wday < 7;
}

inline bool CheckMonth(int mon)
{
	return mon >= 0 && mon < 12;
}

inline int GetMaxMDay(int year, int month)
{
	static constexpr int MAX_MDAY[12] = { 31,-1,31,30,31,30,31,31,30,31,30,31 };
	int max_mday = MAX_MDAY[month];
	if (1 == month) {
		max_mday = ((((0 == year % 4) && (0 != year % 100)) || (0 == year % 400)) ? 29 : 28);
	}
	return max_mday;
}

inline bool CheckMDay(int mday, int year, int month)
{
	return mday > 0 && mday <= GetMaxMDay(year, month);
}

time_t Now();

sys_clock_t SysNow();

std_clock_t StdNow();

intvl_t Ns2Ms(ns_t ns);

int TimeZone(bool recal = false);

int NextMinuteInterval(int second);

int NextHourInterval(int minute, int second);

int NextDayInterval(int hour, int minute, int second);

int NextWeekInterval(int wday, int hour, int minute, int second);

int NextMonthInterval(int mday, int hour, int minute, int second);

NAMESPACE_CLOSE

#endif // USRV_TIME_H
