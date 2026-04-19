#include <iostream>

#ifdef _WIN32
    #define Rectangle WinRectangle
    #define CloseWindow WinCloseWindow
    #define ShowCursor WinShowCursor
    #define DrawText WinDrawText
    #define DrawTextEx WinDrawTextEx
    #define LoadImage LoadImage

    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>

    #undef Rectangle
    #undef CloseWindow
    #undef ShowCursor
    #undef DrawText
    #undef DrawTextEx
    #undef LoadImage
#endif

#include "raylib.h"

#include "Renderer.hpp"

Renderer::Renderer() {
    for (int i = 0; i < 100; ++i) {
        frames[i].id = 0;
    }
}

Renderer::~Renderer() {
    for (int i = 0; i < 100; ++i) {
        if (frames[i].id > 0) {
            UnloadTexture(frames[i]);
        }
    }
}

void Renderer::set_frame(int frame, Texture2D texture) {
    if (frame >= 0 && frame < 100) {
        frames[frame] = texture;
    } else {
        std::cout << "Error: Index " << frame << " is out of range (0-99)\n";
    }
}

void Renderer::draw_frame(int frame) {
    if (frame < 0 || frame >= 100 || frames[frame].id <= 0) {
        return;
    }

    BeginDrawing();
    ClearBackground(BLANK);
    DrawTexture(frames[frame], 0, 0, WHITE);
    EndDrawing();
}

bool Renderer::init() {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_TOPMOST);
    InitWindow(256, 256, "SimpleVirtualAvatar");

#ifdef _WIN32
    HWND hwnd = (HWND)GetWindowHandle();
    if (hwnd) {
        long style = GetWindowLong(hwnd, GWL_EXSTYLE);
        SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    }
#endif

    return IsWindowReady();
}

void Renderer::draw_console_volume_level(float vol) {
    const int barWidth = 100;
    int pos = static_cast<int>(vol * barWidth);
    
    std::cout << "\033[A\033[A";
    std::cout << "\rVolume: [";
    for (int i = 0; i < barWidth; ++i) {
        std::cout << (i < pos ? "#" : " ");
    }
    std::cout << "] " << static_cast<int>(vol * 100) << "%    \n" << std::flush;
}