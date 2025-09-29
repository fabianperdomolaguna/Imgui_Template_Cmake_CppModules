#include <string>
#include <memory>
#include <functional>

#include "GLFW/glfw3.h"
#include "imgui.h"
#include "imgui_internal.h"

#include "icon_titlebar.embed"

#include "title_bar.h"
#include "viewport_bar.h"

void DrawWindowButtons(
    ImVec2& button_start, 
    ImVec2& button_end, 
    ImTextureID icon_texture,
	const float& button_size,
	const float& icon_size,
    ImU32 button_color,
    const std::function<void()>& on_click,
    bool round_top_right,
    const float& round_corner_radius)
{
    ImRect button_rect(button_start, button_end);
    bool button_hovered = button_rect.Contains(ImGui::GetMousePos());
    if (button_hovered)
    {
        if (round_top_right)
        {
            ImGui::GetBackgroundDrawList()->AddRectFilled(
                button_start, button_end, button_color,
                round_corner_radius, ImDrawFlags_RoundCornersTopRight);
        }
        else 
        {
            ImGui::GetBackgroundDrawList()->AddRectFilled(button_start, button_end, button_color);
        }
    }
    if (button_hovered && ImGui::IsMouseClicked(0))
		on_click();

    ImVec2 center = button_rect.GetCenter();
    ImVec2 icon_start = ImVec2(floor(center.x - icon_size * 0.5f), floor(center.y - icon_size * 0.5f));
    ImVec2 icon_end = ImVec2(floor(icon_start.x + icon_size), floor(icon_start.y + icon_size));
    ImGui::GetBackgroundDrawList()->AddImage(
        icon_texture,
        icon_start,
        icon_end
    );

	button_start = ImVec2(button_start.x + button_size, button_start.y);
    button_end = ImVec2(button_end.x + button_size, button_end.y);
}

bool IsMaximized(GLFWwindow* window)
{
    return (bool)glfwGetWindowAttrib(window, GLFW_MAXIMIZED);
}

TitleBar::TitleBar(Application* app) : m_app(app) 
{
    m_app->m_imgui_context->header_height.push_back(title_bar_height);
	color_style = m_app->m_imgui_context->color_style;
    UpdateTitleBarColor();

    icon_titlebar = std::make_unique<ImageTexture>(g_icon_titlebar, g_icon_titlebar_len, GL_RGBA, true);
    LoadButtonTextures(color_style, false);
}

void TitleBar::MenuBar()
{
    float menu_bar_size = 0.0f;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, IM_COL32(0, 0, 0, 0));
    if (BeginViewportBar("##TitleBarMenuBar", ImGui::GetMainViewport(),
		title_bar_height + 15.0f, title_bar_height * 0.5f - ImGui::GetFrameHeight() * 0.5f, 
		menu_bar_size, ImGui::GetFrameHeight(),
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar))
    {
        ImVec2 menu_start = ImGui::GetCursorScreenPos();

        ImGui::BeginMenuBar();
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
				m_app->m_window->m_close_popup = true;

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("Copy", "Ctrl + C");

			ImGui::EndMenu();
		}

        ImVec2 menu_end = ImGui::GetCursorScreenPos();
        menu_bar_size = menu_end.x - menu_start.x;

		ImGui::EndMenuBar();

		ImGui::End();
    }
    ImGui::PopStyleColor(2);
}

void TitleBar::HandleBorderResize()
{
    //Get cursor position and window size
    double mouse_x, mouse_y;
    glfwGetCursorPos(m_app->m_window->m_glfw_window, &mouse_x, &mouse_y);

    int window_width, window_height;
    glfwGetWindowSize(m_app->m_window->m_glfw_window, &window_width, &window_height);

    //Border conditions
    bool left   = (mouse_x < border_size) && (mouse_y > title_bar_height);
    bool right  = (mouse_x > window_width - border_size) && (mouse_y > title_bar_height);
    bool top    = false;
    bool bottom = mouse_y > window_height - border_size;

    //Change cursor symbol
    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_Arrow) 
    {
        if (right && bottom)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNWSE);
        else if (left && bottom)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNESW);
        else if (left)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        else if (right)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
        else if (bottom)
            ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    }

    //Resize with mouse
    if (glfwGetMouseButton(m_app->m_window->m_glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) 
    {
        int window_x, window_y;
        glfwGetWindowPos(m_app->m_window->m_glfw_window, &window_x, &window_y);

        if (right) 
            glfwSetWindowSize(m_app->m_window->m_glfw_window, (int)mouse_x, window_height);
        if (bottom) 
            glfwSetWindowSize(m_app->m_window->m_glfw_window, window_width, (int)mouse_y);
        if (left) 
        {
            int new_width = window_width - (int)mouse_x;
            int new_x = window_x + (int)mouse_x;
            glfwSetWindowPos(m_app->m_window->m_glfw_window, new_x, window_y);
            glfwSetWindowSize(m_app->m_window->m_glfw_window, new_width, window_height);
        }
    }
}

void TitleBar::OnRender()
{
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
    ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
    ImGui::Begin("OverlayWindow", nullptr,
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoBackground);

    ImGui::GetWindowDrawList()->PushClipRectFullScreen();

    ImGui::GetBackgroundDrawList()->AddRectFilled(
        ImVec2(ImGui::GetMainViewport()->Pos.x, ImGui::GetMainViewport()->Pos.y + 42.0f),
        ImVec2(ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x,
            ImGui::GetMainViewport()->Pos.y + ImGui::GetMainViewport()->Size.y),
        IM_COL32(51, 51, 51, 255)
    );

    if (color_style != m_app->m_imgui_context->color_style)
    {
        color_style = m_app->m_imgui_context->color_style;
        UpdateTitleBarColor();
        LoadButtonTextures(color_style, true);
	}

    ImVec2 top_left = ImGui::GetMainViewport()->Pos;
    ImVec2 bottom_right = ImVec2(top_left.x + ImGui::GetMainViewport()->Size.x, top_left.y + 42.0f);
    ImRect titlebar_rect(top_left, bottom_right);
    DrawCenteredText(m_app->m_window->m_window_specification.title, titlebar_rect);

    ImGui::GetBackgroundDrawList()->AddRectFilled(
        top_left,
        bottom_right,
        titlebar_background_color,
        IsMaximized(m_app->m_window->m_glfw_window) ? 0.0f : round_corner_radius,
        ImDrawFlags_RoundCornersTop
    );

    ImGui::InvisibleButton("##TitleBarDragZone", ImVec2(ImGui::GetMainViewport()->Size.x, 42.0f));
    if (ImGui::IsMouseHoveringRect(top_left, bottom_right) && ImGui::IsMouseDragging(0))
    {
        int wx, wy;
        glfwGetWindowPos(m_app->m_window->m_glfw_window, &wx, &wy);
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        glfwSetWindowPos(m_app->m_window->m_glfw_window, wx + (int)delta.x, wy + (int)delta.y);
    }
    ImGui::End();

    HandleBorderResize();

    MenuBar();

	float logo_size = title_bar_height * 0.8;
    const ImVec2 logo_offset(title_bar_height * 0.3, title_bar_height * 0.1);
    ImGui::GetBackgroundDrawList()->AddImage(
        (ImTextureID)(intptr_t)icon_titlebar->get_texture(),
        ImVec2(top_left.x + logo_offset.x, top_left.y + logo_offset.y),
        ImVec2(top_left.x + logo_offset.x + logo_size, top_left.y + logo_offset.y + logo_size)
	);

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 button_start = ImVec2(viewport->Pos.x + viewport->Size.x - button_size * 3, viewport->Pos.y);
	ImVec2 button_end = ImVec2(button_start.x + button_size, button_start.y + title_bar_height);

    DrawWindowButtons(
        button_start,
        button_end,
        (ImTextureID)(intptr_t)minimize_button->get_texture(),
        button_size,
        icon_size,
        IM_COL32(154, 140, 152, 128),
        [&]() { glfwIconifyWindow(m_app->m_window->m_glfw_window); }
    );

    DrawWindowButtons(
        button_start,
        button_end,
        (ImTextureID)(intptr_t)(
            IsMaximized(m_app->m_window->m_glfw_window) ? restore_button->get_texture()
            : maximize_button->get_texture()),
        button_size,
        icon_size,
        IM_COL32(154, 140, 152, 128),
        [&]() {
            if (IsMaximized(m_app->m_window->m_glfw_window))
                glfwRestoreWindow(m_app->m_window->m_glfw_window);
            else
                glfwMaximizeWindow(m_app->m_window->m_glfw_window);
        }
    );

    DrawWindowButtons(
        button_start,
        button_end,
        (ImTextureID)(intptr_t)close_button->get_texture(),
        button_size,
        icon_size,
        IM_COL32(255, 0, 0, 128),
        [&]() { m_app->m_window->m_close_popup = true; },
        !IsMaximized(m_app->m_window->m_glfw_window),
        round_corner_radius
    );
}

void TitleBar::UpdateTitleBarColor()
{
    titlebar_background_color =
        (m_app->m_imgui_context->color_style == "Light")
        ? IM_COL32(222, 220, 215, 255)
        : IM_COL32(40, 40, 40, 255);
}

void TitleBar::LoadButtonTextures(const std::string& style, bool reload)
{
    auto suffix = "_" + style;

    auto load_or_reload = [&](std::unique_ptr<ImageTexture>& texture, const std::string& key) {
        const auto& icon = button_map[key + suffix];
        if (reload)
            texture->Reload(icon.data, icon.len);
        else
            texture = std::make_unique<ImageTexture>(icon.data, icon.len, GL_RGBA, true);
    };

    load_or_reload(minimize_button, "minimize");
    load_or_reload(maximize_button, "maximize");
    load_or_reload(close_button, "close");
    load_or_reload(restore_button, "restore");
}

void TitleBar::DrawCenteredText(std::string text, const ImRect& rect)
{
    ImGuiIO& io = ImGui::GetIO();
    auto boldFont = io.Fonts->Fonts[2];
    ImGui::PushFont(io.Fonts->Fonts[2]);

    const char* text_cstr = text.c_str();
    ImVec2 text_size = ImGui::CalcTextSize(text_cstr);

    ImVec2 center = rect.GetCenter();
    ImVec2 text_pos(
        floor(center.x - text_size.x * 0.5f),
        floor(center.y - text_size.y * 0.5f)
    );
    ImU32 text_color = ImGui::ColorConvertFloat4ToU32(ImGui::GetStyleColorVec4(ImGuiCol_Text));
    ImGui::GetWindowDrawList()->AddText(text_pos, text_color, text_cstr);
    
    ImGui::PopFont();
}

std::string TitleBar::GetName() const
{
    return "Title_Bar";
}
