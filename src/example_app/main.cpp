#include "pybind11/embed.h"

import Application;
import MainMenuBar;
import RenderScene;

namespace py = pybind11;

int Main(int argc, char** argv)
{	
    py::scoped_interpreter guard{};

    Application* app = new Application("ImGui - OpenGL Context");

    app->PushLayerApp<MainMenuBar>();
    app->PushLayer<SimpleRender>();

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