#pragma once

class Animation {
private:
    float blink_timer = 0.0f;
    bool is_blinking = false;
    
    static constexpr float BLINK_START_TIME = 2.0f;
    static constexpr float BLINK_END_TIME = 2.2f;
    
public:
    void update_blink_timer(float dt);
    bool is_blink() const;
    void reset_blink_timer();
};
