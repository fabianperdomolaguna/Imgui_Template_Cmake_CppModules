#include <string>
#include <memory>

#include "core/window.h"
#include "core/imgui_context.h"
#include "core/layer.h"
#include "core/application.h"
#include "image/image_reader.h"
#include "path/path_utilities.h"

Application::Application(const AppSpecification& spec) : m_app_specification(spec)
{
    m_executable_path = GetExecutablePath().string();

	m_window = std::make_unique<Window>(
		m_app_specification.title,
		m_app_specification.width,
		m_app_specification.height,
		m_app_specification.custom_title_bar
	);
	m_imgui_context = std::make_unique<ImguiContext>(m_window->m_window, m_executable_path);
}

Application::~Application() {};

void Application::Run()
{
	while (m_window->m_running)
	{
		m_window->PreRender();
		m_imgui_context->PreRender(m_app_specification.custom_title_bar);

		if (m_window->m_close_popup)
			m_window->CloseAppPopup();
         
		for (auto& layer_render : m_layer_stack)
			layer_render->OnRender();

		m_imgui_context->PostRender();
		m_window->PostRender();
	}
}

void Application::SetWindowIcon(std::string icon_path)
{
	ImageTexture image = ImageTexture(icon_path, GL_RGBA);
    
    GLFWimage icon;
	icon.width = image.m_width;
	icon.height = image.m_height;
	icon.pixels = image.m_data;

	glfwSetWindowIcon(m_window->m_window, 1, &icon);
}

void Application::SetWindowIcon(uint8_t* image_data, uint32_t image_size)
{
	ImageTexture image = ImageTexture(image_data, image_size, GL_RGBA);

	GLFWimage icon;
	icon.width = image.m_width;
	icon.height = image.m_height;
	icon.pixels = image.m_data;

	glfwSetWindowIcon(m_window->m_window, 1, &icon);
}