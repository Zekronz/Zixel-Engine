/*
    Log.h
    Copyright (c) 2023-2023 Zekronz - MIT License
    https://github.com/Zekronz/Zixel-Engine
*/

#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Zixel {

	class Log {
	private:
		static std::shared_ptr<spdlog::logger> logger;
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetLogger() { return logger; }
	};
	 
}

#ifndef NDEBUG

	#define ZIXEL_TRACE(...) ::Zixel::Log::GetLogger()->trace(__VA_ARGS__)
	#define ZIXEL_INFO(...) ::Zixel::Log::GetLogger()->info(__VA_ARGS__)
	#define ZIXEL_WARN(...) ::Zixel::Log::GetLogger()->warn(__VA_ARGS__)
	#define ZIXEL_ERROR(...) ::Zixel::Log::GetLogger()->error(__VA_ARGS__)
	#define ZIXEL_CRITICAL(...) ::Zixel::Log::GetLogger()->critical(__VA_ARGS__)

#else

	#define ZIXEL_TRACE(...) 
	#define ZIXEL_INFO(...) 
	#define ZIXEL_WARN(...) 
	#define ZIXEL_ERROR(...) 
	#define ZIXEL_CRITICAL(...) 

#endif