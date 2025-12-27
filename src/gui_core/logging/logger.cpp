module;

#include <string>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#include <mutex>
#include <chrono>

#include "spdlog/spdlog.h"
#include "spdlog/async.h"
//#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/base_sink.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "nlohmann/json.hpp"

export module Logger;

export struct LogEntry {
    std::string message;
    spdlog::level::level_enum level;
};

export struct ImguiLogger {
    std::deque <LogEntry> items;
	std::mutex mutex;
    size_t max_size = 100;

    void AddLog(std::string log, spdlog::level::level_enum level)
    {
        std::lock_guard<std::mutex> lock(mutex);
        items.push_back({ std::move(log), level });;
        if (items.size() > max_size)
            items.pop_front();
    }

    void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex);
        items.clear();
    }
};

template<typename mutex>
class ImguiLoggerSink : public spdlog::sinks::base_sink<mutex>
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
        spdlog::sinks::base_sink<mutex>::formatter_->format(message, formatted);
        GetImguiLogger().AddLog(fmt::to_string(formatted), message.level);
    }

    void flush_() override {};
};

template<typename mutex>
class JsonFileLoggerSink : public spdlog::sinks::base_sink<mutex>
{
    std::ofstream file;
    std::string filename;
    size_t max_size_bytes;
    size_t current_size;

public:
    JsonFileLoggerSink(const std::string& filename, size_t max_bytes) 
        : filename(filename), max_size_bytes(max_bytes), current_size(0)
    {
        file.open(filename, std::ios::app);
        if (file.is_open()) {
            file.seekp(0, std::ios::end); 
            current_size = static_cast<size_t>(file.tellp());
        }
    }

protected:
    void sink_it_(const spdlog::details::log_msg& message) override
    {
        nlohmann::ordered_json json;

        std::time_t now_time = std::chrono::system_clock::to_time_t(message.time);
        std::tm calendar_time;

        #ifdef _WIN32
            localtime_s(&calendar_time, &now_time);
        #else
            localtime_r(&now_time, &calendar_time);
        #endif

        auto miliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(message.time.time_since_epoch()) % 1000;
        char time_buffer[64];
        std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &calendar_time);
        json["time"] = std::format("{}.{:03d}", time_buffer, miliseconds.count());

        json["level"] = spdlog::level::to_string_view(message.level).data();
        json["thread"] = message.thread_id;

        std::string file_path = message.source.filename ? message.source.filename : "";
        size_t src_pos = file_path.find("src/");
        if (src_pos != std::string::npos) {
            json["file"] = file_path.substr(src_pos);
        } else {
            size_t last_slash = file_path.find_last_of("/\\");
            json["file"] = (last_slash != std::string::npos) ? file_path.substr(last_slash + 1) : file_path;
        }

        json["line"] = message.source.line;
        json["function"] = message.source.funcname ? message.source.funcname : "";

        std::string full_payload = std::string(message.payload.data(), message.payload.size());
        size_t extras_variables_position = full_payload.find(" | ");
        if (extras_variables_position != std::string::npos) {
            json["message"] = full_payload.substr(0, extras_variables_position);
            std::string extras_variables = full_payload.substr(extras_variables_position + 3);
            process_extras_variables(json["details"], extras_variables);
        } else {
            json["message"] = full_payload;
        }

        std::string dumped_json = json.dump() + "\n";
        if (current_size + dumped_json.size() > max_size_bytes) {
            rotate_logs();
        }
        file << dumped_json;
        current_size += dumped_json.size();
    }

    void rotate_logs() {
        file.close();
        size_t dot_position = filename.find_last_of(".");
        std::string backup_name = filename.substr(0, dot_position) + ".old" + filename.substr(dot_position);
        std::remove(backup_name.c_str());
        std::rename(filename.c_str(), backup_name.c_str());
        file.open(filename, std::ios::trunc);
        current_size = 0;
    }

    void process_extras_variables(nlohmann::ordered_json& extras_json, const std::string& extras_string)
    {
        std::stringstream ss(extras_string);
        std::string segment;
        while (std::getline(ss, segment, '|'))
        {
            size_t equal_position = segment.find('=');
            if (equal_position != std::string::npos)
            {
                std::string key = segment.substr(0, equal_position);
                std::string value = segment.substr(equal_position + 1);
                key.erase(0, key.find_first_not_of(" "));
                key.erase(key.find_last_not_of(" ") + 1);
                extras_json[key] = value;
            }
        }
    }

    void flush_() override { file.flush(); }
};

using imgui_sink_mt = ImguiLoggerSink<std::mutex>;
using json_file_sink_mt = JsonFileLoggerSink<std::mutex>;

export class Logger
{
public:
    static void Init()
    {
        spdlog::init_thread_pool(8192, 1);

        // auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // console_sink->set_pattern("[%Y-%m-%d %T.%e] [%^%l%$] %v");

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

    static ImguiLogger& GetGUILogger()
    {
        return imgui_sink_mt::GetImguiLogger();
    }

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
    static std::string format_extras(Args&&... args) 
    {
        try {
            constexpr size_t total_args = sizeof...(args);
            if constexpr (total_args == 0) return "";
            static_assert(total_args % 2 == 0, "Argument must be pairs: (Key, Value)");

            std::string key_value_string;
            key_value_string.reserve(128);
            auto tuple = std::forward_as_tuple(std::forward<Args>(args)...);

            [&] <std::size_t... pair_index>(std::index_sequence<pair_index...>) {
                ((key_value_string += " | " +
                    to_str(std::get<pair_index * 2>(tuple)) + "=" +
                    to_str(std::get<pair_index * 2 + 1>(tuple))
                    ), ...);
            }(std::make_index_sequence<total_args / 2>{});

            return key_value_string;
        } catch (...) {
            return " | [Log Format Error]";
        }
    }
};