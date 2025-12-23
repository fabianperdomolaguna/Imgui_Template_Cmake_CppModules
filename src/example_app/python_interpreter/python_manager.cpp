#include <format>
#include <memory>

#include "pybind11/embed.h"
namespace py = pybind11;

#include "python_manager.h"
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

bool PythonManager::Initialize(const std::string& path)
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

bool PythonManager::IsInitialized() const
{
    return static_cast<bool>(m_python_interpreter);
}

void PythonManager::AddSystemPath(const std::string& path)
{
    if (!IsInitialized()) {
        LOG_ERROR("[PythonManager] AddSystemPath failed: interpreter not initialized");
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

py::module PythonManager::ImportModule(const std::string& name)
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