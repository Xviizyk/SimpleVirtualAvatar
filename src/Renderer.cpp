#include "Renderer.hpp"

#include "Utils.hpp"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <sstream>
#include <string>

Renderer::Renderer(int width, int height, const std::string& title)
    : window_width(width),
      window_height(height),
      window_title(title),
      current_state(AvatarState::IDLE) {}

Renderer::~Renderer() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}

Rectangle Renderer::calculate_avatar_rect(int base_size) {
    const float size = base_size * last_dpi_scale * avatar_scale;
    return { avatar_position.x, avatar_position.y, size, size };
}

AvatarState Renderer::get_avatar_state() const {
    return current_state;
}

bool Renderer::get_ui_visibility() const {
    return is_ui_visible;
}

Color Renderer::get_color_by_state(AvatarState state) {
    switch (state) {
        case AvatarState::IDLE:     return {100, 200, 100, 255};
        case AvatarState::TALKING:  return {100, 150, 255, 255};
        case AvatarState::SCREAMING: return {255, 100, 100, 255};
        default:                     return WHITE;
    }
}

void Renderer::handle_mouse_drag(const Rectangle& avatar_rect) {
    if (is_anything_pressed) return;

    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(mouse, avatar_rect)) {
            is_dragging = true;
            drag_offset = {
                mouse.x - avatar_rect.x,
                mouse.y - avatar_rect.y
            };
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        is_dragging = false;
    }

    if (is_dragging) {
        avatar_position = {
            mouse.x - drag_offset.x,
            mouse.y - drag_offset.y
        };
    }
}

bool Renderer::init() {
    SetConfigFlags(
        FLAG_WINDOW_ALWAYS_RUN |
        FLAG_WINDOW_TRANSPARENT |
        FLAG_WINDOW_RESIZABLE |
        FLAG_WINDOW_TOPMOST
    );

    InitWindow(window_width, window_height, window_title.c_str());
    if (!IsWindowReady()) {
        return false;
    }

    SetTargetFPS(0);
    set_ui_visibility(true);
    return true;
}

void Renderer::render_avatar(AssetManager& assets, const Rectangle& avatar_rect, bool current_blink_state, ShaderEditor& shaderEditor, const Vector2& shakeOffset) {
    Rectangle rect = avatar_rect;
    rect.x += shakeOffset.x;
    rect.y += shakeOffset.y;

    Texture2D avatar_texture = assets.get_avatar_frame(
        current_state,
        anim.get_current_frame(),
        current_blink_state
    );

    if (shaderEditor.IsEnabled()) {
        shaderEditor.ApplyUniforms(static_cast<float>(GetTime()));
        BeginShaderMode(shaderEditor.GetShader());
    }

    if (avatar_texture.id > 0) {
        DrawTexturePro(
            avatar_texture,
            {0, 0, static_cast<float>(avatar_texture.width), static_cast<float>(avatar_texture.height)},
            rect,
            {0, 0},
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(rect, {80, 80, 80, 255});
        DrawRectangleLinesEx(rect, 2.0f, {150, 150, 150, 255});

        const char* msg = "No avatar loaded";
        const int fs = static_cast<int>(FONT_SIZE * last_dpi_scale);
        const int tw = MeasureText(msg, fs);
        DrawText(msg, static_cast<int>(rect.x + (rect.width - tw) / 2.0f), static_cast<int>(rect.y + rect.height / 2.0f), fs, TEXT_COLOR);
    }

    if (shaderEditor.IsEnabled()) {
        EndShaderMode();
    }
}

void Renderer::render_fps(int screen_width, float dpi, int font_size) {
    fps_update_timer += GetFrameTime();

    if (fps_update_timer >= 0.1f || cached_fps_x == 0) {
        fps_update_timer = 0.0f;
        const int current_fps = GetFPS();
        std::snprintf(fps_buffer, sizeof(fps_buffer), "FPS: %d", current_fps);

        const float padding = PADDING * dpi;
        const int text_width = MeasureText(fps_buffer, font_size);
        cached_fps_x = screen_width - text_width - static_cast<int>(padding * 1.5f);
    }

    DrawText(fps_buffer, cached_fps_x, static_cast<int>(PADDING * dpi), font_size, TEXT_COLOR);
}

void Renderer::render_ui(float volume) {
    const int current_height = GetScreenHeight();

    Rectangle avatar_rect = calculate_avatar_rect(400);
    const float dpi = last_dpi_scale;
    const float padding = PADDING * dpi + 35.0f;
    const float font_size = FONT_SIZE * dpi;
    const float line_height = 35.0f * dpi;
    const float btnSize = 50.0f * dpi;
    const float btnX = padding + 220.0f * dpi;

    float y_offset = padding;

    char volume_text[32];
    std::snprintf(volume_text, sizeof(volume_text), "Volume: %.2f", volume * sensitivity);
    DrawText(volume_text, static_cast<int>(padding), static_cast<int>(y_offset), static_cast<int>(font_size), TEXT_COLOR);
    render_volume_bar(volume, static_cast<int>(padding + 150.0f * dpi), static_cast<int>(y_offset));

    y_offset += line_height;

    char sensitivity_text[32];
    std::snprintf(sensitivity_text, sizeof(sensitivity_text), "Sensitivity: %.2f", sensitivity);
    DrawText(sensitivity_text, static_cast<int>(padding), static_cast<int>(y_offset), static_cast<int>(font_size), TEXT_COLOR);

    is_anything_pressed = false;
    if (render_button({ btnX, y_offset - (btnSize / 4.0f), btnSize, btnSize }, "-", dpi)) sensitivity -= 10.0f * GetFrameTime();
    if (render_button({ btnX + btnSize + 5.0f, y_offset - (btnSize / 4.0f), btnSize, btnSize }, "+", dpi)) sensitivity += 10.0f * GetFrameTime();

    if (sensitivity < 0.0f) sensitivity = 0.0f;
    if (sensitivity > 100.0f) sensitivity = 100.0f;

    render_tips(current_height, dpi, static_cast<int>(font_size));
    render_fps(GetScreenWidth(), dpi, static_cast<int>(font_size));
}

bool Renderer::render_button(Rectangle bounds, const char* text, float dpi) {
    if (!is_ui_visible) return false;

    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    bool pressed = false;

    Color col = hovered ? LIGHTGRAY : GRAY;

    if (hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        col = DARKGRAY;
        pressed = true;
        is_anything_pressed = true;
    }

    DrawRectangleRec(bounds, col);
    DrawRectangleLinesEx(bounds, 1.0f, WHITE);

    int fontSize = static_cast<int>(20 * dpi);
    int textWidth = MeasureText(text, fontSize);
    DrawText(
        text,
        static_cast<int>(bounds.x + (bounds.width - textWidth) / 2.0f),
        static_cast<int>(bounds.y + (bounds.height - fontSize) / 2.0f),
        fontSize,
        WHITE
    );

    return pressed;
}

void Renderer::render_volume_bar(float volume, int bar_x, int bar_y) {
    float dpi = last_dpi_scale;
    int bar_width = static_cast<int>(VOLUME_BAR_WIDTH * dpi);
    int bar_height = static_cast<int>(VOLUME_BAR_HEIGHT * dpi);

    DrawRectangle(bar_x, bar_y, bar_width, bar_height, BAR_BG_COLOR);

    float normalized_volume = volume * sensitivity;
    normalized_volume = std::clamp(normalized_volume, 0.0f, 1.0f);

    int filled_width = static_cast<int>(bar_width * normalized_volume);

    Color bar_color = BAR_FG_COLOR;
    if (normalized_volume > 0.7f) bar_color = {255, 200, 100, 255};
    if (normalized_volume > 0.9f) bar_color = {255, 100, 100, 255};

    DrawRectangle(bar_x, bar_y, filled_width, bar_height, bar_color);
    DrawRectangleLines(bar_x, bar_y, bar_width, bar_height, TEXT_COLOR);
}

void Renderer::render_tips(int screen_height, float dpi, int font_size) {
    const char* tips[] = {
        "Shift+F11: Toggle UI"
    };

    float padding = PADDING * dpi;
    for (int i = 0; i < 1; i++) {
        int y = screen_height - static_cast<int>(padding) - (1 - i) * static_cast<int>(25 * dpi);
        DrawText(
            tips[i],
            static_cast<int>(padding),
            y,
            static_cast<int>(font_size - 4 * dpi),
            UI_TIPS_COLOR
        );
    }
}

void Renderer::toggle_ui_visibility() {
    set_ui_visibility(!is_ui_visible);
}

void Renderer::set_avatar_state(AvatarState state) {
    if (current_state != state) {
        current_state = state;

        switch (state) {
            case AvatarState::IDLE:
                anim.set_max_frames(idle_max_frames);
                break;
            case AvatarState::TALKING:
                anim.set_max_frames(talk_max_frames);
                break;
            case AvatarState::SCREAMING:
                anim.set_max_frames(scream_max_frames);
                break;
        }

        anim.reset_frame();
    }
}

void Renderer::set_max_frames(int idle_max, int talk_max, int scream_max) {
    const bool changed = (idle_max_frames != idle_max) || (talk_max_frames != talk_max) || (scream_max_frames != scream_max);

    idle_max_frames = idle_max;
    talk_max_frames = talk_max;
    scream_max_frames = scream_max;

    if (changed) {
        switch (current_state) {
            case AvatarState::IDLE:     anim.set_max_frames(idle_max_frames); break;
            case AvatarState::TALKING:  anim.set_max_frames(talk_max_frames); break;
            case AvatarState::SCREAMING: anim.set_max_frames(scream_max_frames); break;
        }
    }
}

void Renderer::set_ui_visibility(bool visible) {
    is_ui_visible = visible;

    void* hwnd = GetWindowHandle();
    if (!hwnd) return;

    if (is_ui_visible) {
        winutils.set_window_borderless(hwnd, false);
        winutils.set_overlay_mode(hwnd, false);
        ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    } else {
        winutils.set_window_borderless(hwnd, true);
        winutils.set_overlay_mode(hwnd, true);
        SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    }
}

void Renderer::set_window_title(const std::string& title) {
    window_title = title;
    if (IsWindowReady()) {
        SetWindowTitle(window_title.c_str());
    }
}

void Renderer::update(AssetManager& assets, float volume, MenuBar& menuBar, SpriteEditor& spriteEditor, ShaderEditor& shaderEditor, Shake& shake) {
    const float delta_time = Utils::get_delta_time();

    update_dpi_scale(delta_time);
    update_animation(delta_time);
    shake.Update(delta_time);

    Rectangle avatar_rect = calculate_avatar_rect(400);

    if (!spriteEditor.IsOpen() && !shaderEditor.IsOpen() && !show_shake_settings) {
        handle_mouse_drag(avatar_rect);
    }
    BeginDrawing();

    if (is_ui_visible) {
        ClearBackground({20, 20, 20, 255});
    } else {
        ClearBackground({0, 0, 0, 0});
    }

    bool current_blink_state = anim.is_blink();
    Vector2 shakeOffset = shake.GetOffset();
    render_avatar(assets, avatar_rect, current_blink_state, shaderEditor, shakeOffset);

    if (is_ui_visible) {
        render_ui(volume);
        
        MenuBarAction action = menuBar.Draw();

        if (action.openIdleEditor)   spriteEditor.Open(AvatarState::IDLE);
        if (action.openTalkEditor)   spriteEditor.Open(AvatarState::TALKING);
        if (action.openScreamEditor) spriteEditor.Open(AvatarState::SCREAMING);
        if (action.openShaderEditor) shaderEditor.Open();
        if (action.openShakeSettings) show_shake_settings = !show_shake_settings;

        
        if (spriteEditor.IsOpen()) {
            spriteEditor.Draw(assets);
        }
        
        if (shaderEditor.IsOpen()) {
            shaderEditor.Draw();
        }
        
        if (show_shake_settings) {
            draw_shake_settings();
        }
    }

    EndDrawing();
}

void Renderer::update_dpi_scale(float delta_time) {
    update_dpi_scale_timer -= delta_time;
    if (update_dpi_scale_timer > 0.0f) return;

    update_dpi_scale_timer = 0.5f;

    void* hwnd = GetWindowHandle();
    if (!hwnd) return;

    Vector2 current_pos = GetWindowPosition();
    int current_w = GetScreenWidth();

    if (last_window_position.x == current_pos.x &&
        last_window_position.y == current_pos.y &&
        last_render_width == current_w) {
        return;
    }

    last_window_position = current_pos;
    last_render_width = current_w;

    last_dpi_scale = winutils.get_dpi_scale(hwnd);
}

void Renderer::update_animation(float delta_time) {
    anim.update(delta_time);
}

void Renderer::draw_shake_settings() {
    if (!show_shake_settings) {
        return;
    }

    Config& cfg = ConfigManager::Get();
    Vector2 mouse = GetMousePosition();

    float x = windowRect.x;
    float y = windowRect.y;
    float w = windowRect.width;
    float h = windowRect.height;

    Rectangle headerRect = { x, y, w, 30.0f };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, headerRect)) {
        isDragging = true;
        dragOffset = { mouse.x - x, mouse.y - y };
    }

    if (isDragging) {
        windowRect.x = mouse.x - dragOffset.x;
        windowRect.y = mouse.y - dragOffset.y;
        
        x = windowRect.x;
        y = windowRect.y;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            isDragging = false;
        }
    }

    DrawRectangleRec(Rectangle{x, y, w, h}, Color{28, 28, 32, 255});
    DrawRectangleLinesEx(Rectangle{x, y, w, h}, 1.0f, Color{90, 90, 100, 255});
    
    DrawRectangleRec(headerRect, isDragging ? Color{60, 60, 70, 255} : Color{45, 45, 50, 255});
    DrawText("Shake Settings", static_cast<int>(x + 14.0f), static_cast<int>(y + 6.0f), 20, RAYWHITE);

    if (render_button(Rectangle{x + w - 30.0f, y + 4.0f, 22.0f, 22.0f}, "X", 1.0f)) {
        show_shake_settings = false;
        return;
    }

    DrawText("Strength", static_cast<int>(x + 14.0f), static_cast<int>(y + 52.0f), 18, RAYWHITE);
    if (render_button(Rectangle{x + 120.0f, y + 48.0f, 28.0f, 24.0f}, "-", 1.0f)) {
        cfg.shakeStrength = std::max(0.0f, cfg.shakeStrength - 0.5f);
        ConfigManager::Save();
    }
    if (render_button(Rectangle{x + 152.0f, y + 48.0f, 28.0f, 24.0f}, "+", 1.0f)) {
        cfg.shakeStrength += 0.5f;
        ConfigManager::Save();
    }
    DrawText(TextFormat("%.2f", cfg.shakeStrength), static_cast<int>(x + 190.0f), static_cast<int>(y + 50.0f), 18, LIGHTGRAY);

    DrawText("Duration", static_cast<int>(x + 14.0f), static_cast<int>(y + 88.0f), 18, RAYWHITE);
    if (render_button(Rectangle{x + 120.0f, y + 84.0f, 28.0f, 24.0f}, "-", 1.0f)) {
        cfg.shakeDuration = std::max(0.0f, cfg.shakeDuration - 0.05f);
        ConfigManager::Save();
    }
    if (render_button(Rectangle{x + 152.0f, y + 84.0f, 28.0f, 24.0f}, "+", 1.0f)) {
        cfg.shakeDuration += 0.05f;
        ConfigManager::Save();
    }
    DrawText(TextFormat("%.2f", cfg.shakeDuration), static_cast<int>(x + 190.0f), static_cast<int>(y + 86.0f), 18, LIGHTGRAY);

    DrawText("Mode", static_cast<int>(x + 14.0f), static_cast<int>(y + 124.0f), 18, RAYWHITE);

    const bool xSel = cfg.shakeMode == Config::ShakeMode::X;
    const bool ySel = cfg.shakeMode == Config::ShakeMode::Y;
    const bool xySel = cfg.shakeMode == Config::ShakeMode::XY;

    if (render_button(Rectangle{x + 120.0f, y + 120.0f, 40.0f, 24.0f}, "X", xSel)) {
        cfg.shakeMode = Config::ShakeMode::X;
        ConfigManager::Save();
    }
    if (render_button(Rectangle{x + 166.0f, y + 120.0f, 40.0f, 24.0f}, "Y", ySel)) {
        cfg.shakeMode = Config::ShakeMode::Y;
        ConfigManager::Save();
    }
    if (render_button(Rectangle{x + 212.0f, y + 120.0f, 60.0f, 24.0f}, "X+Y", xySel)) {
        cfg.shakeMode = Config::ShakeMode::XY;
        ConfigManager::Save();
    }

    DrawText("Triggered on state change.", static_cast<int>(x + 14.0f), static_cast<int>(y + 168.0f), 15, Color{170, 170, 180, 255});
}

bool Renderer::should_close() const {
    return WindowShouldClose();
}

void Renderer::shutdown() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}