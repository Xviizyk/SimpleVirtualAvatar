#include "Shake.hpp"

#include <algorithm>
#include <cstdlib>

float Shake::Random01() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

void Shake::Trigger(float s, float duration, Config::ShakeMode m) {
    strength = s;
    totalDuration = (duration > 0.0f) ? duration : 0.0f;
    timeLeft = totalDuration;
    mode = m;
}

void Shake::Update(float dt) {
    if (timeLeft <= 0.0f) {
        offset = {0.0f, 0.0f};
        return;
    }

    timeLeft -= dt;
    if (timeLeft <= 0.0f || totalDuration <= 0.0f) {
        offset = {0.0f, 0.0f};
        timeLeft = 0.0f;
        return;
    }

    const float t = std::clamp(timeLeft / totalDuration, 0.0f, 1.0f);
    const float amp = strength * t;

    float x = (Random01() * 2.0f - 1.0f) * amp;
    float y = (Random01() * 2.0f - 1.0f) * amp;

    switch (mode) {
        case Config::ShakeMode::X:  y = 0.0f; break;
        case Config::ShakeMode::Y:  x = 0.0f; break;
        case Config::ShakeMode::XY: break;
    }

    offset = {x, y};
}

Vector2 Shake::GetOffset() const {
    return offset;
}

bool Shake::IsActive() const {
    return timeLeft > 0.0f;
}

void Shake::Reset() {
    timeLeft = 0.0f;
    totalDuration = 0.0f;
    strength = 0.0f;
    offset = {0.0f, 0.0f};
}