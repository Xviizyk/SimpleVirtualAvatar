#include <raylib.h>
#include "OsUtils_win.hpp"

namespace OsUtilsWin {
    float get_dpi_scale(void*) {
        return GetWindowScaleDPI().x;
    }

    void set_overlay_mode(void* hwnd, bool overlay) {
        if (overlay) {
            SetWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);
            OsUtilsWin::setup_overlay(hwnd);
            OsUtilsWin::set_click_through(hwnd, false);
        } else {
            ClearWindowState(FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED);
        }
    }

    void set_window_borderless(void*, bool borderless) {
        if (borderless) SetWindowState(FLAG_WINDOW_UNDECORATED);
        else            ClearWindowState(FLAG_WINDOW_UNDECORATED);
    }
}