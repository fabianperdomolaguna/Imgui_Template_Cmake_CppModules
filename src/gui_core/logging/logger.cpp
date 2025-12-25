#include <mutex>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

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
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");

    auto imgui_sink = std::make_shared<imgui_sink_mt>();
    imgui_sink->set_pattern("[%T.%e] [%l] %v");

    std::vector<spdlog::sink_ptr> sinks{ console_sink, imgui_sink };
    auto logger = std::make_shared<spdlog::logger>("multi_sink", sinks.begin(), sinks.end());

    logger->set_level(spdlog::level::trace);
    spdlog::set_default_logger(logger);
}

ImguiLogger& Logger::GetGUILogger()
{
    return imgui_sink_mt::GetImguiLogger();
}