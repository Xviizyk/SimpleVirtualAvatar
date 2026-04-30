#include "SpriteEditor.hpp"
#include "raylib.h"
#include "OsUtils.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <string>
#include <vector>

namespace {
    static bool UiButton(const Rectangle& r, const char* text) {
        const bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        DrawRectangleRec(r, hovered ? Color{75, 75, 90, 255} : Color{55, 55, 65, 255});
        DrawRectangleLinesEx(r, 1.0f, Color{120, 120, 130, 255});

        const int fs = 18;
        const int tw = MeasureText(text, fs);
        DrawText(text, static_cast<int>(r.x + (r.width - tw) / 2.0f), static_cast<int>(r.y + (r.height - fs) / 2.0f), fs, RAYWHITE);

        return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    static std::string BaseName(const std::filesystem::path& p) {
        return p.filename().string();
    }

    static bool MousePressedOn(const Rectangle& r) {
        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), r);
    }
}

void SpriteEditor::Open(AvatarState state) {
    open = true;
    editingState = state;
    SyncFromConfig();
}

bool SpriteEditor::IsOpen() const {
    return open;
}

int& SpriteEditor::FrameCountRef() {
    auto& cfg = ConfigManager::Get();
    switch (editingState) {
        case AvatarState::IDLE:     return cfg.maxIdleFrames;
        case AvatarState::TALKING:  return cfg.maxTalkFrames;
        case AvatarState::SCREAMING: return cfg.maxScreamFrames;
    }
    return cfg.maxIdleFrames;
}

const char* SpriteEditor::StateName() const {
    switch (editingState) {
        case AvatarState::IDLE:     return "Idle";
        case AvatarState::TALKING:  return "Talk";
        case AvatarState::SCREAMING: return "Scream";
    }
    return "Unknown";
}

const char* SpriteEditor::Prefix() const {
    switch (editingState) {
        case AvatarState::IDLE:     return "idle";
        case AvatarState::TALKING:  return "talk";
        case AvatarState::SCREAMING: return "scream";
    }
    return "idle";
}

std::filesystem::path SpriteEditor::FramePath(int index) const {
    std::string filename = Prefix();
    if (editingBlink) {
        filename += "_blink";
    }
    filename += "_" + std::to_string(index) + ".png";
    return ConfigManager::GetAssetsDir() / filename;
}

void SpriteEditor::SyncFromConfig() {
    pendingFrames = std::clamp(FrameCountRef(), 1, MAX_FRAMES);
    FrameCountRef() = pendingFrames;
    ConfigManager::Save();
}

void SpriteEditor::SyncToConfigAndReload(AssetManager& assets) {
    pendingFrames = std::clamp(pendingFrames, 1, MAX_FRAMES);
    FrameCountRef() = pendingFrames;

    ConfigManager::Save();
    assets.reload_avatar_state(editingState, pendingFrames, Prefix());
}

bool SpriteEditor::OpenFileDialog(std::string& outPath) const {
    return OsUtils::open_file_dialog(GetWindowHandle(), outPath);
}

bool SpriteEditor::AssignImageToSlot(int index, const std::string& sourcePath, AssetManager& assets) {
    if (index < 0 || index >= pendingFrames) return false;

    Image img = LoadImage(sourcePath.c_str());
    if (img.data == nullptr) return false;

    const auto dst = FramePath(index);
    const bool ok = ExportImage(img, dst.string().c_str());
    UnloadImage(img);

    if (!ok) return false;

    assets.reload_avatar_state(editingState, pendingFrames, Prefix());
    return true;
}

void SpriteEditor::Draw(AssetManager& assets) {
    if (!open) return;

    Vector2 mouse = GetMousePosition();
    Rectangle headerRect = { windowRect.x, windowRect.y, windowRect.width, 30.0f };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, headerRect)) {
        isDragging = true;
        dragOffset = { mouse.x - windowRect.x, mouse.y - windowRect.y };
    }
    if (isDragging) {
        windowRect.x = mouse.x - dragOffset.x;
        windowRect.y = mouse.y - dragOffset.y;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isDragging = false;
    }

    const float panelX = windowRect.x;
    const float panelY = windowRect.y;
    const float panelW = windowRect.width;
    
    const float slotW = (panelW - 30.0f) / static_cast<float>(COLUMNS);
    const float slotH = 68.0f;
    const float headerH = 110.0f;
    const int rows = (pendingFrames + COLUMNS - 1) / COLUMNS;
    const float calculatedH = headerH + rows * (slotH + 10.0f) + 20.0f;
    windowRect.height = calculatedH;
    
    const float panelH = windowRect.height;

    DrawRectangleRec(Rectangle{panelX, panelY, panelW, panelH}, Color{28, 28, 32, 255});
    DrawRectangleLinesEx(Rectangle{panelX, panelY, panelW, panelH}, 1.0f, Color{90, 90, 100, 255});
    
    DrawText((std::string("Sprite Editor - ") + StateName()).c_str(), 
              static_cast<int>(panelX + 14.0f), static_cast<int>(panelY + 12.0f), 22, RAYWHITE);

    if (UiButton(Rectangle{panelX + panelW - 34.0f, panelY + 10.0f, 22.0f, 22.0f}, "X")) {
        open = false;
        return;
    }

    DrawText("Frames:", static_cast<int>(panelX + 14.0f), static_cast<int>(panelY + 48.0f), 18, LIGHTGRAY);

    if (UiButton(Rectangle{panelX + 85.0f, panelY + 42.0f, 30.0f, 28.0f}, "-")) {
        pendingFrames = std::max(1, pendingFrames - 1);
        SyncToConfigAndReload(assets);
    }
    UiButton(Rectangle{panelX + 120.0f, panelY + 42.0f, 40.0f, 28.0f}, std::to_string(pendingFrames).c_str());
    if (UiButton(Rectangle{panelX + 165.0f, panelY + 42.0f, 30.0f, 28.0f}, "+")) {
        pendingFrames = std::min(MAX_FRAMES, pendingFrames + 1);
        SyncToConfigAndReload(assets);
    }

    const char* blinkBtnText = editingBlink ? "MODE: BLINK" : "MODE: NORMAL";
    if (UiButton(Rectangle{panelX + 210.0f, panelY + 42.0f, 150.0f, 28.0f}, blinkBtnText)) {
        editingBlink = !editingBlink;
    }

    DrawText("Click a slot to assign or drag & drop image.", 
              static_cast<int>(panelX + 14.0f), static_cast<int>(panelY + 80.0f), 16, Color{170, 170, 180, 255});

    std::vector<Rectangle> slotRects;
    const float startX = panelX + 10.0f;
    const float slotsStartY = panelY + headerH;

    for (int i = 0; i < pendingFrames; ++i) {
        const int col = i % COLUMNS;
        const int row = i / COLUMNS;

        Rectangle r{
            startX + col * (slotW + 8.0f),
            slotsStartY + row * (slotH + 10.0f),
            slotW,
            slotH
        };
        slotRects.push_back(r);

        const auto path = FramePath(i);
        const bool exists = std::filesystem::exists(path);
        
        Color bg;
        if (exists) {
            bg = editingBlink ? Color{75, 45, 95, 255} : Color{45, 75, 45, 255}; 
        } else {
            bg = Color{50, 50, 58, 255};
        }

        DrawRectangleRounded(r, 0.10f, 6, bg);
        DrawRectangleRoundedLines(r, 0.10f, 6, exists ? RAYWHITE : Color{120, 120, 130, 255});

        DrawText(TextFormat("%s %d", editingBlink ? "Blink" : "Frame", i), 
                  static_cast<int>(r.x + 8.0f), static_cast<int>(r.y + 8.0f), 18, RAYWHITE);
        
        std::string label = exists ? BaseName(path) : "Empty";
        if (label.length() > 15) label = label.substr(0, 12) + "...";
        DrawText(label.c_str(), static_cast<int>(r.x + 8.0f), static_cast<int>(r.y + 32.0f), 16, Color{225, 225, 230, 255});
    }

    for (int i = 0; i < pendingFrames; ++i) {
        if (MousePressedOn(slotRects[i])) {
            std::string file;
            if (OpenFileDialog(file)) AssignImageToSlot(i, file, assets);
        }
    }

    if (IsFileDropped()) {
        FilePathList dropped = LoadDroppedFiles();
        for (int i = 0; i < pendingFrames; ++i) {
            if (CheckCollisionPointRec(mouse, slotRects[i])) {
                if (dropped.count > 0) AssignImageToSlot(i, dropped.paths[0], assets);
                break;
            }
        }
        UnloadDroppedFiles(dropped);
    }
}