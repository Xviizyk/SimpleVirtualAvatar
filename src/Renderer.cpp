#include <cmath>
#include <sstream>
#include <iomanip>
#include <string>
#include "Renderer.hpp"
#include "Utils.hpp"

Renderer::Renderer(int width, int height, const std::string& title)
    : is_ui_visible     (true),
      is_blinking       (false),
      window_width      (width),
      window_height     (height),
      idle_max_frames   (0),
      talk_max_frames   (0),
      scream_max_frames (0),
      window_title      (title),
      current_state     (AvatarState::IDLE) {}

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init() {
    // int screenWidth = GetMonitorWidth();
    // int screenHeight = GetMonitorHeight();

    SetConfigFlags(
        FLAG_WINDOW_ALWAYS_RUN  |
        FLAG_VSYNC_HINT         |
        FLAG_WINDOW_TRANSPARENT |
        FLAG_WINDOW_UNDECORATED |
        FLAG_WINDOW_TOPMOST
    );

    InitWindow(
        window_width,
        window_height,
        window_title.c_str()
    );

    if (!IsWindowReady()) return false;

    // window_width  = screenWidth;
    // window_height = screenHeight;

    // SetWindowPosition (0, 0);
    set_ui_visibility (true);
    SetTargetFPS      (60);
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
}

void Renderer::end_frame() {
    EndDrawing();
}

void Renderer::update_animation(float delta_time) {
    anim.update(delta_time);
}

Rectangle Renderer::calculate_avatar_rect(int base_size) {
    float scale = get_dpi_scale() * avatar_scale;

    float size = base_size        * scale;
    float margin = avatar_margin  * scale;

    float x = 0, y = 0;

    switch (avatar_corner) {
        case AvatarCorner::TOP_LEFT:
            x = margin;
            y = margin;
            break;

        case AvatarCorner::TOP_RIGHT:
            x = window_width - size - margin;
            y = margin;
            break;

        case AvatarCorner::BOTTOM_LEFT:
            x = margin;
            y = window_height - size - margin;
            break;

        case AvatarCorner::BOTTOM_RIGHT:
            x = window_width - size - margin;
            y = window_height - size - margin;
            break;
    }

    return { x, y, size, size };
}

void Renderer::draw_avatar(AssetManager& assets, float volume, float sensitivity) {
    float delta_time = Utils::get_delta_time();
    update_animation(delta_time);

    begin_frame();
    if (is_ui_visible) {
        ClearBackground({20, 20, 20, 255});
    } else {
        ClearBackground(BLANK);
    }
    
    Rectangle avatar_rect = calculate_avatar_rect(400);
    
    render_avatar(assets, avatar_rect);
    
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
                 (int)avatar_rect.x - 70, 
                 (int)avatar_rect.y, 
                 (int)(FONT_SIZE * get_dpi_scale()), 
                 TEXT_COLOR);
    }
}

void Renderer::render_ui(float volume, float sensitivity) {
    std::string state_text = "State: " + Utils::get_name_of_state_by_number(static_cast<int>(current_state));
    DrawText(state_text.c_str(), (int)(PADDING * get_dpi_scale()), (int)(PADDING * get_dpi_scale()), (int)(FONT_SIZE * get_dpi_scale()), TEXT_COLOR);
    
    std::ostringstream volume_stream;
    volume_stream << std::fixed << std::setprecision(2) << "Volume: " << volume;
    DrawText(volume_stream.str().c_str(), (int)(PADDING * get_dpi_scale()), (int)(PADDING * get_dpi_scale()) + 30, (int)(FONT_SIZE * get_dpi_scale()), TEXT_COLOR);
    
    render_volume_bar(volume);
    
    std::ostringstream sensitivity_stream;
    sensitivity_stream << std::fixed << std::setprecision(2) << "Sensitivity: " << sensitivity;
    DrawText(sensitivity_stream.str().c_str(), (int)(PADDING * get_dpi_scale()), (int)(PADDING * get_dpi_scale()) + 60, (int)(FONT_SIZE * get_dpi_scale()), TEXT_COLOR);
    
    render_tips();
    
    render_fps();
}

void Renderer::render_volume_bar(float volume) {
    int bar_x = (int)(PADDING * get_dpi_scale()) + 120;
    int bar_y = (int)(PADDING * get_dpi_scale()) + 30;
    
    DrawRectangle(bar_x, bar_y, (int)(VOLUME_BAR_WIDTH * get_dpi_scale()), (int)(VOLUME_BAR_HEIGHT * get_dpi_scale()), BAR_BG_COLOR);
    
    float normalized_volume = volume > 1.0f ? 1.0f : (volume < 0.0f ? 0.0f : volume);
    int filled_width = (int)(VOLUME_BAR_WIDTH * normalized_volume);
    
    Color bar_color = BAR_FG_COLOR;
    if (normalized_volume > 0.7f) {
        bar_color = {255, 200, 100, 255};
    }
    if (normalized_volume > 0.9f) {
        bar_color = {255, 100, 100, 255};
    }
    
    DrawRectangle(bar_x, bar_y, filled_width, (int)(VOLUME_BAR_HEIGHT * get_dpi_scale()), bar_color);
    DrawRectangleLines(bar_x, bar_y, (int)(VOLUME_BAR_WIDTH * get_dpi_scale()), (int)(VOLUME_BAR_HEIGHT * get_dpi_scale()), TEXT_COLOR);
}

float Renderer::get_dpi_scale() {
    void* hwnd = GetWindowHandle();
    if (!hwnd) return 1.0f;

    return winutils.get_dpi_scale(hwnd);
}

void Renderer::render_tips() {
    const char* tips[] = {
        "F10/F11: Adjust Sensitivity",
        "F12: Toggle UI"
    };
    
    for (int i = 0; i < 2; i++) {
        DrawText(tips[i], 
                 (int)(PADDING * get_dpi_scale()), 
                 window_height - (int)(PADDING * get_dpi_scale()) - (2 - i) * 25, 
                 (int)(FONT_SIZE * get_dpi_scale()) - 4, 
                 UI_TIPS_COLOR);
    }
}

void Renderer::render_fps() {
    std::string fps_text = "FPS: " + std::to_string(GetFPS());
    DrawText(fps_text.c_str(), window_width - 150, (int)(PADDING * get_dpi_scale()), (int)(FONT_SIZE * get_dpi_scale()), TEXT_COLOR);
}

void Renderer::toggle_ui_visibility() {
    set_ui_visibility(!is_ui_visible);
}

void Renderer::set_ui_visibility(bool visible) {
    if (is_ui_visible == visible) return;

    is_ui_visible = visible;

    void* hwnd = GetWindowHandle();
    if (!hwnd) return;

    if (is_ui_visible) {
        winutils.set_overlay_mode(hwnd, false);
        ClearWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    } else {
        winutils.set_overlay_mode(hwnd, true);
        SetWindowState(FLAG_WINDOW_MOUSE_PASSTHROUGH);
    }
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
