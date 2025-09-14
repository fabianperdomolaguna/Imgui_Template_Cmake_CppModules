#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

#include "core/window.h"
#include "core/imgui_context.h"
#include "core/layer.h"

struct AppSpecification
{
	std::string title;
	int32_t width;
	int32_t height;
	bool custom_title_bar;
};

class Application
{
public:
	AppSpecification m_app_specification;
    std::unique_ptr<Window> m_window;
	std::unique_ptr<ImguiContext> m_imgui_context;
	std::vector<std::shared_ptr<Layer>> m_layer_stack;
    
	std::string m_executable_path;

    Application(const AppSpecification& spec);
    ~Application();

    void Run();

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

    void SetWindowIcon(std::string icon_path);
    void SetWindowIcon(uint8_t* image_data, uint32_t image_size);
};