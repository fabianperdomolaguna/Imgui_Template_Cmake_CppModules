module;

#include <fstream>
#include <string>

#include "nlohmann/json.hpp"

export module SettingsConfig;

nlohmann::json ReadConfigFile(std::string executable_path)
{
    std::ifstream json_config_file(executable_path + "/ConfigFile.json");
    if (!json_config_file.is_open())
        throw std::runtime_error("Config file not found or could not be opened.");

    try {
        return nlohmann::json::parse(json_config_file);
    } catch (const nlohmann::json::parse_error& e) {
        throw std::runtime_error("Error parsing JSON config file: " + std::string(e.what()));
    }
}

void WriteConfigFile(std::string executable_path, nlohmann::json& json_data)
{
    std::ofstream json_config_file(executable_path + "/ConfigFile.json");
    if (!json_config_file.is_open())
        throw std::runtime_error("Could not open config file for writing.");

    json_config_file << json_data.dump(4);
}

export template<typename T>
T GetConfigVariable(std::string executable_path, std::string config_variable);

template<>
float GetConfigVariable<float>(std::string executable_path, std::string config_variable) 
{
    nlohmann::json json_data = ReadConfigFile(executable_path);
    return json_data[config_variable].get<float>();
}

template<>
std::string GetConfigVariable<std::string>(std::string executable_path, std::string config_variable) 
{
    nlohmann::json json_data = ReadConfigFile(executable_path);
    return json_data[config_variable].get<std::string>();
}

export template<typename T>
void ChangeConfigVariable(std::string executable_path, std::string config_variable, T new_config_value);

template<>
void ChangeConfigVariable(std::string executable_path, std::string config_variable, std::string new_config_value)
{
    nlohmann::json json_data = ReadConfigFile(executable_path);
    json_data[config_variable] = new_config_value;

    WriteConfigFile(executable_path, json_data);
}

template<>
void ChangeConfigVariable(std::string executable_path, std::string config_variable, float new_config_value)
{
    nlohmann::json json_data = ReadConfigFile(executable_path);
    json_data[config_variable] = new_config_value;

    WriteConfigFile(executable_path, json_data);
}
