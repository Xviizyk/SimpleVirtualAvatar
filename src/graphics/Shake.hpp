#pragma once

#include "../core/Config.hpp"
#include "raylib.h"

class Shake {
public:
    void trigger(float strength, float duration, Config::ShakeMode mode);
    void update(float dt);

    Vector2 get_offset() const;
    bool is_active() const;
    void reset();

private:
    float timeLeft = 0.0f;
    float totalDuration = 0.0f;
    float strength = 0.0f;

    Config::ShakeMode mode = Config::ShakeMode::XY;
    Vector2 offset = { 0.0f, 0.0f };

    static float random_01();
};