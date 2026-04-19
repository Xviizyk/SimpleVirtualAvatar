#pragma once

#include "raylib.h"

class Input {
public:
    bool is_interface_toggled() {
        return IsKeyPressed(KEY_LEFT_SHIFT) &&
               IsKeyDown(KEY_LEFT_ALT) && 
               IsKeyPressed(KEY_D);
    }
    
    bool change_interface_visibility() {
        return IsKeyDown(KEY_LEFT_SHIFT) &&
               IsKeyDown(KEY_LEFT_ALT) && 
               IsKeyDown(KEY_D);
    }

    float get_volume_sensitivity_change() {
        return change_volume_sensitivity();
    }
    
    float change_volume_sensitivity() {
        bool plus = IsKeyPressed(KEY_LEFT_SHIFT) &&
                    IsKeyDown(KEY_LEFT_ALT) &&
                    IsKeyPressed(KEY_EQUAL);
        
        bool minus = IsKeyPressed(KEY_LEFT_SHIFT) &&
                     IsKeyDown(KEY_LEFT_ALT) &&
                     IsKeyPressed(KEY_MINUS);

        if (minus) return -0.1f;
        else if (plus) return 0.1f;

        return 0.0f;
    }
};
