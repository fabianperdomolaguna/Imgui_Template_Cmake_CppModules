module;

#include <cstdint>

#include "imgui.h"

export module ColorStyles;

constexpr auto ColorFromBytes = [](uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
{
    return ImVec4((float)red / 255.0f, (float)green / 255.0f, (float)blue / 255.0f, (float)alpha / 255.0f);
};

export void SetDarkTheme()
{
    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    const ImVec4 bg_color = ColorFromBytes(37, 37, 38);
    const ImVec4 light_bg_color = ColorFromBytes(82, 82, 85);
    const ImVec4 very_light_bg_color = ColorFromBytes(90, 90, 95);

    const ImVec4 panel_color = ColorFromBytes(65, 65, 70);
    const ImVec4 panel_hover_color = ColorFromBytes(29, 151, 236);
    const ImVec4 panel_active_color = ColorFromBytes(0, 119, 200);

    const ImVec4 text_color = ColorFromBytes(255, 255, 255);
    const ImVec4 text_disabled_color = ColorFromBytes(151, 151, 151);
    const ImVec4 border_color = ColorFromBytes(78, 78, 78);

    const ImVec4 modal_dimmed_background = ColorFromBytes(204, 204, 204, 89);

    const ImVec4 check_color = ColorFromBytes(255, 153, 0);

    colors[ImGuiCol_WindowBg] = ImVec4{ 0.18f, 0.18f, 0.18f, 1.0f };
    colors[ImGuiCol_ChildBg] = bg_color;
    colors[ImGuiCol_PopupBg] = bg_color;

    colors[ImGuiCol_Border] = border_color;
    colors[ImGuiCol_BorderShadow] = border_color;

    colors[ImGuiCol_Header] = panel_color;
    colors[ImGuiCol_HeaderHovered] = panel_hover_color;
    colors[ImGuiCol_HeaderActive] = panel_active_color;

    colors[ImGuiCol_Button] = panel_active_color;
    colors[ImGuiCol_ButtonHovered] = light_bg_color;
    colors[ImGuiCol_ButtonActive] = very_light_bg_color;

    colors[ImGuiCol_FrameBg] = panel_color;
    colors[ImGuiCol_FrameBgHovered] = panel_hover_color;
    colors[ImGuiCol_FrameBgActive] = panel_active_color;

    colors[ImGuiCol_Tab] = bg_color;
    colors[ImGuiCol_TabHovered] = panel_hover_color;
    colors[ImGuiCol_TabActive] = panel_active_color;
    colors[ImGuiCol_TabUnfocused] = bg_color;
    colors[ImGuiCol_TabUnfocusedActive] = panel_active_color;

    colors[ImGuiCol_TitleBg] = bg_color;
    colors[ImGuiCol_TitleBgActive] = panel_hover_color;
    colors[ImGuiCol_TitleBgCollapsed] = bg_color;

    colors[ImGuiCol_Text] = text_color;
    colors[ImGuiCol_TextDisabled] = text_disabled_color;
    colors[ImGuiCol_TextSelectedBg] = panel_active_color;

    colors[ImGuiCol_MenuBarBg] = panel_color;

    colors[ImGuiCol_ScrollbarBg] = panel_color;
    colors[ImGuiCol_ScrollbarGrab] = very_light_bg_color;
    colors[ImGuiCol_ScrollbarGrabHovered] = panel_hover_color;
    colors[ImGuiCol_ScrollbarGrabActive] = panel_active_color;

    colors[ImGuiCol_CheckMark] = check_color;

    colors[ImGuiCol_SliderGrab] = panel_hover_color;
    colors[ImGuiCol_SliderGrabActive] = panel_active_color;

    colors[ImGuiCol_Separator] = border_color;
    colors[ImGuiCol_SeparatorHovered] = border_color;
    colors[ImGuiCol_SeparatorActive] = border_color;

    colors[ImGuiCol_ResizeGrip] = bg_color;
    colors[ImGuiCol_ResizeGripHovered] = panel_color;
    colors[ImGuiCol_ResizeGripActive] = light_bg_color;

    colors[ImGuiCol_PlotLines] = panel_active_color;
    colors[ImGuiCol_PlotLinesHovered] = panel_hover_color;
    colors[ImGuiCol_PlotHistogram] = panel_active_color;
    colors[ImGuiCol_PlotHistogramHovered] = panel_hover_color;

    colors[ImGuiCol_DragDropTarget] = bg_color;
    colors[ImGuiCol_NavHighlight] = bg_color;

    colors[ImGuiCol_ModalWindowDimBg] = modal_dimmed_background;
    
    colors[ImGuiCol_DockingPreview] = panel_active_color;

    style.WindowPadding = ImVec2(2, 2);
}

export void SetLightTheme()
{
    ImGui::StyleColorsLight();

    auto& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    const ImVec4 bg_color = ColorFromBytes(230, 230, 230);
    const ImVec4 light_bg_color = ColorFromBytes(82, 82, 85);
    const ImVec4 very_light_bg_color = ColorFromBytes(90, 90, 95);

    const ImVec4 panel_color = ColorFromBytes(166, 199, 214);
    const ImVec4 panel_hover_color = ColorFromBytes(191, 224, 240);
    const ImVec4 panel_active_color = ColorFromBytes(140, 173, 189);

    const ImVec4 text_color = ColorFromBytes(79, 64, 61);
    const ImVec4 text_disabled_color = ColorFromBytes(150, 150, 150);
    const ImVec4 border_color = ColorFromBytes(190, 190, 190);

    const ImVec4 title_bg_color = ColorFromBytes(107, 191, 255);
    const ImVec4 title_collapsed_color = ColorFromBytes(102, 166, 204);

    const ImVec4 button_color = ColorFromBytes(253, 218, 44);
    const ImVec4 button_hovered = ColorFromBytes(107, 209, 255);
    const ImVec4 button_active = ColorFromBytes(184, 255, 255);

    const ImVec4 menu_bg_color = ColorFromBytes(189, 189, 240);

    const ImVec4 check_color = ColorFromBytes(111, 65, 65);

    const ImVec4 modal_dimmed_background = ColorFromBytes(204, 204, 204, 89);

    colors[ImGuiCol_WindowBg] = ImVec4{ 0.95f, 0.94f, 0.92f, 1.0f };
    colors[ImGuiCol_ChildBg] = bg_color;
    colors[ImGuiCol_PopupBg] = bg_color;

    colors[ImGuiCol_Border] = border_color;
    colors[ImGuiCol_BorderShadow] = border_color;

    colors[ImGuiCol_Header] = panel_color;
    colors[ImGuiCol_HeaderHovered] = panel_hover_color;
    colors[ImGuiCol_HeaderActive] = panel_active_color;

    colors[ImGuiCol_Button] = button_color;
    colors[ImGuiCol_ButtonHovered] = button_hovered;
    colors[ImGuiCol_ButtonActive] = button_active;

    colors[ImGuiCol_FrameBg] = panel_color;
    colors[ImGuiCol_FrameBgHovered] = panel_hover_color;
    colors[ImGuiCol_FrameBgActive] = panel_active_color;

    colors[ImGuiCol_Tab] = bg_color;
    colors[ImGuiCol_TabHovered] = panel_hover_color;
    colors[ImGuiCol_TabActive] = panel_active_color;
    colors[ImGuiCol_TabUnfocused] = bg_color;
    colors[ImGuiCol_TabUnfocusedActive] = panel_active_color;

    colors[ImGuiCol_TitleBg] = panel_hover_color;
    colors[ImGuiCol_TitleBgActive] = title_bg_color;
    colors[ImGuiCol_TitleBgCollapsed] = title_collapsed_color;

    colors[ImGuiCol_Text] = text_color;
    colors[ImGuiCol_TextDisabled] = text_disabled_color;
    colors[ImGuiCol_TextSelectedBg] = panel_active_color;

    colors[ImGuiCol_MenuBarBg] = menu_bg_color;

    colors[ImGuiCol_ScrollbarBg] = panel_color;
    colors[ImGuiCol_ScrollbarGrab] = light_bg_color;
    colors[ImGuiCol_ScrollbarGrabHovered] = light_bg_color;
    colors[ImGuiCol_ScrollbarGrabActive] = very_light_bg_color;

    colors[ImGuiCol_CheckMark] = check_color;

    colors[ImGuiCol_SliderGrab] = button_color;
    colors[ImGuiCol_SliderGrabActive] = menu_bg_color;

    colors[ImGuiCol_Separator] = border_color;
    colors[ImGuiCol_SeparatorHovered] = border_color;
    colors[ImGuiCol_SeparatorActive] = border_color;

	colors[ImGuiCol_ModalWindowDimBg] = modal_dimmed_background;

    colors[ImGuiCol_DockingPreview] = ImVec4(0.0f, 0.47f, 0.78f, 1.0f);

    style.WindowPadding = ImVec2(2, 2);
}