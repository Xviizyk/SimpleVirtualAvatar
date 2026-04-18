#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

enum class State { Idle, Talking, Screaming };

class AudioHandler {
private:
    State currentState = State::Idle;
    float volume = 0.0f;
    float talkThreshold = 0.2f;
    float screamThreshold = 0.8f;
    bool isBlinking = false;
    float blinkTimer = 0.0f;

public:
    void update_blink(float dt);
    void update(float vol, float dt);
    int get_active_sprite_frame();
    bool is_open();
    float get_delta_time();
};

#endif