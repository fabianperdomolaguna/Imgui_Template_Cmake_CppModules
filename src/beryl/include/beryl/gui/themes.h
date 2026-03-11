#pragma once

#include <cstdint>

#include "imgui.h"

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

constexpr ImVec4 ColorFromBytes(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255);
void ApplyPalette(const ThemePalette& palette);

namespace beryl::themes
{
    void SetLightTheme();
    void SetDarkTheme();
}