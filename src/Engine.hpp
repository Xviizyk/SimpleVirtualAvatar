#pragma once

#include "AssetManager.hpp"
#include "AudioInput.hpp"
#include "Config.hpp"
#include "Input.hpp"
#include "MenuBar.hpp"
#include "Renderer.hpp"
#include "Shake.hpp"
#include "ShaderEditor.hpp"
#include "SpriteEditor.hpp"
#include "Utils.hpp"

class Engine {
private:
    AssetManager assets;
    AudioInput audio;
    Animation anim;
    Renderer renderer;
    Input input;

    MenuBar menuBar;
    SpriteEditor spriteEditor;
    ShaderEditor shaderEditor;
    Shake shake;

    bool isRunning = false;
    float current_volume = 0.0f;
    float sensitivity = 3.0f;
    AvatarState current_state = AvatarState::IDLE;

    void process_input();
    void update();
    void render();

public:
    bool init();
    void run();
    void shutdown();
};