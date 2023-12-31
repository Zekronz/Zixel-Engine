/*
    Log.cpp
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#include "Engine/Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Zixel {

	std::shared_ptr<spdlog::logger> Log::logger;

	void Log::Init() {
		spdlog::set_pattern("%^[%T] [%n]: %v%$");
		logger = spdlog::stdout_color_mt("Zixel");
		logger->set_level(spdlog::level::trace);
	}

}