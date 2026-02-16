#include <format>
#include <memory>

#include "pybind11/embed.h"
namespace py = pybind11;

#include "python_interpreter/python_manager.h"
#include "logging/logger.h"

PythonManager& PythonManager::Instance()
{
    static PythonManager instance;
    return instance;
}

PythonManager& PythonManager::PyMgr()
{
    return Instance();
}

void PythonManager::Configure(const std::filesystem::path& venv_root)
{
    if (m_active) 
    {
        Logger::Error("Cannot configure PythonManager after activation");
        return;
    }
    m_venv_root = venv_root;
}

bool PythonManager::BeginSession(const std::string& scripts_path)
{
    if(m_active)
        return true;

    if (m_venv_root.empty()) {
        Logger::Error("[PythonManager] VENV Root not configured!");
        return false;
    }

    std::filesystem::path site_packages;
    #if defined(_WIN32)
        site_packages = m_venv_root / "Lib" / "site-packages";
    #else
        site_packages = m_venv_root / "lib" / std::format("python{}", PYTHON_VERSION) / "site-packages";
    #endif

    if (!std::filesystem::exists(site_packages)) {
        Logger::Error(std::format("[PythonManager] Invalid venv: site-packages not found at {}", site_packages.string()));
        return false;
    }

    Py_Initialize();
    if (!Py_IsInitialized())
    {
        Logger::Error("[PythonManager] Py_Initialize failed");
        return false;
    }
    m_active = true;

    try 
    {
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, site_packages.string());
        
        if (!scripts_path.empty()) {
            AddSystemPath(scripts_path);
        }
    } catch (const py::error_already_set& e) 
    {
        Logger::Error(std::format("[PythonManager] Failed to setup sys.path: {}", e.what()));
    }

    Logger::Info("[PythonManager] Python session started");
    return true;
}

void PythonManager::EndSession()
{
    if(!m_active)
        return;

    Py_Finalize();

    m_active = false;
    Logger::Info("[PythonManager] Python session ended");
}

py::module PythonManager::ImportModule(const std::string& name)
{
    if (!m_active) {
        Logger::Error("[PythonManager] ImportModule failed: interpreter not initialized");
        return py::module();
    }

    try {
        return py::module::import(name.c_str());
    }
    catch (const py::error_already_set& e) {
        Logger::Error(std::format("[PythonManager] could not complete ImportModule('{}'): {}", name, e.what()));
        return py::module();
    }
}

void PythonManager::AddSystemPath(const std::string& path)
{
    if (!m_active) {
        Logger::Error("[PythonManager] AddSystemPath failed: interpreter not initialized");
        return;
    }

    try {
        py::module sys = py::module::import("sys");
        sys.attr("path").attr("insert")(0, path);
    }
    catch (const py::error_already_set& e) {
        Logger::Error(std::format("[PythonManager] could not complete AddSystemPath('{}'): {}", path, e.what()));
        return;
    }
}