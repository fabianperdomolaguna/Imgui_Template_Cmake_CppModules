module;

#include <format>
#include <filesystem>
#include <string>

#include "Python.h"
#include "pybind11/embed.h"

export module PythonManager;

import beryl.logger;

namespace py = pybind11;

export class PythonManager
{
private:
    PythonManager() = default;
    ~PythonManager() = default;
    PythonManager(const PythonManager&) = delete;
    PythonManager& operator = (const PythonManager&) = delete;

    bool m_active = false;
    std::filesystem::path m_venv_root;

public:
    static PythonManager& Instance()
    {
        static PythonManager instance;
        return instance;
    }

    static PythonManager& PyMgr() { return Instance(); }

    void Configure(const std::filesystem::path& venv_root)
    {
        if (m_active) 
        {
            beryl::logger::Error("Cannot configure PythonManager after activation");
            return;
        }
        m_venv_root = venv_root;
    }

    bool BeginSession(const std::string& scripts_path = "")
    {
        if(m_active)
            return true;

        if (m_venv_root.empty()) {
            beryl::logger::Error("[PythonManager] VENV Root not configured!");
            return false;
        }

        std::filesystem::path site_packages;
        #if defined(_WIN32)
            site_packages = m_venv_root / "Lib" / "site-packages";
        #else
            site_packages = m_venv_root / "lib" / std::format("python{}", PYTHON_VERSION) / "site-packages";
        #endif

        if (!std::filesystem::exists(site_packages)) {
            beryl::logger::Error(std::format("[PythonManager] Invalid venv: site-packages not found at {}", site_packages.string()));
            return false;
        }

        Py_Initialize();
        if (!Py_IsInitialized())
        {
            beryl::logger::Error("[PythonManager] Py_Initialize failed");
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
            beryl::logger::Error(std::format("[PythonManager] Failed to setup sys.path: {}", e.what()));
        }

        beryl::logger::Info("[PythonManager] Python session started");
        return true;
    }

    void EndSession()
    {
        if(!m_active)
            return;

        Py_Finalize();

        m_active = false;
        beryl::logger::Info("[PythonManager] Python session ended");
    }

    py::module ImportModule(const std::string& name)
    {
        if (!m_active) {
            beryl::logger::Error("[PythonManager] ImportModule failed: interpreter not initialized");
            return py::module();
        }

        try {
            return py::module::import(name.c_str());
        }
        catch (const py::error_already_set& e) {
            beryl::logger::Error(std::format("[PythonManager] could not complete ImportModule('{}'): {}", name, e.what()));
            return py::module();
        }
    }

    void AddSystemPath(const std::string& path)
    {
        if (!m_active) {
            beryl::logger::Error("[PythonManager] AddSystemPath failed: interpreter not initialized");
            return;
        }

        try {
            py::module sys = py::module::import("sys");
            sys.attr("path").attr("insert")(0, path);
        }
        catch (const py::error_already_set& e) {
            beryl::logger::Error(std::format("[PythonManager] could not complete AddSystemPath('{}'): {}", path, e.what()));
            return;
        }
    }

    template<typename... Args>
    py::object SafeCall(py::object callable, Args&&... args)
    {
        if (!m_active) {
            beryl::logger::Error("[PythonManager] SafeCall failed: interpreter not initialized");
            return py::object();
        }

        try {
            return callable(std::forward<Args>(args)...);
        }
        catch (const py::error_already_set& e) {
            beryl::logger::Error(std::format("[PythonManager] Interpreter could not complete SafeCall: {}", e.what()));
            return py::object();
        }
    }
};

export inline PythonManager& PyMgr = PythonManager::PyMgr();