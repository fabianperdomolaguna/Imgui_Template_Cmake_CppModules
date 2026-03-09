module;

#include <concepts>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

export module beryl.core:application;

import :window;
import :layer;
import beryl.gui;
import beryl.renderer;
import beryl.utils.path;

template<typename T>
concept IsLayer = std::derived_from<T, beryl::core::Layer>;

export namespace beryl::core
{
	class Application
	{
	public:
		std::unique_ptr<beryl::core::Window> m_window;
		std::unique_ptr<beryl::gui::Context> m_gui_context;
		std::vector<std::unique_ptr<beryl::core::Layer>> m_layer_stack;

		std::filesystem::path m_executable_path;

		Application(const beryl::core::WindowSpecification& spec)
		{
			m_executable_path = beryl::utils::path::GetExecutablePath();

			m_window = std::make_unique<beryl::core::Window>(spec);
			m_gui_context = std::make_unique<beryl::gui::Context>(m_window->m_glfw_window);
			m_gui_context->UpdateDockspaceFlags(m_window->m_window_specification.custom_title_bar);
		}

		virtual ~Application() = default;

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;

		void Run()
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

		template<typename T, typename... Args>
		void PushLayer(Args&&... args)
		{
			auto layer = std::make_unique<T>(std::forward<Args>(args)...);
			layer->OnAttach();
			m_layer_stack.emplace_back(std::move(layer));
		}

		template<typename T>
		void PushLayerApp()
		{
			auto layer = std::make_unique<T>(this);
			layer->OnAttach();
			m_layer_stack.emplace_back(std::move(layer));
		}

		template<typename T>
		T* GetLayer(std::string_view name)
		{
			for (auto& layer : m_layer_stack)
			{
				if (layer->GetName() == name)
					return static_cast<T*>(layer.get());
			}
			return nullptr;
		}
		
		void SetWindowIcon(const std::string& icon_path) const
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

		void SetWindowIcon(const uint8_t* image_data, uint32_t image_size) const
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
	};
}