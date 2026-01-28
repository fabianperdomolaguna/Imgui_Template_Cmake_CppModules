module;

#include <format>
#include <string>
#include <memory>

#include "pybind11/embed.h"

#include "logger.h"

export module PythonManager;

namespace py = pybind11;

export class PythonManager
{
private:
    PythonManager() = default;
    ~PythonManager() = default;
    PythonManager(const PythonManager&) = delete;
    PythonManager& operator=(const PythonManager&) = delete;

    std::unique_ptr<py::scoped_interpreter> m_python_interpreter;
public:
    static PythonManager& Instance()
    {
        static PythonManager instance;
        return instance;
    }

    static PythonManager& PyMgr() { return Instance(); }

    bool Initialize(const std::string& path = "")
    {
        if (m_python_interpreter)
            return true;

        try {
            m_python_interpreter = std::make_unique<py::scoped_interpreter>(py::scoped_interpreter{});
            if (!path.empty())
                AddSystemPath(path);
            LOG_INFO("[PythonManager] Python interpreter initialized");
            return true;
        }
        catch (const py::error_already_set& e) {
            LOG_ERROR(std::format("[PythonManager] initialization failed: {}", e.what()));
            m_python_interpreter.reset();
            return false;
        }
    }

    bool IsInitialized() const { return static_cast<bool>(m_python_interpreter); }

    void AddSystemPath(const std::string& path)
    {
        if (!IsInitialized()) {
            LOG_ERROR("[PythonManager] AddSystemPath failed: interpreter not initialized");
            return;
        }

        try {
            py::module sys = py::module::import("sys");
            sys.attr("path").attr("insert")(0, path);
        }
        catch (const py::error_already_set& e) {
            LOG_ERROR(std::format("[PythonManager] could not complete AddSystemPath('{}'): {}", path, e.what()));
            return;
        }
    }

    py::module ImportModule(const std::string& name)
    {
        if (!IsInitialized()) {
            LOG_ERROR("[PythonManager] ImportModule failed: interpreter not initialized");
            return py::module();
        }

        try {
            return py::module::import(name.c_str());
        }
        catch (const py::error_already_set& e) {
            LOG_ERROR(std::format("[PythonManager] could not complete ImportModule('{}'): {}", name, e.what()));
            return py::module();
        }
    }

    template<typename... Args>
    py::object SafeCall(py::object callable, Args&&... args)
    {
        if (!IsInitialized()) {
            LOG_ERROR("[PythonManager] SafeCall failed: interpreter not initialized");
            return py::object();
        }

        try {
            return callable(std::forward<Args>(args)...);
        }
        catch (const py::error_already_set& e) {
            LOG_ERROR(std::format("[PythonManager] Interpreter could not complete SafeCall: {}", e.what()));
            return py::object();
        }
    }
};