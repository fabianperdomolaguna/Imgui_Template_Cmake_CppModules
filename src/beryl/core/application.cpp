#include "beryl/core/application.h"

#include "beryl/utils/path_utils.h"
#include "beryl/renderer/texture.h"

beryl::core::Application::Application(const WindowSpecification& spec)
{
    m_executable_path = beryl::utils::path::GetExecutablePath();

	m_window = std::make_unique<beryl::core::Window>(spec);
	m_gui_context = std::make_unique<beryl::gui::Context>(m_window->m_glfw_window);
	m_gui_context->UpdateDockspaceFlags(m_window->m_window_specification.custom_title_bar);
}

void beryl::core::Application::Run()
{
	while (m_window->m_running)
	{
		m_window->PreRender();
		m_gui_context->PreRender();

		m_window->CloseAppPopup();

		for (auto& layer_render : m_layer_stack)
			layer_render->OnRender();

		m_gui_context->PostRender();
		m_window->PostRender();
	}
}

void beryl::core::Application::SetWindowIcon(const std::string& icon_path) const
{
	beryl::renderer::Texture2D image(icon_path, GL_RGBA, false);
			
	if (image.m_data == nullptr) 
		return;

	GLFWimage icon;
	icon.width = image.m_width;
	icon.height = image.m_height;
	icon.pixels = image.m_data;

	glfwSetWindowIcon(m_window->m_glfw_window, 1, &icon);
}

void beryl::core::Application::SetWindowIcon(const uint8_t* image_data, uint32_t image_size) const
{
	beryl::renderer::Texture2D image(image_data, image_size, GL_RGBA, false);

	if (image.m_data == nullptr)
		return;

	GLFWimage icon;
	icon.width = image.m_width;
	icon.height = image.m_height;
	icon.pixels = image.m_data;

	glfwSetWindowIcon(m_window->m_glfw_window, 1, &icon);
}