#pragma once

#include <filesystem>
#include <mutex>
#include <memory>
#include <string>

#include "pybind11/embed.h"

#include "beryl/logging/logger.h"

namespace py = pybind11;

namespace app::python
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

        static Manager& Instance();

        ~Manager();

        void Configure(const std::filesystem::path& venv_root);
        bool BeginSession();
        py::module ImportModule(const std::string& name);
        void AddSystemPath(const std::string& path);

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