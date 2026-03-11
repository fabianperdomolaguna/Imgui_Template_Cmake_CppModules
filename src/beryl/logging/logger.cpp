#include "beryl/logging/logger.h"

void beryl::logger::Init(const std::string& log_filename)
{
    spdlog::init_thread_pool(8192, 1);

    auto imgui_sink = std::make_shared<ImguiLoggerSink<std::mutex>>();
    imgui_sink->set_pattern("[%T.%e] [%l] %v");

    auto file_sink = std::make_shared<JsonFileLoggerSink<std::mutex>>(log_filename);
    file_sink->set_level(spdlog::level::warn);

    std::vector<spdlog::sink_ptr> sinks{ imgui_sink, file_sink };

    auto logger = std::make_shared<spdlog::async_logger>(
        "sigma_logger", 
        sinks.begin(), 
        sinks.end(), 
        spdlog::thread_pool(), 
        spdlog::async_overflow_policy::overrun_oldest
    );
    
    logger->set_level(spdlog::level::trace);
    logger->flush_on(spdlog::level::warn);
    spdlog::set_default_logger(logger);
}

ImguiLogger& beryl::logger::GetGUILogger()
{
    return ImguiLoggerSink<std::mutex>::GetImguiLogger();
}

void beryl::logger::Shutdown()
{
    spdlog::shutdown();
}

void beryl::logger::Critical(std::string_view msg, const std::source_location loc)
{
    LogInternal(spdlog::level::critical, loc, msg);
}

void beryl::logger::Error(std::string_view msg, const std::source_location loc)
{
    LogInternal(spdlog::level::err, loc, msg);
}

void beryl::logger::Warn(std::string_view msg, const std::source_location loc)
{
    LogInternal(spdlog::level::warn, loc, msg);
}

void beryl::logger::Info(std::string_view msg, const std::source_location loc)
{
    LogInternal(spdlog::level::info, loc, msg);
}

void beryl::logger::Trace(std::string_view msg, const std::source_location loc)
{
    LogInternal(spdlog::level::trace, loc, msg);
}
