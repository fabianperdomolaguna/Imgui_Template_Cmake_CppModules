#pragma once

#include <filesystem>
#include <format>
#include <fstream>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>

#include "nlohmann/json.hpp"

#include "beryl/logging/logger.h"

struct ConfigState 
{
    nlohmann::json cache;
    bool loaded = false;
    std::shared_mutex mutex;
};

ConfigState& GetState();
bool LoadIfNeeded(const std::filesystem::path& executable_path);

namespace beryl::config::gui
{
    template <typename T>
    std::optional<T> Get(const std::filesystem::path& executable_path, const std::string& key)
    {
        if (!LoadIfNeeded(executable_path))
            return std::nullopt;

        auto& state = GetState();

        std::shared_lock lock(state.mutex);

        if (!state.cache.contains(key) || state.cache[key].is_null())
        {
            beryl::logger::Warn(std::format("Key '{}' not found in Config GUI file", key));
            return std::nullopt;
        }

        try
        {
            return state.cache[key].get<T>();
        }
        catch (const std::exception& e)
        {
            beryl::logger::Warn(std::format("Type mismatch for '{}': {}", key, e.what()));
            return std::nullopt;
        }
    }

    template <typename T>
    void Set(const std::filesystem::path& executable_path, const std::string& key, const T& value)
    {
        if (!LoadIfNeeded(executable_path))
        {
            beryl::logger::Error(std::format(
                "Aborting save for Key '{}' - GUI config file could not be validated", key));
            return;
        }

        auto& state = GetState();

        std::unique_lock lock(state.mutex);

        state.cache[key] = value;

        std::filesystem::path config_path = executable_path / "ConfigGuiFile.json";
        std::ofstream config_file(config_path);
        if (config_file.is_open())
        {
            config_file << state.cache.dump(4);
        }
        else
        {
            beryl::logger::Error("Failed to write GUI config to disk");
        }
    }
}