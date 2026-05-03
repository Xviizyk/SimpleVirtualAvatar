#pragma once

#include "../system/AssetManager.hpp"
#include "raylib.h"
#include <filesystem>
#include <string>

class AssetManager;

class SpriteEditor {
public:
    void open_window(AvatarState state);
    bool is_open() const;

    void draw(AssetManager& assets, float dpi);

    bool open = false;
    bool isDragging = false;

    int pendingFrames = 1;
private:
    AvatarState editingState = AvatarState::IDLE;
    bool editingBlink = false;

    Rectangle windowRect = { 120.0f, 120.0f, 420.0f, 300.0f };
    Vector2 dragOffset { 0.0f, 0.0f};
    static constexpr int MAX_FRAMES = 64;
    static constexpr int COLUMNS = 4;

    int& frame_count_ref();
    const char* state_name() const;
    const char* prefix() const;

    std::filesystem::path frame_path(int index) const;

    void sync_from_config();
    void sync_to_config_and_reload(AssetManager& assets);

    bool open_file_dialog(std::string& outPath) const;
    bool assign_image_to_slot(int index, const std::string& sourcePath, AssetManager& assets);
};