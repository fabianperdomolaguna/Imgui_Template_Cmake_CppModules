#include <mutex>

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
//#include "spdlog/sinks/stdout_color_sinks.h"

#include "logging/logger.h"

void ImguiLogger::AddLog(std::string log, spdlog::level::level_enum level)
{
    std::lock_guard<std::mutex> lock(mutex);
    items.push_back({ std::move(log), level });
    if (items.size() > max_size)
        items.pop_front();
}

void ImguiLogger::Clear()
{
    std::lock_guard<std::mutex> lock(mutex);
    items.clear();
}

void Logger::Init()
{
    spdlog::init_thread_pool(8192, 1);

    //auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    //console_sink->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");

    auto imgui_sink = std::make_shared<imgui_sink_mt>();
    imgui_sink->set_pattern("[%T.%e] [%l] %v");

    //Define JSON file sink with max size 0.5 MB
    auto file_sink = std::make_shared<json_file_sink_mt>("logs.json", static_cast<size_t>(0.5*1024*1024));
    file_sink->set_pattern("{\"time\":\"%Y-%m-%dT%H:%M:%S.%f\", \"level\":\"%l\", \"message\":\"%v\"}");
    file_sink->set_level(spdlog::level::warn);

    std::vector<spdlog::sink_ptr> sinks{ imgui_sink, file_sink };
    auto logger = std::make_shared<spdlog::async_logger>(
        "async_logger", 
        sinks.begin(), 
        sinks.end(), 
        spdlog::thread_pool(), 
        spdlog::async_overflow_policy::block
    );

    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
}

ImguiLogger& Logger::GetGUILogger()
{
    return imgui_sink_mt::GetImguiLogger();
}