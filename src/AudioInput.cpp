#include "AudioInput.hpp"
#include <iostream>
#include <cmath>

void AudioInput::data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    AudioInput* self = static_cast<AudioInput*>(device->pUserData);
    if (!input) return;

    const float* samples = static_cast<const float*>(input);

    float sum = 0.0f;
    for (ma_uint32 i = 0; i < frameCount; ++i) {
        sum += samples[i] * samples[i];
    }

    self->rms = std::sqrt(sum / frameCount);
}

bool AudioInput::init() {
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = 44100;
    config.dataCallback = data_callback;
    config.pUserData = this;

    if (ma_device_init(nullptr, &config, &device) != MA_SUCCESS) {
        std::cout << "Failed to initialize audio device\n";
        return false;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start audio device\n";
        ma_device_uninit(&device);
        return false;
    }

    return true;
}

void AudioInput::shutdown() {
    ma_device_uninit(&device);
}