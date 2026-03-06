#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>
#endif

#include <format>
#include <memory>

import beryl.core;
import beryl.logger;
import app.python.manager;
import app.icons.window_icon;
import app.layers;

int Main(int argc, char** argv)
{	
    beryl::logger::Init();

    std::unique_ptr<beryl::core::Application> app = 
        std::make_unique<beryl::core::Application>(beryl::core::WindowSpecification{
            .title = "ImGui - OpenGL Context",
            .width = 1600,
            .height = 800,
            .custom_title_bar = true
        });

    app::python::Manager::Instance();
    app::python::PyMgr().Configure(app->m_executable_path / ".venv");

    app->PushLayerApp<app::layer::TitleBar>();
    app->PushLayerApp<app::layer::CustomMenuBar>();

    /*BeginMainMenuBar cannot be used with a Custom Titlebar, 
    because it is always anchored to the main viewport at (0,0)*/
    //If dont use the custom titlebar you can activate the SetWindowIcon
    // app->SetWindowIcon(app::icons::window_icon, app::icons::window_icon_len);
    // app->PushLayerApp<app::layer::MenuBar>();

    app->PushLayer<app::layer::SimpleRender>();

    beryl::logger::Info("App started");

    app->Run();

    beryl::logger::Shutdown();

    return 0;
}

#if defined(_WIN32) && !defined(_DEBUG)
    int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
    {
        return Main(__argc, __argv);
    }
#else
    int main(int argc, char** argv)
    {
        return Main(argc, argv);
    }
#endif