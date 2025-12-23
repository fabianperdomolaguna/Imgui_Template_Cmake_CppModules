#include <filesystem>
#include <string>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "path/path_utilities.h"
#include "logging/logger.h"

std::filesystem::path GetExecutablePath() {
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        LOG_CRITICAL("Failed to get executable path on Windows");
        throw std::runtime_error("Failed to get executable path on Windows");
    }
    return std::filesystem::path(std::string(buffer, len)).parent_path();
#else
    char buffer[4096];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (len == -1) {
        LOG_CRITICAL("Failed to get executable path on Linux");
        throw std::runtime_error("Failed to get executable path on Linux");
    }
    return std::filesystem::path(std::string(buffer, len)).parent_path();
#endif
}