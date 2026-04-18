#include "Renderer.hpp"
#include <iostream>

void Renderer::draw(int frame) {
    std::cout << "Rendering frame: " << frame << std::endl;
}

void Renderer::draw_console_volume_level(float vol) {
    int barWidth = 50;
    int pos = static_cast<int>(vol * barWidth * 5);

    std::cout << "\r[";
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "#";
        else std::cout << " ";
    }
    std::cout << "] " << vol << std::flush;
}