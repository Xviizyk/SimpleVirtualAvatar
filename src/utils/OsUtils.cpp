#include "OsUtils.hpp"
#include <raylib.h>
#include <nfd.h>
#include <cstdlib>
#include <filesystem>
#include <iostream>

static std::string get_env_var(const char* key) {
    const char* val = std::getenv(key);
    return val ? std::string(val) : "";
}

float OsUtils::get_dpi_scale(void*) {
    Vector2 scale = GetWindowScaleDPI();
    return scale.x;
}

void OsUtils::set_window_always_on_top(void*) {
    SetWindowState(FLAG_WINDOW_TOPMOST);
}

void OsUtils::force_show(void*) {
    ClearWindowState(FLAG_WINDOW_MINIMIZED | FLAG_WINDOW_HIDDEN);
    SetWindowState(FLAG_WINDOW_TOPMOST);
}

void OsUtils::set_overlay_mode(void*, bool overlay) {
    if (overlay) {
        SetWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
    } else {
        ClearWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
    }
}

void OsUtils::set_window_borderless(void*, bool borderless) {
    if (borderless) {
        SetWindowState(FLAG_WINDOW_UNDECORATED);
    } else {
        ClearWindowState(FLAG_WINDOW_UNDECORATED);
    }
}

void OsUtils::make_window_ghost(void*) {
    SetWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST | FLAG_WINDOW_MOUSE_PASSTHROUGH);
}

bool OsUtils::is_key_down_global(int raylibKey) {
    return IsKeyDown(raylibKey);
}

bool OsUtils::is_shift_pressed_global() {
    return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
}

bool OsUtils::open_file_dialog(void*, std::string& outPath) {
    nfdchar_t* outPathC = nullptr;
    nfdfilteritem_t filters[] = { { "Image Files", "png,jpg,jpeg,bmp,tga" } };
    
    nfdresult_t result = NFD_OpenDialog(&outPathC, filters, 1, nullptr);
    
    if (result == NFD_OKAY) {
        outPath = outPathC;
        NFD_FreePath(outPathC);
        return true;
    }
    
    if (result == NFD_ERROR) {
        std::cerr << "NFD Error: " << NFD_GetError() << std::endl;
    }
    
    return false;
}

std::string OsUtils::get_appdata_path() {
    std::string path;
    
#if defined(_WIN32)
    path = get_env_var("APPDATA");
#elif defined(__APPLE__)
    path = get_env_var("HOME") + "/Library/Application Support";
#else
    path = get_env_var("XDG_CONFIG_HOME");
    if (path.empty()) {
        path = get_env_var("HOME") + "/.config";
    }
#endif
    if (path.empty() || path == "/Library/Application Support" || path == "/.config") {
        return ".";
    }
    
    return path;
}