#pragma once

#include <format>

#include "spdlog/spdlog.h"

import Logger;

#define LOG_INTERNAL_CALL(level, message, ...) \
    do { \
        auto logger_ptr = spdlog::default_logger_raw(); \
        if (logger_ptr->should_log(level)) { \
            logger_ptr->log(spdlog::source_loc{__FILE__, __LINE__, __func__}, \
            level, "{}{}", message, Logger::FormatExtras(__VA_ARGS__)); \
        } \
    } while (0)

#define LOG_CRITICAL(message, ...) LOG_INTERNAL_CALL(spdlog::level::critical, message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...)    LOG_INTERNAL_CALL(spdlog::level::err,      message, ##__VA_ARGS__)
#define LOG_WARN(message, ...)     LOG_INTERNAL_CALL(spdlog::level::warn,     message, ##__VA_ARGS__)
#define LOG_INFO(message, ...)     LOG_INTERNAL_CALL(spdlog::level::info,     message, ##__VA_ARGS__)
#define LOG_TRACE(message, ...)    LOG_INTERNAL_CALL(spdlog::level::trace,    message, ##__VA_ARGS__)
