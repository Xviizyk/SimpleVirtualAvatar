#include "Utils.hpp"
#include "raylib.h"

namespace Utils {

    float get_delta_time() {
        return GetFrameTime();
    }

    float smooth_volume(float current_vol, float target_rms) {
        return current_vol * 0.9f + target_rms * 0.1f;
    }

    std::string get_name_of_state_by_number(int num) {
        switch (num) {
            case 0:  return "Idle";
            case 1:  return "Talk";
            case 2:  return "Scream";
            default: return "";
        }
    }

}