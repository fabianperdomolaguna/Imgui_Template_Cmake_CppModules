#pragma once

#include <format>

#include "spdlog/spdlog.h"

import Logger;

#define LOG_CRITICAL(msg, ...) \
    spdlog::default_logger_raw()->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, \
    spdlog::level::critical, msg + Logger::format_extras(__VA_ARGS__))

#define LOG_ERROR(msg, ...) \
    spdlog::default_logger_raw()->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, \
    spdlog::level::err, msg + Logger::format_extras(__VA_ARGS__))

#define LOG_WARN(msg, ...) \
    spdlog::default_logger_raw()->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, \
    spdlog::level::warn, msg + Logger::format_extras(__VA_ARGS__))

#define LOG_INFO(msg, ...) \
    spdlog::default_logger_raw()->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, \
    spdlog::level::info, msg + Logger::format_extras(__VA_ARGS__))

#define LOG_TRACE(msg, ...) \
    spdlog::default_logger_raw()->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, \
    spdlog::level::trace, msg + Logger::format_extras(__VA_ARGS__))



