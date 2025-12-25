#include "imgui.h"
#include "spdlog/spdlog.h"

#include "logging_console.h"
#include "logging/logger.h"

void LoggingConsole(bool* open)
{
    if (open && !*open) return;

    bool is_visible = ImGui::Begin("Logging Console", open);
    if (!is_visible)
    {
        ImGui::End();
        return;
    }

    if (ImGui::Button("Clear"))
        Logger::GetGUILogger().Clear();
    ImGui::Separator();

    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
    {
        auto& logging_data = Logger::GetGUILogger();
        std::lock_guard<std::mutex> lock(logging_data.mutex);

        bool is_at_bottom = (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 5.0f);

        ImGuiListClipper clipper;
        clipper.Begin((int)logging_data.items.size());
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
            {
                const auto& entry = logging_data.items[i];
                const std::string& full_text = entry.message;

                size_t first_bracket = full_text.find(']');
                size_t second_bracket = full_text.find(']', first_bracket + 1);
        
                std::string header = full_text.substr(0, second_bracket + 1);
                std::string message = full_text.substr(second_bracket + 1); 
        
                ImVec4 color;
                if (entry.level == spdlog::level::err || entry.level == spdlog::level::critical)
                    color = ImVec4(0.8f, 0.04f, 0.04f, 1.0f);
                else if (entry.level == spdlog::level::warn)
                    color = ImVec4(0.77f, 0.64f, 0.05f, 1.0f);
                else if (entry.level == spdlog::level::info || entry.level == spdlog::level::trace)
                    color = ImVec4(0.08f, 0.62f, 0.62f, 1.0f);
            
                ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(header.c_str());
                ImGui::PopStyleColor();
                ImGui::SameLine();
                ImGui::TextUnformatted(message.c_str());
            }
        }
    

        if (is_at_bottom)
            ImGui::SetScrollHereY(1.0f);
    
        ImGui::EndChild();
        ImGui::End();
    }
}