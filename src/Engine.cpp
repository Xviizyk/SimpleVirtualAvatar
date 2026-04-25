#include "Engine.hpp"

#ifdef _MSVC
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

bool Engine::init() {
    if (!renderer.init()) {
        return false;
    }
    
    if (!audio.init()) {
        renderer.shutdown();
        return false;
    }
    
    assets.set_base_path("../../img/");
    assets.load_avatar_state(AvatarState::IDLE, IDLE_MAX_FRAMES, "idle");
    assets.load_avatar_state(AvatarState::TALKING, TALK_MAX_FRAMES, "talk");
    assets.load_avatar_state(AvatarState::SCREAMING, SCREAM_MAX_FRAMES, "scream");

    renderer.set_max_frames(IDLE_MAX_FRAMES, TALK_MAX_FRAMES, SCREAM_MAX_FRAMES);

    isRunning = true;
    return true;
}

void Engine::process_input() {
    if (renderer.should_close()) {
        isRunning = false;
    }

    sensitivity += input.get_volume_sensitivity_change();
    
    if (sensitivity < 0.5f) sensitivity = 0.1f;
    if (sensitivity > 5.0f) sensitivity = 100.0f;
    
    if (input.is_interface_toggled()) {
        renderer.toggle_ui_visibility();
    }
}

void Engine::update() {
    current_volume = Utils::smooth_volume(current_volume, audio.rms);
    
    float final_vol = current_volume * sensitivity;

    if (final_vol < 0.2f) {
        current_state = AvatarState::IDLE;
    } else if (final_vol < 0.7f) {
        current_state = AvatarState::TALKING;
    } else {
        current_state = AvatarState::SCREAMING;
    }
    
    renderer.set_avatar_state(current_state);
}

void Engine::render() {
    renderer.draw_avatar(assets, current_volume, sensitivity);
}

void Engine::run() {
    while (isRunning) {
        process_input();
        update();
        render();
    }
}

void Engine::shutdown() {
    audio.shutdown();
    assets.clear_all();
    renderer.shutdown();
}
