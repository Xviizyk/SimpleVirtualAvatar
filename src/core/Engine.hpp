#pragma once

#include "../system/AssetManager.hpp"
#include "../system/AudioInput.hpp"
#include "Config.hpp"
#include "../system/Input.hpp"
#include "../ui/MenuBar.hpp"
#include "../graphics/Renderer.hpp"
#include "../graphics/Shake.hpp"
#include "../ui/ShaderEditor.hpp"
#include "../ui/SpriteEditor.hpp"
#include "../utils/Utils.hpp"

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