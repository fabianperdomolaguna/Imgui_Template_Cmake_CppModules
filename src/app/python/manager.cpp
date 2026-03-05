module;

#include <filesystem>
#include <format>
#include <memory>
#include <mutex>
#include <string>

#include "Python.h"
#include "pybind11/embed.h"

export module app.python.manager;

import beryl.logger;

namespace py = pybind11;

export namespace app::python
{
    class Manager
    {
    private:
        Manager() = default;
        
        bool m_active = false;
        std::filesystem::path m_venv_root;
        std::mutex m_init_mutex;

        std::unique_ptr<py::gil_scoped_release> m_main_python;

    public:
        Manager(const Manager&) = delete;
        Manager& operator = (const Manager&) = delete;
        
        static Manager& Instance()
        {
            static Manager instance;
            return instance;
        }

        static Manager& PyMgr() { return Instance(); }

        ~Manager()
        {
            if (m_active)
            {
                m_main_python.reset();
                Py_Finalize();
            }
        }

        void Configure(const std::filesystem::path& venv_root)
        {
            std::scoped_lock lock(m_init_mutex);
            if (m_active) return;
            m_venv_root = venv_root;
        }

        bool BeginSession()
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

        py::module ImportModule(const std::string& name)
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

        void AddSystemPath(const std::string& path)
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

        template<typename... Args>
        py::object SafeCall(const py::object& callable, Args&&... args)
        {
            if (!m_active) 
            {
                beryl::logger::Error("[PythonManager] SafeCall failed: interpreter not initialized");
                return {};
            }

            py::gil_scoped_acquire acquire;
            try 
            {
                return callable(std::forward<Args>(args)...);
            }
            catch (const py::error_already_set& e) 
            {
                beryl::logger::Error(std::format("[PythonManager] Interpreter could not complete SafeCall: {}", e.what()));
                return {};
            }
        }
    };

    inline Manager& PyMgr() 
    { 
        return Manager::Instance(); 
    }
}