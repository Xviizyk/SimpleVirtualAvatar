#include <raylib.h>
#include "OsUtils_lin.hpp"

namespace OsUtilsLin {
    float get_dpi_scale(void*) {
        Vector2 scale = GetWindowScaleDPI();
        return scale.x;
    }

    bool is_key_down_global(int raylibKey) {
        return IsKeyDown(raylibKey);
    }
    
    bool is_shift_pressed_global() {
        return IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    }

    void set_overlay_mode(void* h, bool overlay) {
        if (overlay) {
            SetWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
            OsUtilsLin::setup_overlay(h);
        } else {
            ClearWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
        }
    }

    void set_window_borderless(void* h, bool borderless) {
        if (borderless) SetWindowState(FLAG_WINDOW_UNDECORATED);
        else ClearWindowState(FLAG_WINDOW_UNDECORATED);
    }
}