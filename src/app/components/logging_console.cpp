#include "app/components/logging_console.h"

#include <array>
#include <mutex>
#include <string>
#include <vector>

#include "beryl/logging/logger.h"

constexpr ImVec4 app::component::GetLogColor(spdlog::level::level_enum level) noexcept
{
    switch (level)
    {
        case spdlog::level::trace:
        case spdlog::level::info:     return { 0.08f, 0.62f, 0.62f, 1.0f };
        case spdlog::level::warn:     return { 0.77f, 0.64f, 0.05f, 1.0f };
        case spdlog::level::err:      
        case spdlog::level::critical: return { 0.8f, 0.04f, 0.04f, 1.0f };
        default:                      return { 0.0f, 0.0f, 0.0f, 1.0f };
    }
}

void app::component::LoggingConsole(bool* open)
{
    if (open != nullptr && !*open) return;

    if (!ImGui::Begin("Logging Console", open))
    {
        ImGui::End();
        return;
    }

    auto& logging_data = beryl::logger::GetGUILogger();

    static int selected_level_idx = 0;
    const std::array<const char*, 6> levels = { "All", "Trace", "Info", "Warn", "Error", "Critical" };
    const std::array<spdlog::level::level_enum, 6> level_enums = {
        spdlog::level::off,
        spdlog::level::trace,
        spdlog::level::info,
        spdlog::level::warn,
        spdlog::level::err,
        spdlog::level::critical
    };

    if (ImGui::Button("Clear"))
        logging_data.Clear();

    ImGui::SameLine();
    ImGui::SetNextItemWidth(120.0f);
    bool level_filter_changed = ImGui::Combo(
        "Level", &selected_level_idx, levels.data(), static_cast<int>(levels.size()));

    ImGui::Separator();

    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), ImGuiChildFlags_None,
        ImGuiWindowFlags_HorizontalScrollbar))
    {
        std::scoped_lock lock(logging_data.mutex);

        const auto& items = logging_data.items;

        static std::vector<LineEntry> filtered_lines;
        static size_t last_items_size = 0;
        static const std::string* last_msg_ptr = nullptr;

        const bool buffer_rotated = !items.empty() && (&items.back().message != last_msg_ptr);

        if (level_filter_changed || items.size() != last_items_size || buffer_rotated)
        {
            filtered_lines.clear();
            filtered_lines.reserve(items.size() * 2);

            const auto target_level = level_enums.at(static_cast<size_t>(selected_level_idx));

            for (size_t i = 0; i < items.size(); ++i)
            {
                const auto& entry = items[i];

                if (selected_level_idx != 0 && entry.level != target_level)
                    continue;

                const std::string_view full_text = entry.message;

                size_t bracket_pos = full_text.find(']');
                if (bracket_pos != std::string_view::npos) 
                    bracket_pos = full_text.find(']', bracket_pos + 1);
                
                if (bracket_pos != std::string_view::npos)
                {
                    std::string_view header = full_text.substr(0, bracket_pos + 1);
                    std::string_view body = full_text.substr(bracket_pos + 1);

                    size_t extras_pos = body.find(" | ");

                    if (extras_pos != std::string_view::npos)
                    {
                        std::string_view main_message = body.substr(0, extras_pos);

                        filtered_lines.push_back({static_cast<int>(i), 0, header, main_message,
                            entry.level});

                        const std::string_view extras_str = body.substr(extras_pos + 3);

                        size_t start = 0;
                        size_t end = extras_str.find('|');

                        while (true)
                        {
                            std::string_view pair = extras_str.substr(start,
                                (end == std::string_view::npos) ? std::string_view::npos
                                                                : end - start);

                            const size_t first = pair.find_first_not_of(' ');
                            if (first != std::string_view::npos)
                            {
                                const size_t last = pair.find_last_not_of(' ');
                                const std::string_view trimmed =
                                    pair.substr(first, (last - first + 1));

                                filtered_lines.push_back({static_cast<int>(i), 1, {}, trimmed,
                                    entry.level});
                            }

                            if (end == std::string_view::npos) break;
                            start = end + 1;
                            end = extras_str.find('|', start);
                        }
                    }
                    else
                    {
                        filtered_lines.push_back({static_cast<int>(i), 0, header, body,
                            entry.level});
                    }
                }
                else
                {
                    filtered_lines.push_back({static_cast<int>(i), 0, {}, full_text,
                        entry.level});
                }
            }

            last_items_size = items.size();
            last_msg_ptr = items.empty() ? nullptr : &items.back().message;
        }

        const int line_count = static_cast<int>(filtered_lines.size());
        bool is_at_bottom = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f);

        ImGuiListClipper clipper;
        clipper.Begin(line_count);

        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                if (i < 0 || i >= line_count) continue;

                const auto& line = filtered_lines[i];

                if (line.line_type == 0)
                {
                    if (!line.header.empty())
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, GetLogColor(line.level));
                        ImGui::TextUnformatted(
                            line.header.data(), line.header.data() + line.header.size());
                        ImGui::PopStyleColor();

                        if (!line.body.empty())
                        {
                            ImGui::SameLine();
                            ImGui::TextUnformatted(
                                line.body.data(), line.body.data() + line.body.size());
                        }
                    }
                    else
                    {
                        ImGui::TextUnformatted(
                            line.body.data(), line.body.data() + line.body.size());
                    }
                }
                else
                {
                    ImGui::Text("  * %.*s", static_cast<int>(line.body.size()), line.body.data());
                }
            }
        }

        if (is_at_bottom)
            ImGui::SetScrollHereY(1.0f);
    }

    ImGui::EndChild();
    ImGui::End();
}