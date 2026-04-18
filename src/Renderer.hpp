#ifndef RENDERER_H
#define RENDERER_H

class Renderer {
public:
    Renderer();
    ~Renderer();
    bool init();
    void draw(int frame);
    void draw_console_volume_level(float vol);
};

#endif