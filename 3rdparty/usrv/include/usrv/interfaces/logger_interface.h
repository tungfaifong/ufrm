// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#ifndef USRV_LOGGER_INTERFACE_H
#define USRV_LOGGER_INTERFACE_H

#include "lua.hpp"

#include "unit_manager.h"
#include "units/logger_unit.h"
#include "util/common.h"

NAMESPACE_OPEN

namespace logger
{

template<typename ... Args>
inline void log(LoggerUnit::Level level, fmt::format_string<Args...> fmt, Args && ... args)
{
	std::dynamic_pointer_cast<LoggerUnit>(UnitManager::Instance()->Get("LOGGER"))->Log(level, fmt, std::forward<Args>(args)...);
}

template<typename ... Args>
inline void log(const char * file, int32_t line, const char * func, LoggerUnit::Level level, fmt::format_string<Args...> fmt, Args && ... args)
{
	log(level, "[{}:{}:{}()] {}", file, line, func, fmt::format(fmt, std::forward<Args>(args)...));
}

inline void flush()
{
	std::dynamic_pointer_cast<LoggerUnit>(UnitManager::Instance()->Get("LOGGER"))->Flush();
}

#define LOGGER_TRACE(fmt, ...) logger::log(__FILE__, __LINE__, __FUNCTION__, LoggerUnit::Level::TRACE, fmt, ##__VA_ARGS__)
#define LOGGER_DEBUG(fmt, ...) logger::log(__FILE__, __LINE__, __FUNCTION__, LoggerUnit::Level::DEBUG, fmt, ##__VA_ARGS__)
#define LOGGER_INFO(fmt, ...) logger::log(__FILE__, __LINE__, __FUNCTION__, LoggerUnit::Level::INFO, fmt, ##__VA_ARGS__)
#define LOGGER_WARN(fmt, ...) logger::log(__FILE__, __LINE__, __FUNCTION__, LoggerUnit::Level::WARN, fmt, ##__VA_ARGS__)
#define LOGGER_ERROR(fmt, ...) logger::log(__FILE__, __LINE__, __FUNCTION__, LoggerUnit::Level::ERROR, fmt, ##__VA_ARGS__)
#define LOGGER_CRITICAL(fmt, ...) logger::log(__FILE__, __LINE__, __FUNCTION__, LoggerUnit::Level::CRITICAL, fmt, ##__VA_ARGS__)
#define LOGGER_FLUSH() logger::flush()

inline void lua_log(uint8_t level, const std::string & msg, lua_State *L)
{
	lua_Debug info;
	lua_getstack(L, 1, &info);
	lua_getinfo(L, "Sln", &info);
	log(info.short_src, info.currentline, (info.name ? info.name : "?"), (LoggerUnit::Level)level, "{}", msg);
};

inline void lua_trace(const std::string & msg, lua_State *L) { lua_log(LoggerUnit::Level::TRACE, msg, L); }
inline void lua_debug(const std::string & msg, lua_State *L) { lua_log(LoggerUnit::Level::DEBUG, msg, L); }
inline void lua_info(const std::string & msg, lua_State *L) { lua_log(LoggerUnit::Level::INFO, msg, L); }
inline void lua_warn(const std::string & msg, lua_State *L) { lua_log(LoggerUnit::Level::WARN, msg, L); }
inline void lua_error(const std::string & msg, lua_State *L) { lua_log(LoggerUnit::Level::ERROR, msg, L); }
inline void lua_critical(const std::string & msg, lua_State *L) { lua_log(LoggerUnit::Level::CRITICAL, msg, L); }

}

NAMESPACE_CLOSE

#endif // USRV_LOGGER_INTERFACE_H
