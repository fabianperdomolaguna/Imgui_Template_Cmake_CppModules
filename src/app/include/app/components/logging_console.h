#pragma once

#include <string_view>

#include "imgui.h"
#include "spdlog/spdlog.h"

namespace app::component
{
    [[nodiscard]] constexpr ImVec4 GetLogColor(spdlog::level::level_enum level) noexcept;

    struct LineEntry
    {
        int log_index;
        int line_type; // 0 = main line, 1 = extra
        std::string_view header;
        std::string_view body;
        spdlog::level::level_enum level;
    };

    void LoggingConsole(bool* open);
}