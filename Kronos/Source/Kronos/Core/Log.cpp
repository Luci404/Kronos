#include "Kronos/Core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Kronos {

	Ref<spdlog::logger> Log::s_CoreLogger;

	void Log::Initialize()
	{
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>()); // stdout
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("Kronos.log", true)); // File

		logSinks[0]->set_pattern("%^[%T] %n: %v%$"); // stdout
		logSinks[1]->set_pattern("[%T] [%l] %n: %v"); // file

		s_CoreLogger = std::make_shared<spdlog::logger>("KRONOS", begin(logSinks), end(logSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);
	}

}