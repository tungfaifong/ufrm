// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_TIMER_UNIT_H
#define USRV_TIMER_UNIT_H

#include <functional>
#include <queue>
#include <time.h>

#include "unit.h"
#include "util/heap.hpp"
#include "util/object_pool.hpp"
#include "util/time.h"

NAMESPACE_OPEN

class Timer
{
public:
	Timer() = default;
	virtual ~Timer() = default;

	bool operator < (const Timer & t) const
	{
		return _time < t._time;
	}

public:
	void Start(std_clock_t time, std::function<void()> && callback);
	void Stop();

	bool Call();

private:
	std_clock_t _time;
	std::function<void()> _callback;
	friend class TimerUnit;
};

class TimerUnit : public Unit
{
public:
	TimerUnit(size_t tp_alloc_num, size_t ts_alloc_num);
	virtual ~TimerUnit() = default;

	virtual bool Update(intvl_t interval) override final;

public:
	TIMERID CreateTimer(intvl_t time, std::function<void()> && callback);
	bool CallTimer(TIMERID id);
	bool RemoveTimer(TIMERID id);

private:
	void _RemoveTimer(std::shared_ptr<Timer> && timer);

private:
	ObjectPool<Timer> _timer_pool;
	Heap<std::shared_ptr<Timer>> _timers;
};

NAMESPACE_CLOSE

#endif // USRV_TIMER_UNIT_H
