#pragma once

#include "raylib.h"

class ShakeSettings {
public:
    ShakeSettings();

    void open_window();
    void close_window();
    void toggle();
    bool is_open() const;

    void draw(float dpi);

    static bool ui_button(const Rectangle& r, const char* text, float dpi, bool selected = false);

private:
    bool open = false;

    Rectangle windowRect = { 100.0f, 100.0f, 260.0f, 320.0f };

    bool isDragging = false;
    Vector2 dragOffset = { 0.0f, 0.0f };
};