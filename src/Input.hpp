#pragma once

class Input {
public:
    Input() = default;
    bool is_shift_pressed();
    bool is_interface_toggled();
    float get_volume_sensitivity_change();
};