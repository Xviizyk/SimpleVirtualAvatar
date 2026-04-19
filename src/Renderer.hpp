#ifndef RENDERER_H
#define RENDERER_H

#include "raylib.h"

class Renderer {
private:
    Texture2D frames[100];
public:
    Renderer();
    ~Renderer();
    void set_frame(int frame, Texture2D texture);
    bool init();
    void draw_frame(int frame);
    void draw_console_volume_level(float vol);
};

#endif