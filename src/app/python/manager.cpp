#include "app/python/manager.h"

#include <format>

#include "Python.h"

app::python::Manager& app::python::Manager::Instance()
{
    static Manager instance;
    return instance;
}

app::python::Manager::~Manager()
{
    if (m_active)
    {
        m_main_python.reset();
        Py_Finalize();
    }
}

void app::python::Manager::Configure(const std::filesystem::path& venv_root)
{
    std::scoped_lock lock(m_init_mutex);
    if (m_active) return;
    m_venv_root = venv_root;
}

bool app::python::Manager::BeginSession()
{
    std::scoped_lock lock(m_init_mutex);

    if (m_active) return true;

    if (m_venv_root.empty())
    {
        beryl::logger::Error("[PythonManager] VENV Root not configured!");
        return false;
    }

    std::filesystem::path site_packages;
    #if _WIN32
        site_packages = m_venv_root / "Lib" / "site-packages";
    #else
        site_packages = m_venv_root / "lib" / std::format("python{}", PYTHON_VERSION) / "site-packages";
    #endif

    if (!std::filesystem::exists(site_packages))
    {
        beryl::logger::Error(
            std::format("[PythonManager] Invalid venv: site-packages not found at {}",
                site_packages.string()));
        return false;
    }

    Py_Initialize();
    if (Py_IsInitialized() == 0)
    {
        beryl::logger::Error("[PythonManager] Py_Initialize failed");
        return false;
    }
    m_active = true;

    try 
    {
        {
            py::module sys = py::module::import("sys");
            sys.attr("path").attr("insert")(0, site_packages.string());
        }

        m_main_python = std::make_unique<py::gil_scoped_release>();
    } 
    catch (const py::error_already_set& e) 
    {
        beryl::logger::Error(std::format("[PythonManager] Failed to setup sys.path: {}", e.what()));
    }

    beryl::logger::Info("[PythonManager] Python session started");
    return true;
}

py::module app::python::Manager::ImportModule(const std::string& name)
{
    if (!m_active)
    {
        beryl::logger::Error("[PythonManager] ImportModule failed: interpreter not initialized");
        return {};
    }

    try 
    {
        return py::module::import(name.c_str());
    }
    catch (const py::error_already_set& e) 
    {
        beryl::logger::Error(std::format(
            "[PythonManager] could not complete ImportModule('{}'): {}", name, e.what()));
        return {};
    }
}

void app::python::Manager::AddSystemPath(const std::string& path)
{
    if (!m_active) 
    {
        beryl::logger::Error("[PythonManager] AddSystemPath failed: interpreter not initialized");
        return;
    }

    py::gil_scoped_acquire acquire;
    try 
    {
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, path);
    }
    catch (const py::error_already_set& e) 
    {
        beryl::logger::Error(std::format(
            "[PythonManager] could not complete AddSystemPath('{}'): {}", path, e.what()));
        return;
    }
}