#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>
#endif

#include <format>

#include "pybind11/embed.h"

import Application;
import TitleBar;
import CustomMainMenuBar;
import MainMenuBar;
import RenderScene;
import PythonManager;
import Logger;
import Icons.App;

namespace py = pybind11;

int Main(int argc, char** argv)
{	
    Logger::Init();

    Application* app = new Application({
        .title = "ImGui - OpenGL Context",
        .width = 1600,
        .height = 800,
        .custom_title_bar = true
    });

    PythonManager::Instance().Initialize(app->m_executable_path);

    app->PushLayerApp<TitleBar>();
    app->PushLayerApp<CustomMenuBar>();

    /*BeginMainMenuBar cannot be used with a Custom Titlebar, 
    because it is always anchored to the main viewport at (0,0)*/
    //If dont use the custom titlebar you can activate the SetWindowIcon
    // app->SetWindowIcon(icons::app, icons::app_len);
    // app->PushLayerApp<MainMenuBar>();

    app->PushLayer<SimpleRender>(app->m_executable_path);

    Logger::Info("App started");

    app->Run();

    delete app;

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