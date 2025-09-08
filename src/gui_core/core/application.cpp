module;

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "GLFW/glfw3.h"
#include "stb_image.h"

#include "path_utilities.h"

export module Application;

import Window;
import ImguiContext;
import Layer;
import Image;

struct AppSpecification
{
	std::string title;
	int32_t width;
	int32_t height;
	bool custom_title_bar;
};

export class Application
{
    
public:
	AppSpecification m_app_specification;
    std::unique_ptr<Window> m_window;
	std::unique_ptr<ImguiContext> m_imgui_context;
	std::vector<std::shared_ptr<Layer>> m_layer_stack;
    
	std::string m_executable_path;

	Application(const AppSpecification& spec) : m_app_specification(spec)
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

	~Application() {};

	void Run()
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

	template<typename T, typename... Args>
	void PushLayer(Args&&... args)
	{
		static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
		m_layer_stack.emplace_back(std::make_shared<T>(std::forward<Args>(args)...))->OnAttach();
	}

	template<typename T>
	void PushLayerApp()
	{
		static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
		m_layer_stack.emplace_back(std::make_shared<T>(this))->OnAttach();
	}

	
	void SetWindowIcon(std::string icon_path)
	{
		ImageTexture image = ImageTexture(icon_path, GL_RGBA);
    
    	GLFWimage icon;
		icon.width = image.m_width;
		icon.height = image.m_height;
		icon.pixels = image.m_data;

		glfwSetWindowIcon(m_window->m_window, 1, &icon);
	}

	void SetWindowIcon(uint8_t* image_data, uint32_t image_size)
	{
		ImageTexture image = ImageTexture(image_data, image_size, GL_RGBA);

		GLFWimage icon;
		icon.width = image.m_width;
		icon.height = image.m_height;
		icon.pixels = image.m_data;

		glfwSetWindowIcon(m_window->m_window, 1, &icon);
	}
};