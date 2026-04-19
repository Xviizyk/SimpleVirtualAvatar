#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include "miniaudio.h"

class AudioInput {
private:
    ma_device device;
    static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
public:
    float rms = 0.0f;

    bool init();
    void shutdown();
};

#endif