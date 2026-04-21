#pragma once

#include "raylib.h"

class Input {
public:
    bool is_interface_toggled() {
        return IsKeyPressed(KEY_F11) &&
               IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    }

    float is_volume_sensitivity_change() {
        return change_volume_sensitivity();
    }
    
    float change_volume_sensitivity() {
        bool plus = IsKeyPressed(KEY_F10) &&
               IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
        
        bool minus = IsKeyPressed(KEY_F9) &&
               IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);

        if (minus) return -0.1f;
        else if (plus) return 0.1f;

        return 0.0f;
    }
};
