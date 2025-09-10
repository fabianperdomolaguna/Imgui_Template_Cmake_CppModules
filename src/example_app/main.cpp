#include "pybind11/embed.h"

#include "icon_app.h"

import Application;
import TitleBar;
import MainMenuBar;
import CustomMainMenuBar;
import RenderScene;

namespace py = pybind11;

/* Assign Icons with IDI Icon Resource*/
#if defined(_WIN32)
  #include "GLFW/glfw3.h"
  #define GLFW_EXPOSE_NATIVE_WIN32
  #include <GLFW/glfw3native.h>
  #include "icon_resource.h"
  
  void SetWin32WindowIcons(GLFWwindow* window) {
    HWND hwnd = glfwGetWin32Window(window);
    HICON hIcon = LoadIconA(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON_APP));

    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    SetClassLongPtr(hwnd, GCLP_HICON, (LONG_PTR)hIcon);
    SetClassLongPtr(hwnd, GCLP_HICONSM, (LONG_PTR)hIcon);
  }
#endif

int Main(int argc, char** argv)
{	
    py::scoped_interpreter guard{};

    Application* app = new Application({
        .title = "ImGui - OpenGL Context",
        .width = 1600,
        .height = 800,
        .custom_title_bar = true
    });
    app->SetWindowIcon(g_icon_app, g_icon_app_len);

    app->PushLayerApp<TitleBar>();
    app->PushLayerApp<MenuBar>();

    /*BeginMainMenuBar cannot be used with a Custom Titlebar, 
    because it is always anchored to the main viewport at (0,0)*/
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