#include <cmath>
#include <sstream>
#include <iomanip>
#include <string>
#include "Renderer.hpp"
#include "Utils.hpp"

Renderer::Renderer(int width, int height, const std::string& title)
    : is_ui_visible     (true),
      window_width      (width),
      window_height     (height),
      idle_max_frames   (0),
      talk_max_frames   (0),
      scream_max_frames (0),
      window_title      (title),
      current_state     (AvatarState::IDLE) {}

Renderer::~Renderer() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}

Rectangle Renderer::calculate_avatar_rect(int base_size) {
    float size = base_size * last_dpi_scale * avatar_scale;

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
        case AvatarState::IDLE:
            return {100, 200, 100, 255};
        case AvatarState::TALKING:
            return {100, 150, 255, 255};
        case AvatarState::SCREAMING:
            return {255, 100, 100, 255};
        default:
            return WHITE;
    }
}

void Renderer::handle_mouse_drag(const Rectangle& avatar_rect) {
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
        FLAG_WINDOW_ALWAYS_RUN  |
        FLAG_WINDOW_TRANSPARENT |
        FLAG_WINDOW_RESIZABLE   |
        FLAG_WINDOW_TOPMOST
    );

    InitWindow(
        window_width,
        window_height,
        window_title.c_str()
    );

    if (!IsWindowReady()) return false;

    SetTargetFPS      (0);
    set_ui_visibility (true);
    return true;
}

void Renderer::render_avatar(AssetManager& assets, const Rectangle& avatar_rect, bool current_blink_state) {
    Texture2D avatar_texture = assets.get_avatar_frame(
        current_state, 
        anim.get_current_frame(), 
        current_blink_state
    );
    
    if (avatar_texture.id > 0) {
        DrawTexturePro(
            avatar_texture,
            {0, 0, (float)avatar_texture.width, (float)avatar_texture.height},
            avatar_rect,
            {0, 0},
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(avatar_rect, {80, 80, 80, 255});
        DrawRectangleLinesEx(avatar_rect, 2, {150, 150, 150, 255});
        DrawText("No avatar loaded", 
                 (int)avatar_rect.x - 70, 
                 (int)avatar_rect.y, 
                 (int)(FONT_SIZE * last_dpi_scale), 
                 TEXT_COLOR);
    }
}

void Renderer::render_fps(int screen_width, float dpi, int font_size) {
    fps_update_timer += GetFrameTime();

    if (fps_update_timer >= 0.1f || cached_fps_x == 0) {
        fps_update_timer = 0;
        int current_fps = GetFPS();
        snprintf(fps_buffer, sizeof(fps_buffer), "FPS: %d", current_fps);
        
        float padding = PADDING * dpi;
        int text_width = MeasureText(fps_buffer, font_size);
        cached_fps_x = screen_width - text_width - (int)(padding * 1.5f);
    }
    
    DrawText(fps_buffer, cached_fps_x, (int)(PADDING * dpi), font_size, TEXT_COLOR);
}

void Renderer::render_ui(float volume) {
    int current_height = GetScreenHeight();
    Rectangle avatar_rect = calculate_avatar_rect(400);

    float dpi = last_dpi_scale;
    float padding = PADDING * dpi;
    float font_size = FONT_SIZE * dpi;
    float line_height = 35 * dpi;
    float btnSize = 50 * dpi;
    float btnX = padding + 220 * dpi;
    
    float y_offset = padding;

    bool mouse_on_ui = is_ui_visible && (GetMouseX() < (padding + 350 * dpi) && GetMouseY() < (y_offset + btnSize * 2)); 
    if (!mouse_on_ui) {
        handle_mouse_drag(avatar_rect);
    }
    
    char volume_text[32];
    snprintf(volume_text, sizeof(volume_text), "Volume: %.2f", volume * sensitivity);
    DrawText(volume_text, (int)padding, (int)y_offset, (int)font_size, TEXT_COLOR);

    render_volume_bar(volume, (int)(padding + 150 * dpi), (int)y_offset);
    
    y_offset += line_height;

    char sensitivity_text[32];
    snprintf(sensitivity_text, sizeof(sensitivity_text), "Sensitivity: %.2f", sensitivity);
    DrawText(sensitivity_text, (int)padding, (int)y_offset, (int)font_size, TEXT_COLOR);

    is_anything_pressed = false;
    if (render_button({ btnX, y_offset - (btnSize/4), btnSize, btnSize }, "-", dpi)) sensitivity -= 10.0f * GetFrameTime();
    if (render_button({ btnX + btnSize + 5, y_offset - (btnSize/4), btnSize, btnSize }, "+", dpi)) sensitivity += 10.0f * GetFrameTime();

    if (sensitivity < 0.0f) sensitivity = 0.0f;
    if (sensitivity > 100.0f) sensitivity = 100.0f;
    
    render_tips(current_height, dpi, (int)font_size);

    // SetWindowTitle(TextFormat("SimpleVirtualAvatar | FPS: %d", GetFPS()));
    // render_fps(current_width, dpi, (int)font_size);
}

bool Renderer::render_button(Rectangle bounds, const char* text, float dpi) {
    if (!is_ui_visible) return false;

    bool hovered = CheckCollisionPointRec(GetMousePosition(), bounds);
    bool pressed = false;

    Color col = hovered ? LIGHTGRAY : GRAY;

    if (hovered && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        col = DARKGRAY;
        pressed = true;

        is_anything_pressed = true;
    }

    DrawRectangleRec(bounds, col);
    DrawRectangleLinesEx(bounds, 1, WHITE);
    
    int fontSize = (int)(20 * dpi);
    int textWidth = MeasureText(text, fontSize);
    DrawText(text, 
             static_cast<int>(bounds.x + (bounds.width - textWidth) / 2), 
             static_cast<int>(bounds.y + (bounds.height - fontSize) / 2), 
             fontSize, WHITE);

    return pressed;
}

void Renderer::render_volume_bar(float volume, int bar_x, int bar_y) {
    float dpi = last_dpi_scale;
    int bar_width = (int)(VOLUME_BAR_WIDTH * dpi);
    int bar_height = (int)(VOLUME_BAR_HEIGHT * dpi);
    
    DrawRectangle(bar_x, bar_y, bar_width, bar_height, BAR_BG_COLOR);
    
    float normalized_volume = volume * sensitivity;
    if (normalized_volume < 0.0f) normalized_volume = 0.0f;
    if (normalized_volume > 1.0f) normalized_volume = 1.0f;

    int filled_width = (int)(bar_width * normalized_volume);
    
    Color bar_color = BAR_FG_COLOR;
    if (normalized_volume > 0.7f) bar_color = {255, 200, 100, 255};
    if (normalized_volume > 0.9f) bar_color = {255, 100, 100, 255};
    
    DrawRectangle(bar_x, bar_y, filled_width, bar_height, bar_color);
    DrawRectangleLines(bar_x, bar_y, bar_width, bar_height, TEXT_COLOR);
}


void Renderer::render_tips(int screen_height, float dpi, int font_size) {
    const char* tips[] = {
        "F10/F11: Adjust Sensitivity",
        "F12: Toggle UI"
    };
    
    float padding = PADDING * dpi;
    for (int i = 0; i < 2; i++) {
        int y = screen_height - (int)(padding) - (2 - i) * (int)(25 * dpi);
        DrawText(tips[i], 
                 (int)padding, 
                 y, 
                 (int)(font_size - 4 * dpi), 
                 UI_TIPS_COLOR);
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
    idle_max_frames = idle_max;
    talk_max_frames = talk_max;
    scream_max_frames = scream_max;
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

void Renderer::update(AssetManager& assets, float volume) {
    float delta_time = Utils::get_delta_time();

    update_dpi_scale(delta_time);

    if (GetFPS() > max_fps) std::cout << "\r\n" << max_fps;
    max_fps = std::max(max_fps, GetFPS());

    update_animation(delta_time);

    bool current_blink_state = anim.is_blink();

    BeginDrawing();
    if (is_ui_visible) {
        ClearBackground({20, 20, 20, 255});
    } else {
        ClearBackground({0, 0, 0, 0});
    }
    
    Rectangle avatar_rect = calculate_avatar_rect(400);

    render_avatar(assets, avatar_rect, current_blink_state);
    
    if (is_ui_visible) {
        render_ui(volume);
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
        last_render_width == current_w) return;

    last_window_position = current_pos;
    last_render_width = current_w;
    
    last_dpi_scale = winutils.get_dpi_scale(hwnd);
}

void Renderer::update_animation(float delta_time) {
    anim.update(delta_time);
}

bool Renderer::should_close() const {
    return WindowShouldClose();
}

void Renderer::shutdown() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}
