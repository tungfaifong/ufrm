// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_LOOP_HPP
#define USRV_LOOP_HPP

#include <functional>
#include <thread>

#include "util/common.h"
#include "util/time.h"

NAMESPACE_OPEN

class Loop
{
public:
	using UpdateFunc = std::function<bool(intvl_t)>;

	Loop() = default;
	~Loop() = default;

public:
	void Init(intvl_t interval, UpdateFunc update)
	{
		_interval = interval;
		_update = update;
	}

	void Run()
	{
		auto now = StdNow();
		auto last = now;
		auto interval = Ns2Ms(now - last);
		auto busy = true;
		while (!_exit)
		{
			now = StdNow();
			interval = Ns2Ms(now - last);
			if (interval >= _interval || busy)
			{
				last = now;
				busy = _update(interval);
			}
			else
			{
				std::this_thread::sleep_for(ms_t(_interval - interval));
			}
		}
	}

	void Release()
	{
		_update = nullptr;
	}

	void SetExit(bool exit) { _exit = exit; }

	intvl_t Interval() { return _interval; }

private:
	bool _exit = false;
	intvl_t _interval;
	UpdateFunc _update;
};

NAMESPACE_CLOSE

#endif // USRV_LOOP_HPP
