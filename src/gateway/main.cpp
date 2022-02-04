// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include <signal.h>

#include "usrv/unit_manager.h"
#include "usrv/units/logger_unit.h"
#include "usrv/units/server_unit.h"
#include "usrv/units/timer_unit.h"

#include "gateway.h"

using namespace usrv;

void SignalHandler(int signum)
{
	if(signum == SIGUSR1)
	{
		UnitManager::Instance()->SetExit(true);
	}
}

int main()
{
	signal(SIGUSR1, SignalHandler);

	UnitManager::Instance()->Init(1);

	UnitManager::Instance()->Register("LOGGER", std::move(std::make_shared<LoggerUnit>(LoggerUnit::Level::TRACE, "/logs/gateway.log", 1 Mi)));
	UnitManager::Instance()->Register("SERVER", std::move(std::make_shared<ServerUnit>(1 Ki, 1 Ki, 1 Mi)));
	UnitManager::Instance()->Register("ISERVER", std::move(std::make_shared<ServerUnit>(1 Ki, 1 Ki, 1 Mi)));
	UnitManager::Instance()->Register("TIMER", std::move(std::make_shared<TimerUnit>(1 Ki, 1 Ki)));
	UnitManager::Instance()->Register("GATEWAY", std::move(std::make_shared<Gateway>()));

	UnitManager::Instance()->Run();
}
