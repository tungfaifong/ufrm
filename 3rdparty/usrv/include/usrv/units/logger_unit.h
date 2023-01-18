// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_LOGGER_UNIT_H
#define USRV_LOGGER_UNIT_H

#include <map>
#include <thread>

#include "spdlog/sinks/daily_file_sink.h"
#include "fmt/core.h"

#include "components/loop.hpp"
#include "unit.h"
#include "util/spsc_queue.hpp"

NAMESPACE_OPEN

class LoggerUnit : public Unit, public std::enable_shared_from_this<LoggerUnit>
{
public:
	static constexpr uint16_t MAX_LOG_SIZE = UINT16_MAX;
	enum class Level
	{
		TRACE = 0,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		CRITICAL,
		OFF,
		COUNT,
	};

	LoggerUnit(Level level, std::string file_name, size_t spsc_blk_num);
	virtual ~LoggerUnit();

	virtual void OnRegister(const std::shared_ptr<UnitManager> & mgr) override final;
	virtual void Stop() override final;
	virtual void Release() override final;

public:
	template<typename ... Args> void Log(Level level, fmt::format_string<Args...> fmt, Args && ... args);
	void Flush();

private:
	void _Init();
	void _LogStart();
	bool _LogUpdate(intvl_t interval);
	void _RealLog(Level level, const char * log, uint16_t size);

private:
	Level _level;
	std::string _file_name;
	Loop _loop;
	std::thread _log_thread;
	size_t _spsc_blk_num;
	std::unordered_map<std::thread::id, std::shared_ptr<SpscQueue>> _log_queues;
	std::shared_ptr<spdlog::logger> _logger;
	char _log_buffer[MAX_LOG_SIZE];
};

template<typename ... Args> void LoggerUnit::Log(Level level, fmt::format_string<Args...> fmt, Args && ... args)
{
	if(level < _level)
	{
		return;
	}

	auto log = fmt::format(fmt, std::forward<Args>(args)...);

	SpscQueue::Header header;
	header.size = log.size();
	header.data16 = static_cast<uint16_t>(level);
	header.data32 = 0;

	auto tid = std::this_thread::get_id();
	if(_log_queues.find(tid) == _log_queues.end())
	{
		_log_queues[tid] = std::move(std::make_shared<SpscQueue>(_spsc_blk_num));
	}

	_log_queues[tid]->Push(log.c_str(), header);
}

NAMESPACE_CLOSE

#endif // USRV_LOGGER_UNIT_H
