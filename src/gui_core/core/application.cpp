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

struct AppSpecfication
{
	std::string name;
	int32_t width;
	int32_t height;

	bool customTitleBar = false;
	bool centerWindow = false;
};

export class Application
{
    
public:
	AppSpecfication m_app_specification;
    std::unique_ptr<Window> m_window;
	std::unique_ptr<ImguiContext> m_imgui_context;
	std::vector<std::shared_ptr<Layer>> m_layer_stack;
    
	std::string m_executable_path;

	Application(std::string window_title, int32_t width, int32_t height)
	{
		m_app_specification.name = window_title;
		m_app_specification.width = width;
		m_app_specification.height = height;
		m_executable_path = GetExecutablePath().string();

		m_window = std::make_unique<Window>(window_title, width, height);
		m_imgui_context = std::make_unique<ImguiContext>(m_window->m_window, m_executable_path);
	}

	~Application() {};

	void Run()
	{
		while (m_window->m_running)
		{
			m_window->PreRender();
			m_imgui_context->PreRender();

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
    	auto image = ReadImage(icon_path);

    	if (!image.success) {
        	std::cout << "Error: the icon could not be set. Image not loaded." << std::endl;
        	return;
    	}
    
    	GLFWimage icon;
    	icon.width = image.width;
    	icon.height = image.height;
    	icon.pixels = image.data;

    	glfwSetWindowIcon(m_window->m_window, 1, &icon);
    	stbi_image_free(image.data);
	}
};