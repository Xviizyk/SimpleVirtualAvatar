#include "AudioInput.hpp"
#include <iostream>
#include <cmath>

void AudioInput::data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount) {
    AudioInput* self = (AudioInput*)device->pUserData;
    if (!input) return;

    const float* samples = (const float*)input;

    float sum = 0.0f;
    for (ma_uint32 i = 0; i < frameCount; i++) {
        float s = samples[i];
        sum += s * s;
    }

    float rms = sqrt(sum / frameCount);
    self->currentVolume = self->currentVolume * 0.9f + rms * 0.1f;
}

bool AudioInput::init() {
    ma_device_config config = ma_device_config_init(ma_device_type_capture);
    
    config.capture.format = ma_format_f32;
    config.capture.channels = 1;
    config.sampleRate = 44100;
    config.dataCallback = data_callback;
    config.pUserData = this;

    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        std::cout << "Failed to init device\n";
        return false;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        std::cout << "Failed to start device\n";
        return false;
    }

    return true;
}

float AudioInput::get_current_volume() {
    return currentVolume;
}

void AudioInput::shutdown() {
    ma_device_uninit(&device);
}