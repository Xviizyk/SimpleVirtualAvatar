#ifndef AUDIO_INPUT_H
#define AUDIO_INPUT_H

#include "miniaudio.h"

class AudioInput {
private:
    ma_device device;
    float currentVolume = 0.0f;
    static void data_callback(ma_device* device, void* output, const void* input, ma_uint32 frameCount);
public:
    bool init();
    float get_current_volume();
    void shutdown();
};

#endif