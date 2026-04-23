#include "Input.hpp"
#include "WinUtils.hpp"

#ifdef _WIN32
    #include <windows.h>
#else
    #define VK_F9 0
    #define VK_F10 0
    #define VK_F11 0
#endif

bool Input::is_shift_pressed() {
    return WinUtils::is_shift_pressed_global();
}

bool Input::is_interface_toggled() {
    static bool last_state = false;
    bool current_state = WinUtils::is_key_down_global(VK_F11) && is_shift_pressed();

    bool pressed = current_state && !last_state;
    last_state = current_state;
    
    return pressed;
}

float Input::get_volume_sensitivity_change() {
    bool plus = WinUtils::is_key_down_global(VK_F10) && is_shift_pressed();
    bool minus = WinUtils::is_key_down_global(VK_F9) && is_shift_pressed();

    if (minus) return -0.1f;
    if (plus) return 0.1f;

    return 0.0f;
}