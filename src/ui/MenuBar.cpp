#include "MenuBar.hpp"
#include "raylib.h"

#include <algorithm>
#include <array>
#include <string>

namespace {
    constexpr float BAR_H = 30.0f;
    constexpr float MENU_W = 120.0f;
    constexpr float ITEM_H = 26.0f;

    static void DrawMenuItem(Rectangle r, const char* text, bool hovered) {
        DrawRectangleRec(r, hovered ? Color{70, 70, 80, 255} : Color{50, 50, 60, 255});
        DrawRectangleLinesEx(r, 1.0f, Color{120, 120, 130, 255});

        const int fs = 18;
        const int tw = MeasureText(text, fs);
        DrawText(text, static_cast<int>(r.x + (r.width - tw) / 2.0f), static_cast<int>(r.y + 4.0f), fs, RAYWHITE);
    }
}   

MenuBarAction MenuBar::draw() {
    MenuBarAction action{};

    const int sw = GetScreenWidth();
    const Vector2 mouse = GetMousePosition();

    DrawRectangle(0, 0, sw, static_cast<int>(BAR_H), Color{35, 35, 42, 255});
    DrawLine(0, static_cast<int>(BAR_H) - 1, sw, static_cast<int>(BAR_H) - 1, Color{90, 90, 100, 255});

    const std::array<const char*, 3> topLabels = {"Sprite", "Settings", "Shader"};
    Rectangle topRects[3]{};
    static int openMenu = -1;

    float x = 8.0f;
    bool isOverAnyHeader = false;

    for (int i = 0; i < 3; ++i) {
        const int fs = 18;
        const float w = std::max(MENU_W, static_cast<float>(MeasureText(topLabels[i], fs) + 24));
        topRects[i] = Rectangle{x, 2.0f, w, BAR_H - 4.0f};

        if (CheckCollisionPointRec(mouse, topRects[i])) {
            openMenu = i;
            isOverAnyHeader = true;
        }

        bool isActive = (openMenu == i);
        DrawRectangleRec(topRects[i], isActive ? Color{70, 70, 85, 255} : Color{45, 45, 54, 255});
        DrawRectangleLinesEx(topRects[i], 1.0f, Color{100, 100, 110, 255});
        
        int tw = MeasureText(topLabels[i], fs);
        DrawText(topLabels[i], static_cast<int>(x + (w - tw) / 2.0f), 5, fs, RAYWHITE);

        x += w + 6.0f;
    }

    auto draw_dropdown = [&](int idx, const std::array<const char*, 3>& items, int itemCount) {
        Rectangle head = topRects[idx];
        Rectangle menuRect{head.x, head.y + head.height, 240.0f, ITEM_H * itemCount};

        bool overMenu = CheckCollisionPointRec(mouse, menuRect);
        
        if (!isOverAnyHeader && !overMenu) {
            openMenu = -1;
            return;
        }

        DrawRectangleRec(menuRect, Color{42, 42, 50, 255});
        DrawRectangleLinesEx(menuRect, 1.0f, Color{115, 115, 125, 255});

        for (int i = 0; i < itemCount; ++i) {
            Rectangle itemRect{menuRect.x, menuRect.y + ITEM_H * i, menuRect.width, ITEM_H};
            bool itemHovered = CheckCollisionPointRec(mouse, itemRect);

            DrawMenuItem(itemRect, items[i], itemHovered);

            if (itemHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if (idx == 0) {
                    if (i == 0) action.openIdleEditor = true;
                    if (i == 1) action.openTalkEditor = true;
                    if (i == 2) action.openScreamEditor = true;
                } else if (idx == 1) {
                    action.openShakeSettings = true;
                } else if (idx == 2) {
                    action.openShaderEditor = true;
                }
                openMenu = -1;
            }
        }
    };

    if (openMenu == 0) draw_dropdown(0, {"Change idle sprites", "Change talk sprites", "Change scream sprites"}, 3);
    else if (openMenu == 1) draw_dropdown(1, {"Shake settings", "", ""}, 1);
    else if (openMenu == 2) draw_dropdown(2, {"Open shader editor", "", ""}, 1);

    return action;
}