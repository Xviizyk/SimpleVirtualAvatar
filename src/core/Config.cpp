#include "Config.hpp"
#include "../utils/OsUtils.hpp"
#include <iostream>
#include <fstream>
#include "../thirdparty/nlohmann/json.hpp"

namespace fs = std::filesystem;

Config ConfigManager::config;

fs::path ConfigManager::get_appdata_path() {
    return fs::path(OsUtils::get_appdata_path());
}

fs::path ConfigManager::get_app_dir() {
    fs::path path = get_appdata_path() / "SimpleVirtualAvatar";
    return path;
}

fs::path ConfigManager::get_config_path() {
    return get_app_dir() / "config.json";
}

fs::path ConfigManager::get_assets_dir() {
    return get_app_dir() / "assets";
}

fs::path ConfigManager::get_shader_dir() {
    return get_app_dir() / "shaders";
}

void ConfigManager::ensure_folders() {
    fs::create_directories(get_assets_dir());
    fs::create_directories(get_shader_dir());
}

Config& ConfigManager::get() {
    return config;
}

void ConfigManager::load() {
    ensure_folders();
    
    std::ifstream file(get_config_path());
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

void ConfigManager::save() {
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

    std::ofstream file(get_config_path());
    if (file.is_open()) {
        file << j.dump(4);
    }
}