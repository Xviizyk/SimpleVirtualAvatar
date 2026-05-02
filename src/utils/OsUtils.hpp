#pragma once

#include <string>

class OsUtils {
public:
    static void setup_overlay(void* nativeHandle = nullptr);
    static void keep_focus(void* nativeHandle = nullptr);
    static void set_window_opacity(void* nativeHandle, float alpha);
    static void set_click_through(void* nativeHandle, bool enabled);
    static void set_show_in_taskbar(void* nativeHandle, bool show);
    static float get_dpi_scale(void* nativeHandle = nullptr);
    static void set_window_always_on_top(void* nativeHandle = nullptr);
    static void force_show(void* nativeHandle = nullptr);
    static void set_overlay_mode(void* nativeHandle, bool overlay);
    static void set_window_borderless(void* nativeHandle, bool borderless);
    static void make_window_ghost(void* nativeHandle = nullptr);
    static bool is_key_down_global(int raylibKey);
    static bool is_shift_pressed_global();
    static bool open_file_dialog(void* nativeHandle, std::string& outPath);
    static std::string get_appdata_path();
};