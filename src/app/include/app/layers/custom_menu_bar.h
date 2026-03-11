#pragma once

#include "beryl/core/application.h"
#include "beryl/core/layer.h"

void ShowStyleEditor(beryl::core::Application* app);

namespace app::layer
{
    class CustomMenuBar : public beryl::core::Layer
    {
        bool m_show_style_editor = false;
		bool m_show_logging_console = false;

		beryl::core::Application* m_app = nullptr;

	public:
		CustomMenuBar(beryl::core::Application* app);

        void OnRender() override;
    };
}