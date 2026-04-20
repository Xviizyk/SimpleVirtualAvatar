#include "Renderer.hpp"
#include "Utils.hpp"
#include <cmath>
#include <sstream>
#include <iomanip>

#ifdef _WIN32
    #undef Rectangle
#endif

Renderer::Renderer(int width, int height, const std::string& title)
    : window_width(width),
      window_height(height),
      window_title(title),
      is_ui_visible(true),
      current_state(AvatarState::IDLE),
      is_blinking(false) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_TRANSPARENT | FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_TOPMOST);

    InitWindow(window_width, window_height, window_title.c_str());

    int monitor = GetCurrentMonitor();
    SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
    SetWindowPosition(0, 0);
    
    if (!IsWindowReady()) {
        return false;
    }
    
    SetTargetFPS(60);
    
    return true;
}

void Renderer::shutdown() {
    if (IsWindowReady()) {
        CloseWindow();
    }
}

bool Renderer::should_close() const {
    return WindowShouldClose();
}

void Renderer::begin_frame() {
    BeginDrawing();
    ClearBackground(BACKGROUND_COLOR);
}

void Renderer::end_frame() {
    EndDrawing();
}

void Renderer::update_animation(float delta_time) {
    anim.update(delta_time);
}

void Renderer::draw_avatar(AssetManager& assets, float volume, float sensitivity) {
    float delta_time = Utils::get_delta_time();
    update_animation(delta_time);
    
    begin_frame();
    
    int avatar_width = 400;
    int avatar_height = 400;
    Rectangle avatar_rect = {
        (float)(window_width - avatar_width) / 2.0f,
        (float)(window_height - avatar_height) / 2.0f,
        (float)avatar_width,
        (float)avatar_height
    };
    
    render_avatar(assets, avatar_rect);
    SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    if (is_ui_visible) {
        render_ui(volume, sensitivity);
    }
    
    end_frame();
}

void Renderer::render_avatar(AssetManager& assets, const Rectangle& avatar_rect) {
    Texture2D avatar_texture = assets.get_avatar_frame(current_state, anim.get_current_frame(), is_blinking);
    
    if (avatar_texture.id > 0) {
        DrawTexturePro(
            avatar_texture,
            {0, 0, (float)avatar_texture.width, (float)avatar_texture.height},
            avatar_rect,
            {avatar_rect.width / 2, avatar_rect.height / 2},
            0.0f,
            WHITE
        );
    } else {
        DrawRectangleRec(avatar_rect, {80, 80, 80, 255});
        DrawRectangleLinesEx(avatar_rect, 2, {150, 150, 150, 255});
        DrawText("No avatar loaded", 
                 (int)avatar_rect.x + 50, 
                 (int)avatar_rect.y + 180, 
                 FONT_SIZE, 
                 TEXT_COLOR);
    }
}

void Renderer::render_ui(float volume, float sensitivity) {
    ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    std::string state_text = "State: " + Utils::get_name_of_state_by_number(static_cast<int>(current_state));
    DrawText(state_text.c_str(), PADDING, PADDING, FONT_SIZE, TEXT_COLOR);
    
    std::ostringstream volume_stream;
    volume_stream << std::fixed << std::setprecision(2) << "Volume: " << volume;
    DrawText(volume_stream.str().c_str(), PADDING, PADDING + 30, FONT_SIZE, TEXT_COLOR);
    
    render_volume_bar(volume);
    
    std::ostringstream sensitivity_stream;
    sensitivity_stream << std::fixed << std::setprecision(2) << "Sensitivity: " << sensitivity;
    DrawText(sensitivity_stream.str().c_str(), PADDING, PADDING + 60, FONT_SIZE, TEXT_COLOR);
    
    render_tips();
    
    render_fps();
}

void Renderer::render_volume_bar(float volume) {
    int bar_x = PADDING + 120;
    int bar_y = PADDING + 30;
    
    DrawRectangle(bar_x, bar_y, VOLUME_BAR_WIDTH, VOLUME_BAR_HEIGHT, BAR_BG_COLOR);
    
    float normalized_volume = volume > 1.0f ? 1.0f : (volume < 0.0f ? 0.0f : volume);
    int filled_width = (int)(VOLUME_BAR_WIDTH * normalized_volume);
    
    Color bar_color = BAR_FG_COLOR;
    if (normalized_volume > 0.7f) {
        bar_color = {255, 200, 100, 255};
    }
    if (normalized_volume > 0.9f) {
        bar_color = {255, 100, 100, 255};
    }
    
    DrawRectangle(bar_x, bar_y, filled_width, VOLUME_BAR_HEIGHT, bar_color);
    DrawRectangleLines(bar_x, bar_y, VOLUME_BAR_WIDTH, VOLUME_BAR_HEIGHT, TEXT_COLOR);
}

void Renderer::render_tips() {
    const char* tips[] = {
        "F10/F11': Adjust Sensitivity",
        "F12: Toggle UI"
    };
    
    for (int i = 0; i < 2; i++) {
        DrawText(tips[i], 
                 PADDING, 
                 window_height - PADDING - (2 - i) * 25, 
                 FONT_SIZE - 4, 
                 UI_TIPS_COLOR);
    }
}

void Renderer::render_fps() {
    std::string fps_text = "FPS: " + std::to_string(GetFPS());
    DrawText(fps_text.c_str(), window_width - 150, PADDING, FONT_SIZE, TEXT_COLOR);
}

void Renderer::toggle_ui_visibility() {
    is_ui_visible = !is_ui_visible;
}

void Renderer::set_ui_visibility(bool visible) {
    is_ui_visible = visible;
}

bool Renderer::get_ui_visibility() const {
    return is_ui_visible;
}

void Renderer::set_max_frames(int idle_max, int talk_max, int scream_max) {
    idle_max_frames = idle_max;
    talk_max_frames = talk_max;
    scream_max_frames = scream_max;
}

void Renderer::set_avatar_state(AvatarState state) {
    if (current_state != state) {
        current_state = state;
        anim.reset_frame();
    }
}

AvatarState Renderer::get_avatar_state() const {
    return current_state;
}

void Renderer::set_window_title(const std::string& title) {
    window_title = title;
    if (IsWindowReady()) {
        SetWindowTitle(window_title.c_str());
    }
}

void Renderer::set_is_blinking(bool blinking) {
    is_blinking = blinking;
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
