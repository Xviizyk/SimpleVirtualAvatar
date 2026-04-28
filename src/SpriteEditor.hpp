#pragma once

#include "AssetManager.hpp"
#include "Config.hpp"

class SpriteEditor {
public:
    void Open(AvatarState state);
    bool IsOpen() const;

    void Draw(AssetManager& assets);

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

    int& FrameCountRef();
    const char* StateName() const;
    const char* Prefix() const;
    std::filesystem::path FramePath(int index) const;

    void SyncFromConfig();
    void SyncToConfigAndReload(AssetManager& assets);
    bool AssignImageToSlot(int index, const std::string& sourcePath, AssetManager& assets);

    bool OpenFileDialog(std::string& outPath) const;
};