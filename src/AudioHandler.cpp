#include "AudioHandler.hpp"

void AudioHandler::update_blink(float dt) {
    this->blinkTimer += dt;
    if (this->blinkTimer > 2.0f) {
        this->isBlinking = !this->isBlinking;
        this->blinkTimer = 0.0f;
    }
}

void AudioHandler::update(float vol, float dt) { 
    this->volume = vol;
    
    update_blink(dt); 
    
    if (volume > screamThreshold) currentState = State::Screaming;
    else if (volume > talkThreshold) currentState = State::Talking;
    else currentState = State::Idle; 
}

int AudioHandler::get_active_sprite_frame() {
    if (isBlinking) return 3;
    return static_cast<int>(currentState);
}

bool AudioHandler::is_open() {
    return true;
}

float AudioHandler::get_delta_time() {
    return 1.0f / 60.0f;
}