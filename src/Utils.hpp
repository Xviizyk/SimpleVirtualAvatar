#pragma once

#include <string>

namespace Utils {
    float get_delta_time();
    float smooth_volume(float current_vol, float target_rms);
    std::string get_name_of_state_by_number(int num);
}