#pragma once

#include <string>

namespace OsUtilsWin {
    void setup_overlay(void* nativeHandle);
    void keep_focus(void* nativeHandle);
    void set_window_opacity(void* nativeHandle, float alpha);
    void set_click_through(void* nativeHandle, bool enabled);
    void set_show_in_taskbar(void* nativeHandle, bool show);

    float get_dpi_scale(void* nativeHandle);
    void set_window_always_on_top(void* nativeHandle);
    void force_show(void* nativeHandle);
    void set_overlay_mode(void* nativeHandle, bool overlay);
    void set_window_borderless(void* nativeHandle, bool borderless);
    void make_window_ghost(void* nativeHandle);

    bool is_key_down_global(int raylibKey);
    bool is_shift_pressed_global();

    bool open_file_dialog(void* nativeHandle, std::string& outPath);
    std::string get_appdata_path();
}