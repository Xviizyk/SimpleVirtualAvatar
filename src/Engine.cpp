#include "Engine.hpp"

#ifdef TRACY_ENABLE
    #include <tracy/Tracy.hpp>
#endif

#ifdef _MSVC
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

bool Engine::init() {
    ConfigManager::Load();

    if (!renderer.init()) {
        return false;
    }

    if (!audio.init()) {
        renderer.shutdown();
        return false;
    }

    assets.set_base_path(ConfigManager::GetAssetsDir().string());

    const auto& cfg = ConfigManager::Get();
    assets.load_avatar_state(AvatarState::IDLE, cfg.maxIdleFrames, "idle");
    assets.load_avatar_state(AvatarState::TALKING, cfg.maxTalkFrames, "talk");
    assets.load_avatar_state(AvatarState::SCREAMING, cfg.maxScreamFrames, "scream");

    renderer.set_max_frames(cfg.maxIdleFrames, cfg.maxTalkFrames, cfg.maxScreamFrames);
    renderer.set_avatar_state(AvatarState::IDLE);

    isRunning = true;
    return true;
}

void Engine::process_input() {
    if (renderer.should_close()) {
        isRunning = false;
    }

    if (input.is_interface_toggled()) {
        renderer.toggle_ui_visibility();
    }
}

void Engine::update() {
    const auto& cfg = ConfigManager::Get();

    renderer.set_max_frames(cfg.maxIdleFrames, cfg.maxTalkFrames, cfg.maxScreamFrames);

    current_volume = Utils::smooth_volume(current_volume, audio.rms);
    sensitivity = renderer.get_sensitivity();

    const float final_vol = current_volume * sensitivity;

    AvatarState next_state = AvatarState::IDLE;
    if (final_vol < 0.2f) {
        next_state = AvatarState::IDLE;
    } else if (final_vol < 0.7f) {
        next_state = AvatarState::TALKING;
    } else {
        next_state = AvatarState::SCREAMING;
    }

    if (next_state != current_state) {
        current_state = next_state;
        shake.Trigger(cfg.shakeStrength, cfg.shakeDuration, cfg.shakeMode);
    }

    renderer.set_avatar_state(current_state);
}

void Engine::render() {
    renderer.update(assets, current_volume, menuBar, spriteEditor, shaderEditor, shake);
}

void Engine::run() {
    while (isRunning) {
        {
        #ifdef TRACY_ENABLE
            ZoneScopedN("Process input");
        #endif
            process_input();
        }

        {
        #ifdef TRACY_ENABLE
            ZoneScopedN("Update");
        #endif
            update();
        }

        {
        #ifdef TRACY_ENABLE
            ZoneScopedN("Render");
        #endif
            render();
        }

        #ifdef TRACY_ENABLE
            FrameMark;
        #endif
    }
}

void Engine::shutdown() {
    audio.shutdown();
    assets.clear_all();
    renderer.shutdown();
}