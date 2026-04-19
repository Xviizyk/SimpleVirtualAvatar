#include "AudioHandler.hpp"
#include "Renderer.hpp"

void AudioHandler::update_blink(float dt) {
    blinkTimer += dt;
    if (blinkTimer >= 2.0f) {
        isBlinking = !isBlinking;
        blinkTimer = 0.0f;
    }
}

void AudioHandler::update(float vol, float dt) { 
    volume = vol;
    
    update_blink(dt); 
    
    if (volume > screamThreshold) {
        currentState = State::Screaming;
    } else if (volume > talkThreshold) {
        currentState = State::Talking;
    } else {
        currentState = State::Idle;
    }
}

int AudioHandler::get_active_sprite_frame() {
    int baseFrame = 0;

    switch (currentState) {
        case State::Idle:      baseFrame = 0;  break;
        case State::Talking:   baseFrame = 10; break;
        case State::Screaming: baseFrame = 60; break;
    }

    if (isBlinking) {
        baseFrame += 10;
    }

    return baseFrame;
}

bool AudioHandler::is_open() {
    return true;
}

float AudioHandler::get_delta_time() {
    return GetFrameTime();
}