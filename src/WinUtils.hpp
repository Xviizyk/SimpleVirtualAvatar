#pragma once

class WinUtils {
public:
    void set_window_always_on_top(void* windowHandle);
    void force_show(void* windowHandle);
    void set_overlay_mode(void* windowHandle, bool overlay);
    void make_window_ghost(void* windowHandle);
    float get_dpi_scale(void* windowHandle);
};