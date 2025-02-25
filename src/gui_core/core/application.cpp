module;

#include <string>
#include <vector>
#include <memory>
#include <functional>

export module Application;

import Window;
import ImguiContext;
import Layer;

export class Application
{
    
public:
    std::unique_ptr<Window> m_window;
	std::unique_ptr<ImguiContext> m_imgui_context;
	std::vector<std::shared_ptr<Layer>> m_layer_stack;
    std::function<void()> m_menubar;

	Application(std::string window_title)
	{
		m_window = std::make_unique<Window>(window_title, 1200, 600);
		m_imgui_context = std::make_unique<ImguiContext>(m_window->m_window);
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
};