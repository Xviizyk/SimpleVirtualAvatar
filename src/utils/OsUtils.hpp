#pragma once
#include <string>

class OsUtils {
public:
    static float get_dpi_scale(void* windowHandle = nullptr);
    static void set_window_always_on_top(void* windowHandle = nullptr);
    static void force_show(void* windowHandle = nullptr);
    static void set_overlay_mode(void* windowHandle = nullptr, bool overlay = true);
    static void set_window_borderless(void* windowHandle = nullptr, bool borderless = true);
    static void make_window_ghost(void* windowHandle = nullptr);
    static bool is_key_down_global(int raylibKey);
    static bool is_shift_pressed_global();
    static bool open_file_dialog(void* windowHandle, std::string& outPath);
    static std::string get_appdata_path();
};