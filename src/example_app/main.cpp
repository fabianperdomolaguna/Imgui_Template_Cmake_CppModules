#include "GLFW/glfw3.h"
#include "pybind11/embed.h"

import Application;
import MainMenuBar;
import RenderScene;

namespace py = pybind11;

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

    Application* app = new Application("ImGui - OpenGL Context", 1600, 800);

    app->PushLayerApp<MainMenuBar>();
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