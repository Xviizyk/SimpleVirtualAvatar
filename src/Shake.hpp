#pragma once

#include "Config.hpp"
#include "raylib.h"

class Shake {
public:
    void Trigger(float strength, float duration, Config::ShakeMode mode);
    void Update(float dt);

    Vector2 GetOffset() const;
    bool IsActive() const;
    void Reset();

private:
    float timeLeft = 0.0f;
    float totalDuration = 0.0f;
    float strength = 0.0f;

    Config::ShakeMode mode = Config::ShakeMode::XY;
    Vector2 offset = {0.0f, 0.0f};

    float Random01();
};