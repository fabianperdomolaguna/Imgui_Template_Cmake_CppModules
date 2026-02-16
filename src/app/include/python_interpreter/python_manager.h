#pragma once

#include <string>
#include <filesystem>
#include <iostream>

#include "pybind11/embed.h"

#include "logging/logger.h"

namespace py = pybind11;

class PythonManager
{
private:
    PythonManager() = default;
    ~PythonManager() = default;
    PythonManager(const PythonManager&) = delete;
    PythonManager& operator=(const PythonManager&) = delete;

    bool m_active = false;
    std::filesystem::path m_venv_root;
    
public:
    static PythonManager& Instance();
    static PythonManager& PyMgr();
    void Configure(const std::filesystem::path& venv_root);
    bool BeginSession(const std::string& scripts_path = "");
    void EndSession();
    py::module ImportModule(const std::string& name);
    void AddSystemPath(const std::string& path);

    template<typename... Args>
    py::object SafeCall(py::object callable, Args&&... args)
    {
        if (!m_active) {
            Logger::Error("[PythonManager] SafeCall failed: interpreter not initialized");
            return py::object();
        }

        try {
            return callable(std::forward<Args>(args)...);
        }
        catch (const py::error_already_set& e) {
            Logger::Error(std::format("[PythonManager] Interpreter could not complete SafeCall: {}", e.what()));
            return py::object();
        }
    }
};

inline PythonManager& PyMgr = PythonManager::PyMgr();