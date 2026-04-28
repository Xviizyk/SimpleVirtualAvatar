#pragma once
#include <iostream>

class WinUtils {
public:
    static float get_dpi_scale(void* windowHandle);
    static void set_window_always_on_top(void* windowHandle);
    static void force_show(void* windowHandle);
    static void set_overlay_mode(void* windowHandle, bool overlay);
    static void make_window_ghost(void* windowHandle);
    static bool is_key_down_global(int vKey);
    void set_window_borderless(void*, bool);
    static bool is_shift_pressed_global();
    static bool open_file_dialog(void* windowHandle, std::string& outPath);
    static std::string get_appdata_path();
};