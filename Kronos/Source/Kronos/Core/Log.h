#pragma once

#include "Kronos/Core/Memory.h"

#include <spdlog/spdlog.h>

namespace Kronos {

	class Log
	{
	public:
		static void Initialize();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
	};

}

#define KRONOS_CORE_TRACE(...) ::Kronos::Log::GetCoreLogger()->trace(__VA_ARGS__);
#define KRONOS_CORE_INFO(...) ::Kronos::Log::GetCoreLogger()->info(__VA_ARGS__);
#define KRONOS_CORE_WARN(...) ::Kronos::Log::GetCoreLogger()->warn(__VA_ARGS__);
#define KRONOS_CORE_ERROR(...) ::Kronos::Log::GetCoreLogger()->error(__VA_ARGS__);
#define KRONOS_CORE_CRITICAL(...) ::Kronos::Log::GetCoreLogger()->critical(__VA_ARGS__);