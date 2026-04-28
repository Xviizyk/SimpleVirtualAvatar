#include "AssetManager.hpp"

#include <filesystem>

static Texture2D LoadTextureIfExists(const std::filesystem::path& file) {
    if (!std::filesystem::exists(file)) {
        return Texture2D{0};
    }

    Texture2D tex = LoadTexture(file.string().c_str());
    return tex;
}

AssetManager::~AssetManager() {
    clear_all();
}

void AssetManager::set_base_path(const std::string& path) {
    base_path = std::filesystem::path(path);
}

bool AssetManager::load_texture(const std::string& id, const std::string& filename) {
    if (textures.find(id) != textures.end()) {
        return true;
    }

    const std::filesystem::path full_path = base_path / filename;
    Texture2D tex = LoadTextureIfExists(full_path);

    if (tex.id == 0) {
        return false;
    }

    textures[id] = tex;
    return true;
}

bool AssetManager::load_sound(const std::string& id, const std::string& filename) {
    if (sounds.find(id) != sounds.end()) {
        return true;
    }

    const std::filesystem::path full_path = base_path / filename;
    if (!std::filesystem::exists(full_path)) {
        return false;
    }

    Sound snd = LoadSound(full_path.string().c_str());
    if (snd.frameCount == 0) {
        return false;
    }

    sounds[id] = snd;
    return true;
}

Texture2D AssetManager::get_texture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    return Texture2D{0};
}

Sound AssetManager::get_sound(const std::string& id) {
    auto it = sounds.find(id);
    if (it != sounds.end()) {
        return it->second;
    }
    return Sound{};
}

void AssetManager::unload_texture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        if (it->second.id > 0) {
            UnloadTexture(it->second);
        }
        textures.erase(it);
    }
}

void AssetManager::unload_state(StateFrames& state) {
    for (auto& tex : state.normal) {
        if (tex.id > 0) {
            UnloadTexture(tex);
        }
    }
    for (auto& tex : state.blink) {
        if (tex.id > 0) {
            UnloadTexture(tex);
        }
    }

    state.normal.clear();
    state.blink.clear();
}

void AssetManager::load_avatar_state(AvatarState state, int frames_count, const std::string& prefix) {
    if (frames_count < 0) {
        frames_count = 0;
    }

    const int idx = static_cast<int>(state);
    auto& target = avatar_assets[idx];

    unload_state(target);

    target.normal.reserve(frames_count);
    target.blink.reserve(frames_count);

    for (int i = 0; i < frames_count; ++i) {
        const std::filesystem::path normal_file = base_path / (prefix + "_" + std::to_string(i) + ".png");
        const std::filesystem::path blink_file = base_path / (prefix + "_blink_" + std::to_string(i) + ".png");

        target.normal.push_back(LoadTextureIfExists(normal_file));
        target.blink.push_back(LoadTextureIfExists(blink_file));
    }
}

void AssetManager::reload_avatar_state(AvatarState state, int frames_count, const std::string& prefix) {
    load_avatar_state(state, frames_count, prefix);
}

Texture2D AssetManager::get_avatar_frame(AvatarState state, int frame_index, bool is_blinking) {
    const int idx = static_cast<int>(state);
    const auto& target = avatar_assets[idx];

    const std::vector<Texture2D>& blinkFrames = target.blink;
    const std::vector<Texture2D>& normalFrames = target.normal;

    const std::vector<Texture2D>* frames = nullptr;

    if (is_blinking && !blinkFrames.empty()) {
        frames = &blinkFrames;
    } else if (!normalFrames.empty()) {
        frames = &normalFrames;
    } else if (!blinkFrames.empty()) {
        frames = &blinkFrames;
    }

    if (frames == nullptr || frames->empty()) {
        return Texture2D{0};
    }

    const int safeIndex = frame_index % static_cast<int>(frames->size());
    return (*frames)[safeIndex];
}

void AssetManager::clear_all() {
    for (auto& pair : textures) {
        if (pair.second.id > 0) {
            UnloadTexture(pair.second);
        }
    }
    textures.clear();

    for (auto& pair : sounds) {
        if (pair.second.frameCount > 0) {
            UnloadSound(pair.second);
        }
    }
    sounds.clear();

    unload_state(avatar_assets[0]);
    unload_state(avatar_assets[1]);
    unload_state(avatar_assets[2]);
}