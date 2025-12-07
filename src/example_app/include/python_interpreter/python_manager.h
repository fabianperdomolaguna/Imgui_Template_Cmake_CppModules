#pragma once

#include <string>
#include <memory>
#include <iostream>

#include "pybind11/embed.h"
namespace py = pybind11;

class PythonManager
{
private:
    PythonManager() = default;
    ~PythonManager() = default;
    PythonManager(const PythonManager&) = delete;
    PythonManager& operator=(const PythonManager&) = delete;

    std::unique_ptr<py::scoped_interpreter> m_python_interpreter;
    
public:
    static PythonManager& Instance();
    static PythonManager& PyMgr();
    bool Initialize(const std::string& path = "");
    bool IsInitialized() const;
    void AddSystemPath(const std::string& path);
    py::module ImportModule(const std::string& name);

    template<typename... Args>
    py::object SafeCall(py::object callable, Args&&... args)
    {
        if (!IsInitialized()) {
            std::cerr << "[PythonManager] SafeCall failed: interpreter not initialized" << std::endl;
            return py::object();
        }

        try {
            return callable(std::forward<Args>(args)...);
        }
        catch (const py::error_already_set& e) {
            std::cerr << "[PythonManager] Interpreter could not complete SafeCall: " << e.what() << std::endl;
            return py::object();
        }
    }
};