#pragma once

#include "Animation.hpp"
#include "../system/AssetManager.hpp"
#include "../core/Config.hpp"
#include "../ui/MenuBar.hpp"
#include "Shake.hpp"
#include "../ui/ShaderEditor.hpp"
#include "../ui/ShakeSettings.hpp"
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
    Vector2 drag_offset       = { 0.0f, 0.0f };
    Vector2 avatar_position   = { 100.0f, 100.0f };
    Vector2 last_window_position = { 0.0f, 0.0f };

    char fps_buffer[16]{};

    float fps_update_timer    = 0.0f;
    float avatar_scale        = 1.0f;
    float sensitivity         = 3.0f;
    float last_dpi_scale      = 1.0f;
    float dpi_scale_mult      = 1.25f;
    float update_dpi_scale_timer = 0.0f;

    bool is_required_update_dpi_scale = false;
    bool is_ui_visible    = true;
    bool is_anything_pressed = false;
    bool is_dragging      = false;

    int max_fps           = 0;
    int cached_fps_x      = 0;

    int last_render_height = 0;
    int last_render_width  = 0;
    int window_width       = 1280;
    int window_height      = 720;
    int idle_max_frames    = 1;
    int talk_max_frames    = 1;
    int scream_max_frames  = 1;
    int avatar_margin      = 20;

    std::string window_title = "Simple Virtual Avatar";

    AvatarCorner avatar_corner = AvatarCorner::BOTTOM_RIGHT;
    AvatarState  current_state = AvatarState::IDLE;

    OsUtils   winutils;
    Animation anim;

    // ── UI sub-panels (self-contained, own draw() and state) ─────────────────
    ShakeSettings shakeSettings;   // was: bool show_shake_settings + draw_shake_settings()

    // ── layout / style constants ─────────────────────────────────────────────
    static constexpr int PADDING           = 20;
    static constexpr int FONT_SIZE         = 20;
    static constexpr int VOLUME_BAR_WIDTH  = 200;
    static constexpr int VOLUME_BAR_HEIGHT = 20;

    static constexpr float SIDEBAR_W  = 220.0f;
    static constexpr float MENUBAR_H  = 32.0f;
    static constexpr float SECTION_PAD= 14.0f;
    static constexpr float LABEL_FS   = 10.0f;
    static constexpr float BODY_FS    = 12.0f;
    static constexpr float BTN_SIZE   = 22.0f;
    static constexpr float DIVIDER_H  = 1.0f;
    static constexpr float BAR_H      = 4.0f;
    static constexpr float BADGE_H    = 22.0f;
    static constexpr float BADGE_RADIUS= 4.0f;

    static constexpr Color BG_MAIN    = { 18,  18,  20,  255 };
    static constexpr Color BG_SIDEBAR = { 22,  22,  26,  255 };
    static constexpr Color BG_MENUBAR = { 28,  28,  34,  255 };

    static constexpr Color BORDER_COLOR  = { 48,  48,  58,  255 };
    static constexpr Color DIVIDER_COLOR = { 38,  38,  46,  255 };

    static constexpr Color TEXT_COLOR  = { 200, 200, 200, 255 };
    static constexpr Color LABEL_COLOR = {  90,  90, 110, 255 };
    static constexpr Color TEXT_DIM    = { 100, 100, 120, 255 };
    static constexpr Color TEXT_MID    = { 160, 160, 180, 255 };
    static constexpr Color TEXT_BRIGHT = { 220, 220, 235, 255 };
    static constexpr Color TIP_COLOR   = {  70,  70,  88, 255 };
    static constexpr Color FPS_COLOR   = {  80,  80,  98, 255 };

    static constexpr Color BAR_BG_COLOR= { 38,  38,  48, 255 };
    static constexpr Color BAR_FG_COLOR= { 80, 180, 120, 255 };
    static constexpr Color BAR_TRACK   = { 38,  38,  50, 255 };
    static constexpr Color BAR_IDLE    = { 80, 160, 110, 255 };
    static constexpr Color BAR_TALK    = { 60, 130, 220, 255 };
    static constexpr Color BAR_SCREAM  = { 200, 70,  70, 255 };

    static constexpr Color BACKGROUND_COLOR = { 18, 18, 20, 255 };
    static constexpr Color UI_TIPS_COLOR    = { 70, 70, 88, 255 };

    static constexpr Color BTN_NORMAL  = { 38,  38,  50, 255 };
    static constexpr Color BTN_HOVER   = { 52,  52,  68, 255 };
    static constexpr Color BTN_PRESSED = { 68,  68,  88, 255 };
    static constexpr Color BTN_BORDER  = { 55,  55,  72, 255 };

    static constexpr Color BADGE_BG     = { 32, 32, 42, 255 };
    static constexpr Color BADGE_BORDER = { 50, 50, 65, 255 };

    static constexpr Color BADGE_ACTIVE_BG     = { 28,  52,  90, 255 };
    static constexpr Color BADGE_ACTIVE_BORDER = { 50,  95, 165, 255 };
    static constexpr Color BADGE_ACTIVE_TEXT   = { 100, 170, 255, 255 };

    static constexpr Color BADGE_SCREAM_BG     = {  80, 28,  28, 255 };
    static constexpr Color BADGE_SCREAM_BORDER = { 150, 55,  55, 255 };
    static constexpr Color BADGE_SCREAM_TEXT   = { 255, 110, 110, 255 };

    static constexpr Color TAB_ACTIVE_BG  = {  42,  42,  56, 255 };
    static constexpr Color TAB_ACTIVE_FG  = { 220, 220, 235, 255 };
    static constexpr Color TAB_IDLE_FG    = { 100, 100, 120, 255 };

    void render_avatar(AssetManager& assets, const Rectangle& avatar_rect,
                       bool current_blink_state, ShaderEditor& shaderEditor,
                       const Vector2& shakeOffset);
    void render_fps(int screen_width, float dpi, int font_size);
    void render_tips(int screen_height, float dpi, int font_size);
    void render_ui(float volume);
    void render_sidebar(float volume, float dpi);
    void render_menubar(float dpi, MenuBar& menuBar, AssetManager& assets,
                        SpriteEditor& spriteEditor, ShaderEditor& shaderEditor);
    void render_state_badges(float x, float y, float dpi);
    bool render_button(Rectangle bounds, const char* text, float dpi);
    void render_volume_bar(float volume, int bar_x, int bar_y);
    void update_animation(float delta_time);
    void update_dpi_scale(float delta_time);

public:
    Renderer(int width = 1280, int height = 720,
             const std::string& title = "Simple Virtual Avatar");
    ~Renderer();

    Rectangle    calculate_avatar_rect(int base_size);
    AvatarState  get_avatar_state() const;
    static Color get_color_by_state(AvatarState state);
    int          get_height() const { return window_height; }
    bool         get_ui_visibility() const;
    float        get_sensitivity() const { return sensitivity; }
    int          get_width()  const { return window_width; }
    void         handle_mouse_drag(const Rectangle& avatar_rect);
    bool         init();
    void         set_avatar_state(AvatarState state);
    void         set_max_frames(int idle_max, int talk_max, int scream_max);
    void         set_ui_visibility(bool visible);
    void         set_window_title(const std::string& title);
    bool         should_close() const;
    void         shutdown();
    void         toggle_ui_visibility();
    void         update(AssetManager& assets, float volume, MenuBar& menuBar,
                        SpriteEditor& spriteEditor, ShaderEditor& shaderEditor,
                        Shake& shake);
};