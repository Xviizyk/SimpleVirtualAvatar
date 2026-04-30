#include "Input.hpp"
#include "../utils/OsUtils.hpp"
#include "Keys.hpp"

bool Input::is_shift_pressed() {
    return OsUtils::is_shift_pressed_global();
}

bool Input::is_interface_toggled() {
    static bool last_state = false;
    
    bool current_state = OsUtils::is_key_down_global(Key::F11) && is_shift_pressed();

    bool pressed = current_state && !last_state;
    last_state = current_state;
    
    return pressed;
}