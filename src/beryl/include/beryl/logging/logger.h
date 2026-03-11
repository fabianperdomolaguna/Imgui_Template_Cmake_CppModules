#pragma once

#include <chrono>
#include <concepts>
#include <deque>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <source_location>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "nlohmann/json.hpp"

struct LogMessage 
{
    std::string_view message;
    std::source_location location;

    template <typename T>
    requires std::is_convertible_v<T, std::string_view>
    LogMessage(const T& s, std::source_location loc = std::source_location::current())
        : message(s), location(loc) {}
};

struct LogEntry 
{
    std::string message;
    spdlog::level::level_enum level{spdlog::level::info};
};

struct ImguiLogger 
{
    std::deque <LogEntry> items;
	std::mutex mutex;
    size_t max_size = 100;

    void AddLog(std::string log, spdlog::level::level_enum level)
    {
        std::scoped_lock lock(mutex);
        items.push_back({ std::move(log), level });
        if (items.size() > max_size)
            items.pop_front();
    }

    void Clear()
    {
        std::scoped_lock lock(mutex);
        items.clear();
    }
};

template<typename Mutex>
class ImguiLoggerSink : public spdlog::sinks::base_sink<Mutex>
{
public:
    static ImguiLogger& GetImguiLogger()
    {
        static ImguiLogger instance;
        return instance;
    }

protected:
    void sink_it_(const spdlog::details::log_msg& message) override
    {
        spdlog::memory_buf_t formatted;
        spdlog::sinks::base_sink<Mutex>::formatter_->format(message, formatted);
        GetImguiLogger().AddLog(fmt::to_string(formatted), message.level);
    }

    void flush_() override {};
};

template<typename Mutex>
class JsonFileLoggerSink : public spdlog::sinks::base_sink<Mutex>
{
    std::ofstream file;
    std::string filename;
    size_t max_size_bytes;
    size_t current_size = 0;

public:
    explicit JsonFileLoggerSink(const std::string& filename, 
        size_t max_bytes = static_cast<size_t>(512) * 1024) 
        : filename(filename), max_size_bytes(max_bytes)
    {
        file.open(filename, std::ios::app);
        if (!file.is_open())
            throw std::runtime_error("Sigma.Logger: Could not open log file: " + filename);
        file.seekp(0, std::ios::end);
        current_size = static_cast<size_t>(file.tellp());
    }

protected:
    void sink_it_(const spdlog::details::log_msg& message) override
    {
        nlohmann::ordered_json json;

        auto local_time = std::chrono::current_zone()->to_local(message.time);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(local_time.time_since_epoch()) % 1000;

        json["time"] = std::format("{:%Y-%m-%d %H:%M:%S}.{:03d}", 
            std::chrono::floor<std::chrono::seconds>(local_time), 
            ms.count());

        json["level"] = spdlog::level::to_string_view(message.level).data();
        json["thread"] = message.thread_id;

        std::string_view file_path = (message.source.filename != nullptr) ? message.source.filename : "";
        if (size_t src_pos = file_path.find("src/"); src_pos != std::string_view::npos) 
        {
            json["file"] = file_path.substr(src_pos);
        } 
        else 
        {
            size_t last_slash = file_path.find_last_of("/\\");
            json["file"] = (last_slash != std::string_view::npos) ? file_path.substr(last_slash + 1) : file_path;
        }

        json["line"] = message.source.line;
        json["function"] = (message.source.funcname != nullptr) ? message.source.funcname : "";

        std::string_view payload_view(message.payload.data(), message.payload.size());
        if (size_t extras_pos = payload_view.find(" | "); extras_pos != std::string_view::npos) 
        {
            json["message"] = payload_view.substr(0, extras_pos);
            std::string extras_str(payload_view.substr(extras_pos + 3)); 
            ProcessExtrasVariables(json["details"], extras_str);
        } 
        else 
        {
            json["message"] = payload_view;
        }

        std::string dumped_json = json.dump() + "\n";
        if (current_size + dumped_json.size() > max_size_bytes) 
            RotateLogs();
        file << dumped_json;
        current_size += dumped_json.size();
    }

    void RotateLogs() 
    {
        file.close();

        std::filesystem::path path(filename);
        std::filesystem::path backup_path =
            path.parent_path() / (path.stem().string() + ".old" + path.extension().string());
        std::filesystem::remove(backup_path);
        if (std::filesystem::exists(path))
            std::filesystem::rename(path, backup_path);
      
        file.open(filename, std::ios::trunc);
        current_size = 0;
    }

    void ProcessExtrasVariables(nlohmann::ordered_json& extras_json, const std::string& extras_string)
    {
        auto trim = [](std::string_view s) -> std::string {
            const auto first = s.find_first_not_of(' ');
            if (first == std::string_view::npos)
                return "";
            const auto last = s.find_last_not_of(' ');
            return std::string(s.substr(first, (last - first + 1)));
        };

        std::string_view sv(extras_string);
        size_t start = 0;
        size_t end = sv.find('|');
        
        while (true) 
        {
        std::string_view segment = sv.substr(start, end - start);
        
        if (size_t equal_pos = segment.find('='); equal_pos != std::string_view::npos) 
        {
            std::string key = trim(segment.substr(0, equal_pos));
            std::string val = trim(segment.substr(equal_pos + 1));
            extras_json[key] = val;
        }

        if (end == std::string_view::npos) 
            break;
        start = end + 1;
        end = sv.find('|', start);
        }
    }

    void flush_() override { file.flush(); }
};

template<typename T>
decltype(auto) ToView(T&& value) 
{
    using DT = std::decay_t<T>;
    if constexpr (std::is_same_v<DT, bool>) return value ? "true" : "false";
    else return std::forward<T>(value);
}

template<typename... Args>
std::string FormatExtras(Args&&... args) 
{
    constexpr size_t total_args = sizeof...(args);
    if constexpr (total_args == 0) return "";
    static_assert(total_args % 2 == 0, "Argument must be pairs: (Key, Value)");

    std::string key_value_string;
    key_value_string.reserve(128);

    auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);

    [&] <std::size_t... index>(std::index_sequence<index...>) {
        (std::format_to(std::back_inserter(key_value_string), " | {}={}",
            ToView(std::get<index * 2>(tuple)),
            ToView(std::get<(index * 2) + 1>(tuple))), ...);
    }(std::make_index_sequence<sizeof...(args) / 2>{});

    return key_value_string;
}

template <typename... Args>
void LogInternal(spdlog::level::level_enum level, const std::source_location& loc,
    std::string_view message, Args&&... args)
{
    auto* logger = spdlog::default_logger_raw();
    if (logger && logger->should_log(level)) 
    {
        std::string extras = FormatExtras(std::forward<Args>(args)...);
        logger->log(
            spdlog::source_loc{ loc.file_name(), static_cast<int>(loc.line()), loc.function_name() },
            level, "{}{}", message, extras);
    }
}

namespace beryl::logger
{
    void Init(const std::string& log_filename = "logs.json");
    ImguiLogger& GetGUILogger();
    void Shutdown();

    void Critical(std::string_view msg, const std::source_location loc = std::source_location::current());

    template <typename... Args> 
    void Critical(LogMessage msg, Args&&... args)
    {
        LogInternal(spdlog::level::critical, msg.location, msg.message, std::forward<Args>(args)...);
    }

    void Error(std::string_view msg, const std::source_location loc = std::source_location::current());

    template <typename... Args> 
    void Error(LogMessage msg, Args&&... args)
    {
        LogInternal(spdlog::level::err, msg.location, msg.message, std::forward<Args>(args)...);
    }

    void Warn(std::string_view msg, const std::source_location loc = std::source_location::current());

    template <typename... Args> 
    void Warn(LogMessage msg, Args&&... args)
    {
        LogInternal(spdlog::level::warn, msg.location, msg.message, std::forward<Args>(args)...);
    }

    void Info(std::string_view msg, const std::source_location loc = std::source_location::current());

    template <typename... Args> 
    void Info(LogMessage msg, Args&&... args)
    {
        LogInternal(spdlog::level::info, msg.location, msg.message, std::forward<Args>(args)...);
    }

    void Trace(std::string_view msg, const std::source_location loc = std::source_location::current());

    template <typename... Args> 
    void Trace(LogMessage msg, Args&&... args)
    {
        LogInternal(spdlog::level::trace, msg.location, msg.message, std::forward<Args>(args)...);
    }
}