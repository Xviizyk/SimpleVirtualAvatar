#pragma once

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "raylib.h"

enum class AvatarState { IDLE, TALKING, SCREAMING };

struct StateFrames {
    std::vector<Texture2D> normal;
    std::vector<Texture2D> blink;
};

class AssetManager {
private:
    std::unordered_map<std::string, Texture2D> textures;
    std::unordered_map<std::string, Sound> sounds;

    std::filesystem::path base_path = "assets";
    StateFrames avatar_assets[3];

    void unload_state(StateFrames& state);

public:
    AssetManager() = default;
    ~AssetManager();

    bool load_texture(const std::string& id, const std::string& filename);
    bool load_sound(const std::string& id, const std::string& filename);

    Texture2D get_texture(const std::string& id);
    Texture2D get_avatar_frame(AvatarState state, int frame_index, bool is_blinking);
    Sound get_sound(const std::string& id);

    void set_base_path(const std::string& path);

    void unload_texture(const std::string& id);
    void load_avatar_state(AvatarState state, int frames_count, const std::string& prefix);
    void reload_avatar_state(AvatarState state, int frames_count, const std::string& prefix);

    void clear_all();
};