#pragma once

#include <string>

#include "nlohmann/json.hpp"

nlohmann::json ReadConfigFile(std::string executable_path);
void WriteConfigFile(std::string executable_path, nlohmann::json& json_data);

template<typename T>
T GetConfigVariable(std::string executable_path, std::string config_variable);

template<>
float GetConfigVariable<float>(std::string executable_path, std::string config_variable);

template<>
std::string GetConfigVariable<std::string>(std::string executable_path, std::string config_variable);

template<typename T>
void ChangeConfigVariable(std::string executable_path, std::string config_variable, T new_config_value);

template<>
void ChangeConfigVariable(std::string executable_path, std::string config_variable, std::string new_config_value);

template<>
void ChangeConfigVariable(std::string executable_path, std::string config_variable, float new_config_value);
