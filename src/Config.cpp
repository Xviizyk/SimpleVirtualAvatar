#include "Config.hpp"
#include "WinUtils.hpp"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

Config ConfigManager::config;

fs::path ConfigManager::GetAppDataRoot() {
    return fs::path(WinUtils::get_appdata_path());
}

fs::path ConfigManager::GetAppDir() {
    fs::path path = GetAppDataRoot() / "SimpleVirtualAvatar";
    return path;
}

fs::path ConfigManager::GetConfigPath() {
    return GetAppDir() / "config.json";
}

fs::path ConfigManager::GetAssetsDir() {
    return GetAppDir() / "assets";
}

fs::path ConfigManager::GetShaderDir() {
    return GetAppDir() / "shaders";
}

void ConfigManager::EnsureFolders() {
    fs::create_directories(GetAssetsDir());
    fs::create_directories(GetShaderDir());
}

Config& ConfigManager::Get() {
    return config;
}

void ConfigManager::Load() {
    EnsureFolders();
    
    std::ifstream file(GetConfigPath());
    if (!file.is_open()) return;

    try {
        nlohmann::json j;
        file >> j;

        config.maxIdleFrames   = j.value("max_idle_frames", 1);
        config.maxTalkFrames   = j.value("max_talk_frames", 1);
        config.maxScreamFrames = j.value("max_scream_frames", 1);
        config.shakeStrength   = j.value("shake_strength", 5.0f);
        config.shakeDuration   = j.value("shake_duration", 0.25f);

        std::string mode = j.value("shake_mode", "xy");
        if (mode == "x")      config.shakeMode = Config::ShakeMode::X;
        else if (mode == "y") config.shakeMode = Config::ShakeMode::Y;
        else                  config.shakeMode = Config::ShakeMode::XY;
    } catch (const std::exception& e) {
        std::cerr << "Config load error: " << e.what() << std::endl;
    }
}

void ConfigManager::Save() {
    nlohmann::json j;

    j["max_idle_frames"]   = config.maxIdleFrames;
    j["max_talk_frames"]   = config.maxTalkFrames;
    j["max_scream_frames"] = config.maxScreamFrames;
    j["shake_strength"]    = config.shakeStrength;
    j["shake_duration"]    = config.shakeDuration;

    std::string mode = "xy";
    if (config.shakeMode == Config::ShakeMode::X) mode = "x";
    if (config.shakeMode == Config::ShakeMode::Y) mode = "y";
    j["shake_mode"] = mode;

    std::ofstream file(GetConfigPath());
    if (file.is_open()) {
        file << j.dump(4);
    }
}