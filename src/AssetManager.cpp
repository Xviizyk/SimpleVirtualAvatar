#include "AssetManager.hpp"

AssetManager::~AssetManager() {
    clear_all();
}

void AssetManager::set_base_path(const std::string& path) {
    base_path = path;
    if (!base_path.empty() && base_path.back() != '/' && base_path.back() != '\\') {
        base_path += "/";
    }
}

bool AssetManager::load_texture(const std::string& id, const std::string& filename) {
    if (textures.find(id) != textures.end()) {
        std::cout << "[AssetManager] -: Texture '" << id << "' already loaded.\n";
        return true; 
    }

    std::string full_path = base_path + filename;
    Texture2D tex = LoadTexture(full_path.c_str());

    if (tex.id == 0) {
        std::cout << "[AssetManager] !: ERROR - Failed to load texture: " << full_path << "\n";
        return false;
    }

    textures[id] = tex;
    std::cout << "[AssetManager] +: Loaded texture '" << id << "'\n";
    return true;
}

Texture2D AssetManager::get_texture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    
    std::cout << "[AssetManager] !: WARNING - Texture not found: " << id << "\n";
    return Texture2D{ 0 };
}

void AssetManager::unload_texture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        UnloadTexture(it->second);
        textures.erase(it);
        std::cout << "[AssetManager] -: Unloaded texture '" << id << "'\n";
    }
}

void AssetManager::load_avatar_state(AvatarState state, int frames_count, const std::string& prefix) {
    std::string state_name;
    switch (state) {
        case AvatarState::IDLE:      state_name = "idle"; break;
        case AvatarState::TALKING:   state_name = "talk"; break;
        case AvatarState::SCREAMING: state_name = "scream"; break;
    }

    for (int i = 0; i < frames_count; ++i) {
        std::string normal_key = state_name + "_" + std::to_string(i);
        std::string normal_file = prefix + "_" + std::to_string(i) + ".png";
        load_texture(normal_key, normal_file);

        std::string blink_key = state_name + "_blink_" + std::to_string(i);
        std::string blink_file = prefix + "_blink_" + std::to_string(i) + ".png";
        load_texture(blink_key, blink_file);
    }
}

Texture2D AssetManager::get_avatar_frame(AvatarState state, int frame_index, bool is_blinking) {
    std::string state_name;
    switch (state) {
        case AvatarState::IDLE:      state_name = "idle"; break;
        case AvatarState::TALKING:   state_name = "talk"; break;
        case AvatarState::SCREAMING: state_name = "scream"; break;
    }

    std::string key = state_name + (is_blinking ? "_blink_" : "_") + std::to_string(frame_index);
    return get_texture(key);
}

void AssetManager::clear_all() {
    for (auto& pair : textures) {
        if (pair.second.id > 0) UnloadTexture(pair.second);
    }
    textures.clear();

    for (auto& pair : sounds) {
        UnloadSound(pair.second);
    }
    sounds.clear();
    
    std::cout << "[AssetManager] Cleared all assets.\n";
}