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
    #ifdef _DEBUG
        std::cout << "[AssetManager] -: Texture '" << id << "' already loaded.\n";
    #endif
        return true; 
    }

    std::string full_path = base_path + filename;
    Texture2D tex = LoadTexture(full_path.c_str());

    if (tex.id == 0) {
    #ifdef _DEBUG
        std::cout << "[AssetManager] !: ERROR - Failed to load texture: " << full_path << "\n";
    #endif
        return false;
    }

    textures[id] = tex;
#ifdef _DEBUG
    std::cout << "[AssetManager] +: Loaded texture '" << id << "'\n";
#endif
    return true;
}

Texture2D AssetManager::get_texture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    
#ifdef _DEBUG
    std::cout << "[AssetManager] !: WARNING - Texture not found: " << id << "\n";
#endif
    return Texture2D{ 0 };
}

void AssetManager::unload_texture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        UnloadTexture(it->second);
        textures.erase(it);

    #ifdef _DEBUG
        std::cout << "[AssetManager] -: Unloaded texture '" << id << "'\n";
    #endif
    }
}

void AssetManager::load_avatar_state(AvatarState state, int frames_count, const std::string& prefix) {
    int state_idx = static_cast<int>(state);
    auto& target = avatar_assets[state_idx];

    for (int i = 0; i < frames_count; ++i) {
        std::string normal_file = base_path + prefix + "_" + std::to_string(i) + ".png";
        target.normal.push_back(LoadTexture(normal_file.c_str()));

        std::string blink_file = base_path + prefix + "_blink_" + std::to_string(i) + ".png";
        target.blink.push_back(LoadTexture(blink_file.c_str()));
    }
}

Texture2D AssetManager::get_avatar_frame(AvatarState state, int frame_index, bool is_blinking) {
    int state_idx = static_cast<int>(state);
    
    const auto& frames = is_blinking ? avatar_assets[state_idx].blink 
                                     : avatar_assets[state_idx].normal;

    if (frame_index >= 0 && frame_index < (int)frames.size()) {
        return frames[frame_index];
    }

    return Texture2D{ 0 };
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

#ifdef _DEBUG
    std::cout << "[AssetManager] Cleared all assets.\n";
#endif
}