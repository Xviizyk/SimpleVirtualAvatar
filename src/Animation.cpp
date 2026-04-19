#include "Animation.hpp"

void Animation::update_blink_timer(float dt) {
    blink_timer += dt;
    
    is_blinking = (blink_timer >= BLINK_START_TIME && blink_timer < BLINK_END_TIME);
    
    if (blink_timer >= 3.5f) {
        blink_timer = 0.0f;
    }
}

bool Animation::is_blink() const {
    return is_blinking;
}

void Animation::reset_blink_timer() {
    blink_timer = 0.0f;
    is_blinking = false;
}
