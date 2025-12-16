#include "pybind11/embed.h"

#include "icon_app.embed"

import Application;
import TitleBar;
import MainMenuBar;
import CustomMainMenuBar;
import RenderScene;
import PythonManager;

namespace py = pybind11;

int Main(int argc, char** argv)
{	
	PythonManager::Instance().Initialize();

    Application* app = new Application({
        .title = "ImGui - OpenGL Context",
        .width = 1600,
        .height = 800,
        .custom_title_bar = true
    });

    app->PushLayerApp<TitleBar>();
    app->PushLayerApp<CustomMenuBar>();

    /*BeginMainMenuBar cannot be used with a Custom Titlebar, 
    because it is always anchored to the main viewport at (0,0)*/
    //If dont use the custom titlebar you can activate the SetWindowIcon
    //app->SetWindowIcon(g_icon_app, g_icon_app_len);
    //app->PushLayerApp<MainMenuBar>();

    app->PushLayer<SimpleRender>(app->m_executable_path);

    app->Run();

    delete app;

    return 0;
}

#if defined(_WIN32) && !defined(_DEBUG)
    #include <Windows.h>

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