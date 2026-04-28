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
    static Config& Get();

    static void Load();
    static void Save();

    static std::filesystem::path GetAppDataRoot();
    static std::filesystem::path GetAppDir();
    static std::filesystem::path GetAssetsDir();
    static std::filesystem::path GetShaderDir();
    static std::filesystem::path GetConfigPath();

    static void EnsureFolders();

private:
    static Config config;
};