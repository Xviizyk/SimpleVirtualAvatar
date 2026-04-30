#pragma once

#include "Animation.hpp"
#include "../system/AssetManager.hpp"
#include "../core/Config.hpp"
#include "../ui/MenuBar.hpp"
#include "Shake.hpp"
#include "../ui/ShaderEditor.hpp"
#include "../ui/SpriteEditor.hpp"
#include "../utils/OsUtils.hpp"

#include "raylib.h"

#include <string>

enum class AvatarCorner {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

class Renderer {
private:
    Rectangle windowRect = { 100, 100, 600, 400 };
    bool isDragging = false;
    bool isResizing = false;
    Vector2 dragOffset = { 0, 0 };
    const float RESIZE_BORDER = 10.0f;

    Vector2 drag_offset = {0.0f, 0.0f};
    Vector2 avatar_position = {100.0f, 100.0f};
    Vector2 last_window_position = {0.0f, 0.0f};

    char fps_buffer[16]{};

    float fps_update_timer = 0.0f;
    float avatar_scale = 1.0f;
    float sensitivity = 3.0f;
    float last_dpi_scale = 1.0f;
    float update_dpi_scale_timer = 0.0f;

    bool is_ui_visible = true;
    bool is_anything_pressed = false;
    bool is_dragging = false;
    bool show_shake_settings = false;

    int max_fps = 0;
    int cached_fps_x = 0;

    int last_render_height = 0;
    int last_render_width = 0;
    int window_width = 1280;
    int window_height = 720;
    int idle_max_frames = 1;
    int talk_max_frames = 1;
    int scream_max_frames = 1;
    int avatar_margin = 20;

    std::string window_title = "Simple Virtual Avatar";

    AvatarCorner avatar_corner = AvatarCorner::BOTTOM_RIGHT;
    AvatarState current_state = AvatarState::IDLE;

    OsUtils winutils;
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

    void render_avatar(AssetManager& assets, const Rectangle& avatar_rect, bool current_blink_state, ShaderEditor& shaderEditor, const Vector2& shakeOffset);
    void render_fps(int screen_width, float dpi, int font_size);
    void render_tips(int screen_height, float dpi, int font_size);
    void render_ui(float volume);
    bool render_button(Rectangle bounds, const char* text, float dpi);
    void render_volume_bar(float volume, int bar_x, int bar_y);
    void update_animation(float delta_time);
    void update_dpi_scale(float delta_time);
    void draw_shake_settings();

public:
    Renderer(int width = 1280, int height = 720, const std::string& title = "Simple Virtual Avatar");
    ~Renderer();

    Rectangle calculate_avatar_rect(int base_size);
    AvatarState get_avatar_state() const;
    static Color get_color_by_state(AvatarState state);
    int get_height() const { return window_height; }
    bool get_ui_visibility() const;
    float get_sensitivity() const { return sensitivity; }
    int get_width() const { return window_width; }
    void handle_mouse_drag(const Rectangle& avatar_rect);
    bool init();
    void set_avatar_state(AvatarState state);
    void set_max_frames(int idle_max, int talk_max, int scream_max);
    void set_ui_visibility(bool visible);
    void set_window_title(const std::string& title);
    bool should_close() const;
    void shutdown();
    void toggle_ui_visibility();
    void update(AssetManager& assets, float volume, MenuBar& menuBar, SpriteEditor& spriteEditor, ShaderEditor& shaderEditor, Shake& shake);
};