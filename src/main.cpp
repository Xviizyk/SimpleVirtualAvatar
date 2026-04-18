#include "AudioHandler.hpp"
#include "AudioInput.hpp"
#include "Renderer.hpp"
#include <thread>
#include <chrono>

int main(int argc, char* argv[]) {
    AudioInput audio;
    AudioHandler work;
    Renderer render;
    
    if (!audio.init()) {
        return -1;
    }

    while (work.is_open()) {
        float dt = work.get_delta_time();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        float vol = audio.get_current_volume();
        
        work.update(vol, dt);
        
        int frame = work.get_active_sprite_frame();
        // render.draw(frame);
        render.draw_volume_level(vol);
    }

    return 0;
}