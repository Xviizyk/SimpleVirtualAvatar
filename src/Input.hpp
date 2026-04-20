#pragma once

#include "raylib.h"

class Input {
public:
    bool is_interface_toggled() {
        return IsKeyPressed(KEY_F11);
    }

    float is_volume_sensitivity_change() {
        return change_volume_sensitivity();
    }
    
    float change_volume_sensitivity() {
        bool plus = IsKeyPressed(KEY_F9);
        
        bool minus = IsKeyPressed(KEY_F10);

        if (minus) return -0.1f;
        else if (plus) return 0.1f;

        return 0.0f;
    }
};
