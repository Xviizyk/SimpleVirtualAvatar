#include "Animation.hpp"

void Animation::update(float dt) {
    if (!is_blinking) {
        blink_timer += dt;
        if (blink_timer >= BLINK_INTERVAL) {
            is_blinking = true;
            blink_timer = 0.0f;
        }
    } else {
        blink_timer += dt;
        if (blink_timer >= BLINK_DURATION) {
            is_blinking = false;
            blink_timer = 0.0f;
        }
    }

    frame_timer += dt;
    if (frame_timer >= FRAME_DURATION) {
        frame_timer -= FRAME_DURATION;
        current_frame++;
        if (current_frame >= max_frames) {
            current_frame = 0;
        }
    }
}

bool Animation::is_blink() const {
    return is_blinking;
}

void Animation::reset_blink_timer() {
    blink_timer = 0.0f;
    is_blinking = false;
}

int Animation::get_current_frame() const {
    return current_frame;
}

void Animation::set_max_frames(int frames) {
    max_frames = frames;
    if (max_frames < 1) max_frames = 1;
    current_frame = 0;
    frame_timer = 0.0f;
}

void Animation::reset_frame() {
    current_frame = 0;
    frame_timer = 0.0f;
}