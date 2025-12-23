#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include "logging/logger.h"

void Logger::Init()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");

    auto logger = std::make_shared<spdlog::logger>("dev_logger", console_sink);
    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
}