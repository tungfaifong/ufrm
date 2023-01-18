// Copyright (c) 2022 TungFai Fong <iam@tungfaifong.com>

#include <iostream>
#include <signal.h>

#include "toml++/toml.h"
#include "usrv/units/logger_unit.h"
#include "usrv/units/server_unit.h"
#include "usrv/units/timer_unit.h"
#include "usrv/unit_manager.h"

#include "gateway.h"

using namespace usrv;

void SignalHandler(int signum)
{
	if(signum == SIGUSR1)
	{
		UnitManager::Instance()->SetExit(true);
	}
}

int main(int argc, char * argv[])
{
	if(argc != 2)
	{
		std::cerr << "arg invalid: gateway config_path" << std::endl;
		return 1;
	}

	signal(SIGUSR1, SignalHandler);

	toml::table config;
	try
	{
		config = toml::parse_file(PATH_ROOT + argv[1]);
	}
	catch (const toml::parse_error& err)
	{
		std::cerr << "Parsing config failed: " << err << std::endl;
		return 1;
	}

	UnitManager::Instance()->Init(config["Gateway"]["interval"].value_or(1));

	UnitManager::Instance()->Register("LOGGER", std::move(std::make_shared<LoggerUnit>((LoggerUnit::Level)config["Logger"]["level"].value_or(0), config["Logger"]["file_name"].value_or("/logs/gateway.log"), config["Logger"]["spsc_blk_num"].value_or(512 Ki))));
	UnitManager::Instance()->Register("SERVER", std::move(std::make_shared<ServerUnit>(config["Server"]["pp_alloc_num"].value_or(1 Ki), config["Server"]["ps_alloc_num"].value_or(1 Ki), config["Server"]["spsc_blk_num"].value_or(512 Ki))));
	UnitManager::Instance()->Register("ISERVER", std::move(std::make_shared<ServerUnit>(config["IServer"]["pp_alloc_num"].value_or(1 Ki), config["IServer"]["ps_alloc_num"].value_or(1 Ki), config["IServer"]["spsc_blk_num"].value_or(512 Ki))));
	UnitManager::Instance()->Register("TIMER", std::move(std::make_shared<TimerUnit>(config["Timer"]["tp_alloc_num"].value_or(1 Ki), config["Timer"]["ts_alloc_num"].value_or(1 Ki))));
	UnitManager::Instance()->Register("GATEWAY", std::move(std::make_shared<Gateway>(config["Gateway"]["id"].value_or(INVALID_NODE_ID), config)));

	UnitManager::Instance()->Run();

	return 0;
}
