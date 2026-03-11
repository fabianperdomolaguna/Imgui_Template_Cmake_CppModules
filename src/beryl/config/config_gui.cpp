#include "beryl/config/config_gui.h"

ConfigState& GetState()
{
    static ConfigState instance;
    return instance;
}

bool LoadIfNeeded(const std::filesystem::path& executable_path)
{
    auto& state = GetState();

    std::unique_lock lock(state.mutex);

    if (state.loaded)
        return true;

    std::filesystem::path config_path = executable_path / "ConfigGuiFile.json";

    if (!std::filesystem::exists(config_path))
    {
        beryl::logger::Error(std::format("Config GUI file missing: {}", config_path.string()));
        return false;
    }

    try
    {
        std::ifstream config_file(config_path);
        if (!config_file.is_open())
            return false;
        state.cache = nlohmann::json::parse(config_file);
        state.loaded = true;
        return true;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        beryl::logger::Error(std::format("JSON Parse Error: {}", e.what()));
        return false;
    }
}