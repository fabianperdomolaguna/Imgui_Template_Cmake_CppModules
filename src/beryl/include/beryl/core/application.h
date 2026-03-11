#pragma once

#include <concepts>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "beryl/core/window.h"
#include "beryl/core/layer.h"
#include "beryl/gui/context.h"

template<typename T>
concept IsLayer = std::derived_from<T, beryl::core::Layer>;

namespace beryl::core
{
	class Application
	{
	public:
		std::unique_ptr<beryl::core::Window> m_window;
		std::unique_ptr<beryl::gui::Context> m_gui_context;
		std::vector<std::unique_ptr<beryl::core::Layer>> m_layer_stack;
		
		std::filesystem::path m_executable_path;

		Application(const beryl::core::WindowSpecification& spec);
		virtual ~Application() = default;

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;

		void Run();

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

		void SetWindowIcon(const std::string& icon_path) const;
		void SetWindowIcon(const uint8_t* image_data, uint32_t image_size) const;
	};
}