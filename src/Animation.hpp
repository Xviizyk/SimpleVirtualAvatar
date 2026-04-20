#pragma once

class Animation {
private:
    float blink_timer = 0.0f;
    bool is_blinking = false;
    
    float frame_timer = 0.0f;
    int current_frame = 0;
    int max_frames = 1;
    
    static constexpr float BLINK_INTERVAL = 2.0f;
    static constexpr float BLINK_DURATION = 0.2f;
    static constexpr float FRAME_DURATION = 0.1f;
    
public:
    void update(float dt);
    bool is_blink() const;
    void reset_blink_timer();
    int get_current_frame() const;
    void set_max_frames(int frames);
    void reset_frame();
};