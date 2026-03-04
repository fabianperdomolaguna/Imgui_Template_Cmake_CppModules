module;

#include <cstdint>
#include <span>

#include "imgui.h"

export module beryl.gui:themes;

struct ThemePalette
{
    ImVec4 window_background;
    ImVec4 background;
    ImVec4 light_background;
    ImVec4 very_light_background;
    ImVec4 panel;
    ImVec4 panel_hover;
    ImVec4 panel_active;
    ImVec4 button;
    ImVec4 button_hovered;
    ImVec4 button_active;
    ImVec4 title_background;
    ImVec4 title_background_active;
    ImVec4 title_background_collapsed;
    ImVec4 text;
    ImVec4 text_disabled;
    ImVec4 menu_background;
    ImVec4 border;
    ImVec4 check;
    ImVec4 slider_grab;
    ImVec4 slider_grab_active;
    ImVec4 scrollbar_background;
    ImVec4 scrollbar_grab;
    ImVec4 scrollbar_grab_hovered;
    ImVec4 scrollbar_grab_active;
    ImVec4 resize_grip;
    ImVec4 resize_grip_hovered;
    ImVec4 resize_grip_active;
    ImVec4 drag_drop_target;
    ImVec4 modal_dimmed_background;
    ImVec4 docking_preview;
};

constexpr ImVec4 ColorFromBytes(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
{
    return 
    {
        static_cast<float>(red) / 255.0f, 
        static_cast<float>(green) / 255.0f,
        static_cast<float>(blue) / 255.0f, 
        static_cast<float>(alpha) / 255.0f
    };
}

void ApplyPalette(const ThemePalette& palette) 
{
    auto& style = ImGui::GetStyle();
    std::span<ImVec4, ImGuiCol_COUNT> colors{ style.Colors };

    colors[ImGuiCol_WindowBg] = palette.window_background;
    colors[ImGuiCol_ChildBg] = palette.background;
    colors[ImGuiCol_PopupBg] = palette.background;

    colors[ImGuiCol_Border] = palette.border;
    colors[ImGuiCol_BorderShadow] = palette.border;

    colors[ImGuiCol_Header] = palette.panel;
    colors[ImGuiCol_HeaderHovered] = palette.panel_hover;
    colors[ImGuiCol_HeaderActive] = palette.panel_active;

    colors[ImGuiCol_Button] = palette.button;
    colors[ImGuiCol_ButtonHovered] = palette.button_hovered;
    colors[ImGuiCol_ButtonActive] = palette.button_active;

    colors[ImGuiCol_FrameBg] = palette.panel;
    colors[ImGuiCol_FrameBgHovered] = palette.panel_hover;
    colors[ImGuiCol_FrameBgActive] = palette.panel_active;

    colors[ImGuiCol_Tab] = palette.background;
    colors[ImGuiCol_TabHovered] = palette.panel_hover;
    colors[ImGuiCol_TabActive] = palette.panel_active;
    colors[ImGuiCol_TabUnfocused] = palette.background;
    colors[ImGuiCol_TabUnfocusedActive] = palette.panel_active;

    colors[ImGuiCol_ScrollbarBg] = palette.scrollbar_background;
    colors[ImGuiCol_ScrollbarGrab] = palette.scrollbar_grab;
    colors[ImGuiCol_ScrollbarGrabHovered] = palette.scrollbar_grab_hovered;
    colors[ImGuiCol_ScrollbarGrabActive] = palette.scrollbar_grab_active;

    colors[ImGuiCol_Text] = palette.text;
    colors[ImGuiCol_TextDisabled] = palette.text_disabled;
    colors[ImGuiCol_TextSelectedBg] = palette.panel_active;

    colors[ImGuiCol_MenuBarBg] = palette.menu_background;

    colors[ImGuiCol_TitleBg] = palette.title_background;
    colors[ImGuiCol_TitleBgActive] = palette.title_background_active;
    colors[ImGuiCol_TitleBgCollapsed] = palette.title_background_collapsed;

    colors[ImGuiCol_CheckMark] = palette.check;
    colors[ImGuiCol_SliderGrab] = palette.slider_grab;
    colors[ImGuiCol_SliderGrabActive] = palette.slider_grab_active;

    colors[ImGuiCol_Separator] = palette.border;
    colors[ImGuiCol_SeparatorHovered] = palette.border;
    colors[ImGuiCol_SeparatorActive] = palette.border;

    colors[ImGuiCol_ResizeGrip] = palette.resize_grip;
    colors[ImGuiCol_ResizeGripHovered] = palette.resize_grip_hovered;
    colors[ImGuiCol_ResizeGripActive] = palette.resize_grip_active;

    colors[ImGuiCol_DragDropTarget] = palette.drag_drop_target;
    colors[ImGuiCol_ModalWindowDimBg] = palette.modal_dimmed_background;
    colors[ImGuiCol_DockingPreview] = palette.docking_preview;

    style.WindowPadding = ImVec2(4, 4);
    style.FramePadding  = ImVec2(6, 4);
    style.ItemSpacing   = ImVec2(6, 4);
    style.WindowRounding = 4.0f;
    style.FrameRounding  = 2.0f;
}

namespace beryl::themes
{
    void SetLightTheme() 
    {
        static constexpr ThemePalette light = 
        {
            .window_background = ColorFromBytes(242, 240, 235),
            .background = ColorFromBytes(230, 230, 230),
            .light_background = ColorFromBytes(82, 82, 85),
            .very_light_background = ColorFromBytes(90, 90, 95),
            .panel = ColorFromBytes(166, 199, 214),
            .panel_hover = ColorFromBytes(191, 224, 240),
            .panel_active = ColorFromBytes(140, 173, 189),
            .button = ColorFromBytes(253, 218, 44),
            .button_hovered = ColorFromBytes(107, 209, 255),
            .button_active = ColorFromBytes(184, 255, 255),
            .title_background = ColorFromBytes(191, 224, 240),
            .title_background_active = ColorFromBytes(107, 191, 255),
            .title_background_collapsed = ColorFromBytes(102, 166, 204),
            .text = ColorFromBytes(79, 64, 61),
            .text_disabled = ColorFromBytes(150, 150, 150),
            .menu_background = ColorFromBytes(189, 189, 240),
            .border = ColorFromBytes(190, 190, 190),
            .check = ColorFromBytes(111, 65, 65),
            .slider_grab = ColorFromBytes(253, 218, 44),
            .slider_grab_active = ColorFromBytes(189, 189, 240),
            .scrollbar_background = ColorFromBytes(166, 199, 214),
            .scrollbar_grab = ColorFromBytes(82, 82, 85),
            .scrollbar_grab_hovered = ColorFromBytes(120, 120, 124),
            .scrollbar_grab_active = ColorFromBytes(90, 90, 95),
            .resize_grip = ColorFromBytes(190, 190, 190),
            .resize_grip_hovered = ColorFromBytes(191, 224, 240),
            .resize_grip_active = ColorFromBytes(166, 199, 214),
            .drag_drop_target = ColorFromBytes(66, 150, 250, 242),
            .modal_dimmed_background = ColorFromBytes(204, 204, 204, 89),
            .docking_preview = ColorFromBytes(0, 120, 199),
        };
        ImGui::StyleColorsLight();
        ApplyPalette(light);
    }

    void SetDarkTheme() 
    {
        static constexpr ThemePalette dark = 
        {
            .window_background = ColorFromBytes(46, 46, 46),
            .background = ColorFromBytes(37, 37, 38),
            .light_background = ColorFromBytes(82, 82, 85),
            .very_light_background = ColorFromBytes(90, 90, 95),
            .panel = ColorFromBytes(65, 65, 70),
            .panel_hover = ColorFromBytes(29, 151, 236),
            .panel_active = ColorFromBytes(0, 119, 200),
            .button = ColorFromBytes(65, 65, 70),
            .button_hovered = ColorFromBytes(82, 82, 85),
            .button_active = ColorFromBytes(90, 90, 95),
            .title_background = ColorFromBytes(37, 37, 38),
            .title_background_active = ColorFromBytes(29, 151, 236),
            .title_background_collapsed = ColorFromBytes(37, 37, 38),
            .text = ColorFromBytes(255, 255, 255),
            .text_disabled = ColorFromBytes(151, 151, 151),
            .menu_background = ColorFromBytes(65, 65, 70),
            .border = ColorFromBytes(78, 78, 78),
            .check = ColorFromBytes(255, 153, 0),
            .slider_grab = ColorFromBytes(29, 151, 236),
            .slider_grab_active = ColorFromBytes(0, 119, 200),
            .scrollbar_background = ColorFromBytes(65, 65, 70),
            .scrollbar_grab = ColorFromBytes(90, 90, 95),
            .scrollbar_grab_hovered = ColorFromBytes(29, 151, 236),
            .scrollbar_grab_active = ColorFromBytes(0, 119, 200),
            .resize_grip = ColorFromBytes(78, 78, 78),
            .resize_grip_hovered = ColorFromBytes(151, 151, 151),
            .resize_grip_active = ColorFromBytes(231, 231, 231),
            .drag_drop_target = ColorFromBytes(37, 37, 38),
            .modal_dimmed_background = ColorFromBytes(204, 204, 204, 89),
            .docking_preview = ColorFromBytes(0, 119, 200),
        };
        ImGui::StyleColorsDark();
        ApplyPalette(dark);
    }

}