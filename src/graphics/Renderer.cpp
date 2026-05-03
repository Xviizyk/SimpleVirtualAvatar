#include "../graphics/Renderer.hpp"
#include "../system/AssetManager.hpp"
#include "../utils/Utils.hpp"
#include "../utils/OsUtils.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>

static constexpr float LABEL_FS      = 10.0f;
static constexpr Color LABEL_COLOR   = { 90, 90, 110, 255 };

static constexpr float DIVIDER_H     = 1.0f;
static constexpr Color DIVIDER_COLOR = { 38, 38, 46, 255 };

Renderer::Renderer(int width, int height, const std::string& title)
    : window_width(width),
      window_height(height),
      window_title(title),
      current_state(AvatarState::IDLE) {}

Renderer::~Renderer() {
    if (IsWindowReady()) CloseWindow();
}

Rectangle Renderer::calculate_avatar_rect(int base_size) {
    const float size = base_size * last_dpi_scale * avatar_scale;
    return { avatar_position.x, avatar_position.y, size, size };
}

AvatarState Renderer::get_avatar_state() const { return current_state; }
bool        Renderer::get_ui_visibility()  const { return is_ui_visible; }

Color Renderer::get_color_by_state(AvatarState state) {
    switch (state) {
        case AvatarState::IDLE:      return { 100, 200, 100, 255 };
        case AvatarState::TALKING:   return { 100, 150, 255, 255 };
        case AvatarState::SCREAMING: return { 255, 100, 100, 255 };
        default:                     return WHITE;
    }
}

void Renderer::handle_mouse_drag(const Rectangle& avatar_rect) {
    if (is_anything_pressed) return;

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, avatar_rect)) {
            is_dragging = true;
            drag_offset = { mouse.x - avatar_rect.x, mouse.y - avatar_rect.y };
        }
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) is_dragging = false;

    if (is_dragging) {
        avatar_position = { mouse.x - drag_offset.x, mouse.y - drag_offset.y };
    }
}

bool Renderer::init() {
    SetConfigFlags(
        FLAG_WINDOW_ALWAYS_RUN  |
        FLAG_WINDOW_TRANSPARENT |
        FLAG_WINDOW_RESIZABLE   |
#ifdef NDEBUG
        FLAG_VSYNC_HINT         |
#endif
        FLAG_WINDOW_TOPMOST
    );

    InitWindow(window_width, window_height, window_title.c_str());
    if (!IsWindowReady()) return false;

    SetTargetFPS(0);
    set_ui_visibility(true);
    return true;
}

void Renderer::render_avatar(AssetManager& assets, const Rectangle& avatar_rect,
                              bool current_blink_state, ShaderEditor& shaderEditor,
                              const Vector2& shakeOffset)
{
    Rectangle rect = avatar_rect;
    rect.x += shakeOffset.x;
    rect.y += shakeOffset.y;

    Texture2D tex = assets.get_avatar_frame(current_state, anim.get_current_frame(), current_blink_state);

    if (shaderEditor.is_enabled()) {
        shaderEditor.apply_uniforms(static_cast<float>(GetTime()));
        BeginShaderMode(shaderEditor.get_shader());
    }

    if (tex.id > 0) {
        DrawTexturePro(
            tex,
            { 0, 0, static_cast<float>(tex.width), static_cast<float>(tex.height) },
            rect, { 0, 0 }, 0.0f, WHITE
        );
    } else {
        DrawRectangleRec(rect, { 40, 40, 44, 200 });
        DrawRectangleLinesEx(rect, 1.0f, BORDER_COLOR);
        const char* msg = "No avatar loaded";
        const int   fs  = static_cast<int>(BODY_FS * last_dpi_scale);
        const int   tw  = MeasureText(msg, fs);
        DrawText(msg,
            static_cast<int>(rect.x + (rect.width  - tw) / 2.0f),
            static_cast<int>(rect.y + (rect.height - fs) / 2.0f),
            fs, TEXT_DIM);
    }

    if (shaderEditor.is_enabled()) EndShaderMode();
}

void Renderer::render_fps(int screen_width, float dpi, int /*font_size*/) {
    fps_update_timer += GetFrameTime();
    const int fs = static_cast<int>(BODY_FS * dpi);

    if (fps_update_timer >= 0.1f || cached_fps_x == 0) {
        fps_update_timer = 0.0f;
        std::snprintf(fps_buffer, sizeof(fps_buffer), "FPS: %d", GetFPS());
        cached_fps_x = screen_width - MeasureText(fps_buffer, fs) - static_cast<int>(12 * dpi);
    }

    DrawText(fps_buffer, cached_fps_x,
        static_cast<int>((MENUBAR_H + 10.0f) * dpi), fs, FPS_COLOR);
}

static void draw_section_label(const char* text, float x, float y, float dpi) {
    DrawText(text,
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(LABEL_FS * dpi),
        LABEL_COLOR);
}

static void draw_divider(float x, float y, float w, float dpi) {
    DrawRectangle(
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(w),
        static_cast<int>(DIVIDER_H * dpi),
        DIVIDER_COLOR);
}

void Renderer::render_volume_bar(float volume, int bar_x, int bar_y) {
    const float dpi = last_dpi_scale;
    const int bw = static_cast<int>(SIDEBAR_W * 0.82f * dpi);
    const int bh = static_cast<int>(BAR_H * dpi);

    DrawRectangle(bar_x, bar_y, bw, bh, BAR_TRACK);

    float norm = std::clamp(volume * sensitivity, 0.0f, 1.0f);
    int   fill = static_cast<int>(bw * norm);

    Color bar_color = BAR_IDLE;
    if (current_state == AvatarState::TALKING)   bar_color = BAR_TALK;
    if (current_state == AvatarState::SCREAMING) bar_color = BAR_SCREAM;
    if (norm > 0.9f)                             bar_color = BAR_SCREAM;

    DrawRectangle(bar_x, bar_y, fill, bh, bar_color);
}

void Renderer::render_state_badges(float x, float y, float dpi) {
    struct BadgeDef {
        const char* label;
        AvatarState state;
        Color dot_active;
    };

    static const BadgeDef badges[] = {
        { "Idle", AvatarState::IDLE, { 180, 180, 180, 255 } },
        { "Talk", AvatarState::TALKING, {  74, 158, 255, 255 } },
        { "Scream", AvatarState::SCREAMING, { 255,  80,  80, 255 } }
    };

    const int fs = static_cast<int>(BODY_FS * dpi);
    const float bh = BADGE_H * dpi;
    const float dot_r = 4.0f * dpi;
    const float pad_x = 8.0f * dpi;
    const float gap = 6.0f * dpi;
    const float dot_gap = 6.0f * dpi;

    Vector2 mouse = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    float cx = x;
    for (auto& b : badges) {
        const bool active = (current_state == b.state);

        int tw = MeasureText(b.label, fs);
        float bw = pad_x * 2 + dot_r * 2 + dot_gap + tw;

        Rectangle rec = { cx, y, bw, bh };
        bool hovered = CheckCollisionPointRec(mouse, rec);

        if (hovered && clicked && !is_anything_pressed) {
            set_avatar_state(b.state);
            is_anything_pressed = true;
        }

        Color bg = active  ? (b.state == AvatarState::SCREAMING ? BADGE_SCREAM_BG : BADGE_ACTIVE_BG) : hovered ? BTN_HOVER : BADGE_BG;
        Color bdr = active  ? (b.state == AvatarState::SCREAMING ? BADGE_SCREAM_BORDER : BADGE_ACTIVE_BORDER) : hovered ? BADGE_BORDER : BADGE_BORDER;
        Color fg = active  ? (b.state == AvatarState::SCREAMING ? BADGE_SCREAM_TEXT : BADGE_ACTIVE_TEXT) : hovered ? TEXT_MID : TEXT_DIM;

        DrawRectangleRec(rec, bg);
        DrawRectangleLinesEx(rec, 0.5f, bdr);

        float dot_cx = cx + pad_x + dot_r;
        float dot_cy = y + bh / 2.0f;
        DrawCircle(static_cast<int>(dot_cx), static_cast<int>(dot_cy), dot_r, active ? b.dot_active : Color{ 180, 180, 180, 255 });

        DrawText(b.label,
            static_cast<int>(cx + pad_x + dot_r * 2 + dot_gap),
            static_cast<int>(y + (bh - fs) / 2.0f),
            fs, fg);

        cx += bw + gap;
    }
}

void Renderer::render_sidebar(float volume, float dpi) {
    const float sw = SIDEBAR_W * dpi;
    const float sh = static_cast<float>(GetScreenHeight());
    const float mh = MENUBAR_H * dpi;
    const float pad = SECTION_PAD * dpi;
    const int lfs = static_cast<int>(LABEL_FS * dpi);
    const int bfs = static_cast<int>(BODY_FS * dpi);
    const float line= 18.0f * dpi;

    DrawRectangle(0, static_cast<int>(mh), static_cast<int>(sw), static_cast<int>(sh - mh), BG_SIDEBAR);
    DrawRectangle(static_cast<int>(sw), static_cast<int>(mh), 1, static_cast<int>(sh - mh), BORDER_COLOR);

    float y = mh + pad;

    draw_section_label("VOLUME", pad, y, dpi);
    y += line;

    render_volume_bar(volume, static_cast<int>(pad), static_cast<int>(y));
    y += BAR_H * dpi + 5.0f * dpi;

    char vol_text[32];
    std::snprintf(vol_text, sizeof(vol_text), "%.2f", volume * sensitivity);
    DrawText(vol_text, static_cast<int>(sw - pad - MeasureText(vol_text, bfs)),
             static_cast<int>(y), bfs, TEXT_DIM);
    y += line + pad * 0.5f;

    draw_divider(pad, y, sw - pad * 2, dpi);
    y += DIVIDER_H * dpi + pad;

    draw_section_label("SENSITIVITY", pad, y, dpi);
    y += line;

    const float btn_sz  = BTN_SIZE * dpi;
    const float btn_gap = 6.0f    * dpi;

    is_anything_pressed = false;
    if (render_button({ pad, y, btn_sz, btn_sz }, "-", dpi))
        sensitivity -= 10.0f * GetFrameTime();
    if (render_button({ pad + btn_sz + btn_gap, y, btn_sz, btn_sz }, "+", dpi))
        sensitivity += 10.0f * GetFrameTime();

    sensitivity = std::clamp(sensitivity, 0.0f, 100.0f);

    char sens_text[32];
    std::snprintf(sens_text, sizeof(sens_text), "%.2f", sensitivity);
    const float val_x = pad + btn_sz * 2 + btn_gap * 2;

    DrawText(sens_text, static_cast<int>(val_x),
             static_cast<int>(y + (btn_sz - bfs) / 2.0f), bfs, TEXT_BRIGHT);

    const float track_x = val_x + MeasureText(sens_text, bfs) + 8.0f * dpi;
    const float track_w = sw - pad - track_x;
    const float track_y = y + btn_sz / 2.0f - BAR_H * dpi / 2.0f;
    const float track_h = BAR_H * dpi;

    if (track_w > 12.0f * dpi) {
        DrawRectangle(static_cast<int>(track_x), static_cast<int>(track_y),
                      static_cast<int>(track_w), static_cast<int>(track_h), BAR_TRACK);
        float fill_w = track_w * (sensitivity / 100.0f);
        DrawRectangle(static_cast<int>(track_x), static_cast<int>(track_y),
                      static_cast<int>(fill_w),  static_cast<int>(track_h), BAR_IDLE);
    }

    y += btn_sz + pad * 0.75f;

    draw_section_label("DPI SCALE", pad, y, dpi);
    y += line;

    is_anything_pressed = false;
    if (render_button({ pad, y, btn_sz, btn_sz }, "-", dpi)) {
        dpi_scale_mult -= 0.1f * GetFrameTime();
        is_required_update_dpi_scale = true;
    }
    if (render_button({ pad + btn_sz + btn_gap, y, btn_sz, btn_sz }, "+", dpi)) {
        dpi_scale_mult += 0.1f * GetFrameTime();
        is_required_update_dpi_scale = true;
    }

    dpi_scale_mult = std::clamp(dpi_scale_mult, 0.1f, 3.0f);

    char sens_text_dpi[32];
    std::snprintf(sens_text_dpi, sizeof(sens_text_dpi), "%.2f", dpi_scale_mult);
    const float val_x_dpi = pad + btn_sz * 2 + btn_gap * 2;

    DrawText(sens_text_dpi, static_cast<int>(val_x_dpi),
             static_cast<int>(y + (btn_sz - bfs) / 2.0f), bfs, TEXT_BRIGHT);

    const float track_x_dpi = val_x_dpi + MeasureText(sens_text_dpi, bfs) + 8.0f * dpi;
    const float track_w_dpi = sw - pad - track_x_dpi;
    const float track_y_dpi = y + btn_sz / 2.0f - BAR_H * dpi / 2.0f;
    const float track_h_dpi = BAR_H * dpi;

    if (track_w_dpi > 12.0f * dpi) {
        DrawRectangle(static_cast<int>(track_x_dpi), static_cast<int>(track_y_dpi),
                      static_cast<int>(track_w_dpi), static_cast<int>(track_h_dpi), BAR_TRACK);
        float fill_w_dpi = track_w_dpi * (dpi_scale_mult / 3.0f);
        DrawRectangle(static_cast<int>(track_x_dpi), static_cast<int>(track_y_dpi),
                      static_cast<int>(fill_w_dpi),  static_cast<int>(track_h_dpi), BAR_IDLE);
    }

    y += btn_sz + pad * 0.75f;

    draw_divider(pad, y, sw - pad * 2, dpi);
    y += DIVIDER_H * dpi + pad;

    draw_section_label("STATE", pad, y, dpi);
    y += line;

    render_state_badges(pad, y, dpi);

    const char* tip = "Shift+F11  toggle UI";
    DrawText(tip,
        static_cast<int>(pad),
        static_cast<int>(sh - pad - lfs),
        lfs, TIP_COLOR);
}

void Renderer::render_menubar(float dpi, MenuBar& menuBar, AssetManager& assets, SpriteEditor& spriteEditor, ShaderEditor& shaderEditor) {
    const int sw = GetScreenWidth();
    const float mh = MENUBAR_H * dpi;

    DrawRectangle(0, 0, sw, static_cast<int>(mh), BG_MENUBAR);
    DrawRectangle(0, static_cast<int>(mh) - 1, sw, 1, BORDER_COLOR);

    MenuBarAction action = menuBar.draw();

    if (action.openIdleEditor) spriteEditor.open_window(AvatarState::IDLE);
    if (action.openTalkEditor) spriteEditor.open_window(AvatarState::TALKING);
    if (action.openScreamEditor) spriteEditor.open_window(AvatarState::SCREAMING);
    if (spriteEditor.is_open()) spriteEditor.draw(assets, dpi);

    if (action.openShaderEditor) shaderEditor.open_window();
    if (shaderEditor.is_open()) shaderEditor.draw(dpi);

    if (action.openShakeSettings) shakeSettings.toggle();
    if (shakeSettings.is_open()) shakeSettings.draw(dpi);
}

void Renderer::render_ui(float volume) {
    const float dpi = last_dpi_scale;
    render_sidebar(volume, dpi);
    render_fps(GetScreenWidth(), dpi, static_cast<int>(BODY_FS * dpi));
}

bool Renderer::render_button(Rectangle bounds, const char* text, float dpi) {
    if (!is_ui_visible) return false;

    Vector2 mouse = GetMousePosition();
    bool hovered = CheckCollisionPointRec(mouse, bounds);
    bool pressed = false;

    Color col = hovered ? BTN_HOVER : BTN_NORMAL;
    if (hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        col = BTN_PRESSED;
        pressed = true;
        is_anything_pressed = true;
    }

    DrawRectangleRec(bounds, col);
    DrawRectangleLinesEx(bounds, 0.5f, BTN_BORDER);

    const int fs = static_cast<int>(BODY_FS * dpi);
    const int tw = MeasureText(text, fs);
    DrawText(text,
        static_cast<int>(bounds.x + (bounds.width  - tw) / 2.0f),
        static_cast<int>(bounds.y + (bounds.height - fs) / 2.0f),
        fs, TEXT_BRIGHT);

    return pressed;
}

void Renderer::toggle_ui_visibility() {
    set_ui_visibility(!is_ui_visible);
}

void Renderer::set_ui_visibility(bool visible) {
    is_ui_visible = visible;

    void* hwnd = GetWindowHandle();
    if (!hwnd) return;

    if (is_ui_visible) {
        winutils.set_window_borderless(hwnd, false);
        winutils.set_overlay_mode(hwnd, false);
        winutils.set_show_in_taskbar(hwnd, true);
        ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
        ClearWindowState(FLAG_WINDOW_TOPMOST);
    } else {
        winutils.setup_overlay(hwnd);
        winutils.set_window_borderless(hwnd, true);
        winutils.set_overlay_mode(hwnd, true);
        SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    }
}

void Renderer::set_avatar_state(AvatarState state) {
    if (current_state == state) return;
    current_state = state;

    switch (state) {
        case AvatarState::IDLE: anim.set_max_frames(idle_max_frames); break;
        case AvatarState::TALKING: anim.set_max_frames(talk_max_frames); break;
        case AvatarState::SCREAMING: anim.set_max_frames(scream_max_frames); break;
    }
    anim.reset_frame();
}

void Renderer::set_max_frames(int idle_max, int talk_max, int scream_max) {
    const bool changed = (idle_max_frames != idle_max) ||
                         (talk_max_frames != talk_max) ||
                         (scream_max_frames != scream_max);

    idle_max_frames   = idle_max;
    talk_max_frames   = talk_max;
    scream_max_frames = scream_max;

    if (changed) {
        switch (current_state) {
            case AvatarState::IDLE: anim.set_max_frames(idle_max_frames); break;
            case AvatarState::TALKING: anim.set_max_frames(talk_max_frames); break;
            case AvatarState::SCREAMING: anim.set_max_frames(scream_max_frames); break;
        }
    }
}

void Renderer::set_window_title(const std::string& title) {
    window_title = title;
    if (IsWindowReady()) SetWindowTitle(window_title.c_str());
}

void Renderer::update(AssetManager& assets, float volume, MenuBar& menuBar, SpriteEditor& spriteEditor, ShaderEditor& shaderEditor, Shake& shake)
{
    const float delta_time = Utils::get_delta_time();

    update_dpi_scale(delta_time);
    update_animation(delta_time);
    shake.update(delta_time);

    Rectangle avatar_rect = calculate_avatar_rect(400);

    if (!spriteEditor.is_open() &&
        !shakeSettings.is_open() &&
        !shaderEditor.is_open())
    {
        handle_mouse_drag(avatar_rect);
    }

    BeginDrawing();

    if (is_ui_visible) {
        ClearBackground(BG_MAIN);

        const Vector2 shakeOffset = shake.get_offset();
        render_avatar(assets, avatar_rect, anim.is_blink(), shaderEditor, shakeOffset);

        render_ui(volume);
        render_menubar(last_dpi_scale, menuBar, assets, spriteEditor, shaderEditor);
    } else {
        ClearBackground({ 0, 0, 0, 0 });
        const Vector2 shakeOffset = shake.get_offset();
        render_avatar(assets, avatar_rect, anim.is_blink(), shaderEditor, shakeOffset);
    }

    EndDrawing();
}

void Renderer::update_dpi_scale(float delta_time) {
    update_dpi_scale_timer -= delta_time;
    if (update_dpi_scale_timer > 0.0f && !is_required_update_dpi_scale) return;
    update_dpi_scale_timer = 0.5f;

    void* hwnd = GetWindowHandle();
    if (!hwnd && !is_required_update_dpi_scale) return;

    Vector2 pos = GetWindowPosition();
    int     w   = GetScreenWidth();

    if (last_window_position.x == pos.x &&
        last_window_position.y == pos.y &&
        last_render_width      == w     &&
        !is_required_update_dpi_scale)
        return;

    is_required_update_dpi_scale = false;
    last_window_position = pos;
    last_render_width    = w;
    last_dpi_scale       = winutils.get_dpi_scale(hwnd) * dpi_scale_mult;
}

void Renderer::update_animation(float delta_time) {
    anim.update(delta_time);
}

bool Renderer::should_close() const { return WindowShouldClose(); }

void Renderer::shutdown() {
    if (IsWindowReady()) CloseWindow();
}