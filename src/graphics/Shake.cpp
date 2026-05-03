#include "Shake.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

/*static*/ float Shake::random_01() {
    return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

void Shake::trigger(float s, float duration, Config::ShakeMode m) {
    strength = s;
    totalDuration = (duration > 0.0f) ? duration : 0.0f;
    timeLeft = totalDuration;
    mode = m;
}

void Shake::update(float dt) {
    const Config& cfg = ConfigManager::get();

    offset = { 0.0f, 0.0f };

    if (timeLeft > 0.0f) {
        timeLeft -= dt;

        if (timeLeft > 0.0f && totalDuration > 0.0f) {
            const float t   = std::clamp(timeLeft / totalDuration, 0.0f, 1.0f);
            const float amp = strength * t;

            float x = (random_01() * 2.0f - 1.0f) * amp;
            float y = (random_01() * 2.0f - 1.0f) * amp;

            switch (mode) {
                case Config::ShakeMode::X:  y = 0.0f; break;
                case Config::ShakeMode::Y:  x = 0.0f; break;
                case Config::ShakeMode::XY: break;
            }

            offset.x += x;
            offset.y += y;
        } else {
            timeLeft = 0.0f;
        }
    }

    if (cfg.wiggleEnabled) {
        const float t = static_cast<float>(GetTime());
        const float spd = cfg.wiggleSpeed * 2.0f * 3.1415927f;
        offset.x += std::sin(t * spd) * cfg.wiggleAmplitude;
        offset.y += std::sin(t * spd * 1.37f) * cfg.wiggleAmplitude * 0.6f;
    }
}

Vector2 Shake::get_offset() const {
    return offset;
}

bool Shake::is_active() const {
    return timeLeft > 0.0f;
}

void Shake::reset() {
    timeLeft      = 0.0f;
    totalDuration = 0.0f;
    strength      = 0.0f;
    offset        = { 0.0f, 0.0f };
}