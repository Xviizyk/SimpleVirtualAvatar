#include "AudioHandler.hpp"
#include "AudioInput.hpp"
#include "Renderer.hpp"
#include "raylib.h"
#include <chrono>

int main() {    
    AudioInput audio;
    AudioHandler work;
    Renderer render;
    
    if (!audio.init()) {
        return -1;
    }

    if (!render.init()) {
        return -1;
    }

    Image image = LoadImage("../../img/idle.png");
    if (image.data) {
        Texture2D texture = LoadTextureFromImage(image);
        render.set_frame(0, texture);
        UnloadImage(image);
    }

    image = LoadImage("../../img/speaking.png");
    if (image.data) {
        Texture2D texture = LoadTextureFromImage(image);
        render.set_frame(10, texture);
        UnloadImage(image);
    }

    image = LoadImage("../../img/speaking_blink.png");
    if (image.data) {
        Texture2D texture = LoadTextureFromImage(image);
        render.set_frame(20, texture);
        UnloadImage(image);
    }

    image = LoadImage("../../img/idle_blink.png");
    if (image.data) {
        Texture2D texture = LoadTextureFromImage(image);
        render.set_frame(50, texture);
        UnloadImage(image);
    }

    image = LoadImage("../../img/scream.png");
    if (image.data) {
        Texture2D texture = LoadTextureFromImage(image);
        render.set_frame(60, texture);
        UnloadImage(image);
    }

    image = LoadImage("../../img/scream_blink.png");
    if (image.data) {
        Texture2D texture = LoadTextureFromImage(image);
        render.set_frame(70, texture);
        UnloadImage(image);
    }

    while (!WindowShouldClose() && work.is_open()) {
        float dt = work.get_delta_time();
        float vol = audio.get_current_volume();
        
        work.update(vol, dt);
        
        int frame = work.get_active_sprite_frame();
        render.draw_frame(frame);
        render.draw_console_volume_level(vol);
    }

    audio.shutdown();
    return 0;
}