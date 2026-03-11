#include "beryl/utils/path_utils.h"

#include <exception>
#include <format>
#include <stdexcept>
#include <string>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
#endif

#include "beryl/logging/logger.h"

std::filesystem::path beryl::utils::path::GetExecutablePath()
{
#ifdef _WIN32
    std::wstring buffer(32767, L'\0');
    DWORD size = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (size == 0)
    {
        beryl::logger::Critical(std::format("Failed to get executable path: {},", GetLastError()));
        throw std::runtime_error("Failed to get executable path");
    }
    if (size == buffer.size())
    {
        beryl::logger::Critical("Executable path is too long");
        throw std::runtime_error("Executable path is too long");
    }
    buffer.resize(size);
    return std::filesystem::path(buffer).parent_path();
#else
    try
    {
        return std::filesystem::read_symlink("/proc/self/exe").parent_path();
    }
    catch (const std::exception& e)
    {
        beryl::logger::Critical(std::format("Failed to read executable symlink: {}", e.what()));
        throw;
    }
#endif
}