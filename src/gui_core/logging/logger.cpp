module;

#include <string>
#include <deque>
#include <mutex>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/base_sink.h"

export module Logger;

export struct LogEntry {
    std::string message;
    spdlog::level::level_enum level;
};

export struct ImguiLogger {
    std::deque <LogEntry> items;
	std::mutex mutex;
    uint8_t max_size = 100;

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

using imgui_sink_mt = ImguiLoggerSink<std::mutex>;

export class Logger
{
public:
    static void Init()
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