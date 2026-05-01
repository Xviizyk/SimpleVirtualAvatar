#pragma once

#include "../system/AssetManager.hpp"
#include "../core/Config.hpp"

class SpriteEditor {
public:
    void open_window(AvatarState state);
    bool is_open() const;

    void draw(AssetManager& assets);

private:
    Rectangle windowRect = { 100, 100, 600, 400 };
    bool isDragging = false;
    bool isResizing = false;
    Vector2 dragOffset = { 0, 0 };
    const float RESIZE_BORDER = 10.0f;

    bool open = false;
    bool editingBlink = false;
    AvatarState editingState = AvatarState::IDLE;
    int pendingFrames = 1;

    static constexpr int MAX_FRAMES = 32;
    static constexpr int COLUMNS = 4;

    int& frame_count_ref();
    const char* state_name() const;
    const char* prefix() const;
    std::filesystem::path frame_path(int index) const;

    void sync_from_config();
    void sync_to_config_and_reload(AssetManager& assets);
    bool assign_image_to_slot(int index, const std::string& sourcePath, AssetManager& assets);

    bool open_file_dialog(std::string& outPath) const;
};