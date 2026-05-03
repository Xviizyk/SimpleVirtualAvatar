#include "ShakeSettings.hpp"
#include "../core/Config.hpp"
#include <algorithm>

namespace {
    bool ui_btn(const Rectangle& r, const char* text, float dpi, bool selected = false) {
        const float s = dpi;

        const bool hovered = CheckCollisionPointRec(GetMousePosition(), r);

        Color fill = selected ? Color{ 80, 90, 120, 255 } : hovered ? Color{ 75, 75, 90, 255 } : Color{ 55, 55, 65, 255 };

        DrawRectangleRec(r, fill);
        DrawRectangleLinesEx(r, 1.0f * s, Color{ 120, 120, 130, 255 });

        const int fs = static_cast<int>(15 * s);
        const int tw = MeasureText(text, fs);

        DrawText(text,
                 static_cast<int>(r.x + (r.width - tw) / 2.0f),
                 static_cast<int>(r.y + (r.height - fs) / 2.0f),
                 fs, RAYWHITE);

        return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    bool minus_plus(float bx, float by, float& value, float step, float minV, float maxV, float dpi) {
        const float s = dpi;
        bool changed = false;

        if (ui_btn({ bx, by, 26.0f, 22.0f }, "-", s)) { value = std::max(minV, value - step); changed = true; }
        if (ui_btn({ bx + 28.f, by, 26.0f, 22.0f }, "+", s)) { value = std::min(maxV, value + step); changed = true; }

        return changed;
    }

    void section_divider(float x, float y, float w) {
        DrawRectangle(static_cast<int>(x), static_cast<int>(y),
        static_cast<int>(w), 1, Color{ 55, 55, 68, 255 });
    }
}

ShakeSettings::ShakeSettings() = default;

void ShakeSettings::open_window() { open = true; }
void ShakeSettings::close_window() { open = false; }
void ShakeSettings::toggle() { open = !open; }
bool ShakeSettings::is_open() const { return open; }

/*static*/ bool ShakeSettings::ui_button(const Rectangle& r, const char* text, float dpi, bool selected) {
    const float s = dpi;
    return ui_btn(r, text, s, selected);
}

void ShakeSettings::draw(float dpi) {
    if (!open) return;

    const float s = dpi;

    Config& cfg = ConfigManager::get();
    Vector2 mouse = GetMousePosition();

    Rectangle headerRect = { windowRect.x * s, windowRect.y * s, windowRect.width * s, 30.0f * s };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(mouse, headerRect)) {
        isDragging = true;
        dragOffset = { mouse.x - windowRect.x, mouse.y - windowRect.y };
    }

    if (isDragging) {
        windowRect.x = mouse.x - dragOffset.x;
        windowRect.y = mouse.y - dragOffset.y;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isDragging = false;
    }

    const float x = windowRect.x * s;
    const float y = windowRect.y * s;
    const float w = windowRect.width * s;
    const float h = windowRect.height * s;

    DrawRectangleRec({ x, y, w, h }, { 28, 28, 32, 255 });
    DrawRectangleLinesEx({ x, y, w, h }, 1.0f * s, { 90, 90, 100, 255 });

    DrawRectangleRec(headerRect,
                     isDragging ? Color{ 60, 60, 70, 255 } : Color{ 45, 45, 50, 255 });

    DrawText("Shake Settings",
             static_cast<int>(x + 12.0f * s),
             static_cast<int>(y + 7.0f * s),
             static_cast<int>(18 * s), RAYWHITE);

    if (ui_btn({ x + w - 32.0f * s, y + 4.0f * s, 24.0f * s, 22.0f * s }, "X", s)) {
        open = false;
        return;
    }

    const float LX = 12.0f * s;
    const float CTL = 118.0f * s;
    const float VX = 178.0f * s;
    const float ROW = 36.0f * s;

    float ry = y + 42.0f * s;

    DrawText("Strength",
             static_cast<int>(x + LX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), LIGHTGRAY);

    if (minus_plus(x + CTL, ry, cfg.shakeStrength, 0.5f, 0.0f, 30.0f, s)) ConfigManager::save();

    DrawText(TextFormat("%.1f", cfg.shakeStrength),
             static_cast<int>(x + VX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), Color{ 200, 200, 200, 255 });

    ry += ROW;

    DrawText("Duration",
             static_cast<int>(x + LX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), LIGHTGRAY);

    if (minus_plus(x + CTL, ry, cfg.shakeDuration, 0.05f, 0.0f, 3.0f, s)) ConfigManager::save();

    DrawText(TextFormat("%.2f s", cfg.shakeDuration),
             static_cast<int>(x + VX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), Color{ 200, 200, 200, 255 });

    ry += ROW;

    DrawText("Mode",
             static_cast<int>(x + LX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), LIGHTGRAY);

    if (ui_btn({ x + CTL, ry, 38.0f * s, 22.0f * s }, "X", s, cfg.shakeMode == Config::ShakeMode::X)) cfg.shakeMode = Config::ShakeMode::X; ConfigManager::save();
    if (ui_btn({ x + CTL + 42.f * s, ry, 38.0f * s, 22.0f * s }, "Y", s, cfg.shakeMode == Config::ShakeMode::Y)) cfg.shakeMode = Config::ShakeMode::Y; ConfigManager::save();
    if (ui_btn({ x + CTL + 84.f * s, ry, 50.0f * s, 22.0f * s }, "X+Y", s, cfg.shakeMode == Config::ShakeMode::XY)) cfg.shakeMode = Config::ShakeMode::XY; ConfigManager::save();
    ry += ROW;

    section_divider(x + LX, ry, w - LX * 2.0f);
    ry += 10.0f * s;

    const bool wiggleOn = cfg.wiggleEnabled;

    if (ui_btn({ x + LX, ry, 90.0f * s, 26.0f * s }, "Wiggle", s, wiggleOn)) {
        cfg.wiggleEnabled = !cfg.wiggleEnabled;
        ConfigManager::save();
    }

    const char* wigLabel = wiggleOn ? "ON" : "OFF";
    Color wigLabelColor = wiggleOn ? Color{ 130, 210, 130, 255 } : Color{ 100, 100, 120, 255 };

    DrawText(wigLabel,
             static_cast<int>(x + LX + 96.0f * s),
             static_cast<int>(ry + 6.0f * s),
             static_cast<int>(12 * s), wigLabelColor);

    ry += 34.0f * s;

    const Color paramColor = wiggleOn ? LIGHTGRAY : Color{ 70, 70, 80, 255 };
    const Color valColor = wiggleOn ? Color{ 200, 200, 200, 255 } : Color{ 80, 80, 95, 255 };

    DrawText("W.Speed",
             static_cast<int>(x + LX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), paramColor);

    if (wiggleOn) {
        if (minus_plus(x + CTL, ry, cfg.wiggleSpeed, 0.5f, 0.5f, 20.0f, s)) ConfigManager::save();
    } else {
        DrawRectangleRec({ x + CTL, ry, 26.0f * s, 22.0f * s }, { 40, 40, 48, 255 });
        DrawRectangleLinesEx({ x + CTL, ry, 26.0f * s, 22.0f * s }, 1.0f * s, { 60, 60, 70, 255 });
        DrawRectangleRec({ x + CTL + 28.f * s, ry, 26.0f * s, 22.0f * s }, { 40, 40, 48, 255 });
        DrawRectangleLinesEx({ x + CTL + 28.f * s, ry, 26.0f * s, 22.0f * s }, 1.0f * s, { 60, 60, 70, 255 });
    }

    DrawText(TextFormat("%.1f Hz", cfg.wiggleSpeed),
             static_cast<int>(x + VX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), valColor);

    ry += ROW;

    DrawText("W.Amp",
             static_cast<int>(x + LX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), paramColor);

    if (wiggleOn) {
        if (minus_plus(x + CTL, ry, cfg.wiggleAmplitude, 0.5f, 0.5f, 30.0f, s)) ConfigManager::save();
    } else {
        DrawRectangleRec({ x + CTL, ry, 26.0f * s, 22.0f * s }, { 40, 40, 48, 255 });
        DrawRectangleLinesEx({ x + CTL, ry, 26.0f * s, 22.0f * s }, 1.0f * s, { 60, 60, 70, 255 });
        DrawRectangleRec({ x + CTL + 28.f * s, ry, 26.0f * s, 22.0f * s }, { 40, 40, 48, 255 });
        DrawRectangleLinesEx({ x + CTL + 28.f * s, ry, 26.0f * s, 22.0f * s }, 1.0f * s, { 60, 60, 70, 255 });
    }

    DrawText(TextFormat("%.1f px", cfg.wiggleAmplitude),
             static_cast<int>(x + VX),
             static_cast<int>(ry + 3.0f * s),
             static_cast<int>(15 * s), valColor);

    ry += ROW + 4.0f * s;

    section_divider(x + LX, ry, w - LX * 2.0f);

    DrawText("Shake triggers on avatar state change.",
             static_cast<int>(x + LX),
             static_cast<int>(ry + 6.0f * s),
             static_cast<int>(12 * s),
             Color{ 90, 90, 108, 255 });
}