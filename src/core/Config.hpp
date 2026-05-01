#pragma once

#include <filesystem>
#include <string>

struct Config {
    int maxIdleFrames = 1;
    int maxTalkFrames = 1;
    int maxScreamFrames = 1;

    float shakeStrength = 5.0f;
    float shakeDuration = 0.25f;

    enum class ShakeMode {
        X = 0,
        Y = 1,
        XY = 2
    } shakeMode = ShakeMode::XY;
};

class ConfigManager {
public:
    static Config& get();

    static void load();
    static void save();

    static std::filesystem::path get_appdata_path();
    static std::filesystem::path get_app_dir();
    static std::filesystem::path get_assets_dir();
    static std::filesystem::path get_shader_dir();
    static std::filesystem::path get_config_path();

    static void ensure_folders();

private:
    static Config config;
};