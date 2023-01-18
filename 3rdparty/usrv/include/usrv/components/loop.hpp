// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_LOOP_HPP
#define USRV_LOOP_HPP

#include <condition_variable>
#include <functional>
#include <mutex>
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
				std::unique_lock<std::mutex> lock(_mutex);
				_ready = false;
				_cv.wait_for(lock, ms_t(_interval - interval), [this]{ return this->_ready; });
				busy = true;
			}
		}
	}

	void Release()
	{
		_update = nullptr;
	}

	void SetExit(bool exit) { _exit = exit; }

	intvl_t Interval() { return _interval; }

	void Notify()
	{
		{
			std::unique_lock<std::mutex> lock(_mutex);
			_ready = true;
		}
		_cv.notify_one();
	}

private:
	bool _exit = false;
	intvl_t _interval;
	UpdateFunc _update;
	bool _ready = false;
	std::condition_variable _cv;
	std::mutex _mutex;
};

NAMESPACE_CLOSE

#endif // USRV_LOOP_HPP
