#pragma once

#include <string>
#include <format>

#include "spdlog/spdlog.h"

class Logger
{
public:
    static void Init();

    template<typename T>
    static std::string to_str(T&& value) {
        using DT = std::decay_t<T>;

        if constexpr(std::is_same_v<DT, bool>) {
            return value ? "true" : "false";
        }
        else if constexpr(std::is_convertible_v<DT, std::string_view>) {
            return std::string(value);
        }
        else if constexpr(std::is_arithmetic_v<DT>) {
            return std::to_string(value);
        }
        else {
            return "???";
        }
    }

    template<typename... Args>
    static std::string format_extras(Args&&... args) {
        constexpr size_t total_args = sizeof...(args);
        if constexpr (total_args == 0) return "";
        static_assert(total_args % 2 == 0, "Argument must be pairs: (Key, Value)");

        std::string key_value_string = "";
        auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);

        [&]<std::size_t... pair_index>(std::index_sequence<pair_index...>) {
            ((key_value_string += " | " + 
                to_str(std::get<pair_index* 2>(tuple)) + "=" +
                to_str(std::get<pair_index * 2 + 1>(tuple))
            ), ...);
        }(std::make_index_sequence<total_args / 2>{}); 
    
        return key_value_string;
    }
};

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