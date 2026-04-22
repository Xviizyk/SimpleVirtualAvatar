#pragma once

#include "raylib.h"
#include "AssetManager.hpp"
#include "Animation.hpp"
#include "WinUtils.hpp"

#include <string>

enum class AvatarCorner {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

class Renderer {
private:
    Vector2 drag_offset = {0, 0};
    Vector2 avatar_position = {100, 100};
    
    bool is_dragging = false;

    float avatar_scale = 1.0f;

    bool is_ui_visible;
    bool is_blinking;

    int window_width;
    int window_height;
    int idle_max_frames;
    int talk_max_frames;
    int scream_max_frames;
    int avatar_margin = 20;

    std::string window_title;

    AvatarCorner avatar_corner = AvatarCorner::BOTTOM_RIGHT;
    AvatarState current_state;
    
    WinUtils winutils;
    Animation anim;
    
    static constexpr int PADDING = 20;
    static constexpr int FONT_SIZE = 20;
    static constexpr int VOLUME_BAR_WIDTH = 200;
    static constexpr int VOLUME_BAR_HEIGHT = 20;
    
    static constexpr Color TEXT_COLOR = {200, 200, 200, 255};
    static constexpr Color BAR_BG_COLOR = {60, 60, 60, 255};
    static constexpr Color BAR_FG_COLOR = {100, 200, 100, 255};
    static constexpr Color BACKGROUND_COLOR = {0, 0, 0, 0};
    static constexpr Color UI_TIPS_COLOR = {150, 150, 150, 200};
    
    void render_ui(float volume, float sensitivity);
    void render_avatar(AssetManager& assets, const Rectangle& avatar_rect);
    void update_animation(float delta_time);
    void render_volume_bar(float volume, int bar_x, int bar_y);
    void render_tips(int screen_width, int screen_height, float dpi, int font_size);
    void render_fps(int screen_width, float dpi, int font_size);
public:
    Renderer(int width = 1280, int height = 720, const std::string& title = "Simple Virtual Avatar");
    ~Renderer();
    bool init();
    void shutdown();
    bool should_close() const;
    void begin_frame();
    void end_frame();
    void draw_avatar(AssetManager& assets, float volume, float sensitivity);
    void toggle_ui_visibility();
    void handle_mouse_drag(const Rectangle& avatar_rect);
    void set_ui_visibility(bool visible);
    bool get_ui_visibility() const;
    void set_avatar_state(AvatarState state);
    AvatarState get_avatar_state() const;
    Rectangle calculate_avatar_rect(int base_size);
    float get_dpi_scale();
    int get_width() const { return window_width; }
    int get_height() const { return window_height; }
    void set_max_frames(int idle_max, int talk_max, int scream_max);
    void set_window_title(const std::string& title);
    void set_is_blinking(bool blinking);
    static Color get_color_by_state(AvatarState state);
};
