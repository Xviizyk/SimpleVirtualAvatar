#include "WinUtils.hpp"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    float WinUtils::get_dpi_scale(void* windowHandle) {
        HWND hwnd = (HWND)windowHandle;
        if (!hwnd) return 1.0f;

        UINT dpi = GetDpiForWindow(hwnd);
        return dpi / 96.0f;
    }
    
    bool WinUtils::is_key_down_global(int vKey) {
        return (GetAsyncKeyState(vKey) & 0x8000) != 0;
    }

    bool WinUtils::is_shift_pressed_global() {
        return is_key_down_global(VK_SHIFT) || 
               is_key_down_global(VK_LSHIFT) || 
               is_key_down_global(VK_RSHIFT);
    }

    void WinUtils::set_window_always_on_top(void* windowHandle) {
        HWND hwnd = (HWND)windowHandle;
        if (!hwnd) return;

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW;
        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED
        );
    }

    void WinUtils::force_show(void* windowHandle) {
        HWND hwnd = (HWND)windowHandle;
        if (!hwnd) return;

        if (IsIconic(hwnd)) {
            ShowWindow(hwnd, SW_RESTORE);
        }

        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE
        );
    }

    void WinUtils::set_overlay_mode(void* windowHandle, bool overlay) {
        HWND hwnd = (HWND)windowHandle;
        if (!hwnd) return;

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

        if (overlay) {
            exStyle &= ~WS_EX_APPWINDOW;
            exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT;

            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
            SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

            SetWindowPos(
                hwnd,
                HWND_TOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE
            );
        } else {
            exStyle &= ~(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
            exStyle |= WS_EX_APPWINDOW;

            SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);

            SetWindowPos(
                hwnd,
                HWND_NOTOPMOST,
                0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE
            );
        }
    }

    void WinUtils::set_window_borderless(void* windowHandle, bool borderless) {
        HWND hwnd = (HWND)windowHandle;
        if (!hwnd) return;

        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);

        if (borderless) {
            style &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        } else {
            style |= (WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
        }

        SetWindowLongPtr(hwnd, GWL_STYLE, style);

        SetWindowPos(
            hwnd, nullptr, 
            0, 0, 0, 0, 
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE
        );
    }

    void WinUtils::make_window_ghost(void* windowHandle) {
        HWND hwnd = (HWND)windowHandle;
        if (!hwnd) return;

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
        exStyle |= WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;
        exStyle &= ~WS_EX_APPWINDOW;

        SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
        SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA);

        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_NOACTIVATE
        );
    }

#else
    float WinUtils::get_dpi_scale(void*) { return 1.0f; }
    bool WinUtils::is_key_down_global(int) { return false; }
    bool WinUtils::is_shift_pressed_global() { return false; }
    void WinUtils::set_window_always_on_top(void*) {}
    void WinUtils::force_show(void*) {}
    void WinUtils::set_overlay_mode(void*, bool) {}
    void WinUtils::set_window_borderless(void*, bool) {}
    void WinUtils::make_window_ghost(void*) {}
#endif