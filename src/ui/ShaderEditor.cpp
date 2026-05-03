#include "ShaderEditor.hpp"
#include "../utils/OsUtils.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>

namespace {
    bool ui_button(const Rectangle& r, const char* text, bool selected = false) {
        const bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        Color fill = selected ? Color{ 80,  90, 120, 255 } : hovered  ? Color{ 75,  75,  90, 255 } : Color{ 55,  55,  65, 255 };
        DrawRectangleRec(r, fill);
        DrawRectangleLinesEx(r, 1.0f, Color{ 120, 120, 130, 255 });

        const int fs = static_cast<int>(r.height * 0.58f);
        const int tw = MeasureText(text, fs);
        DrawText(text,
                 static_cast<int>(r.x + (r.width  - tw) / 2.0f),
                 static_cast<int>(r.y + (r.height - fs) / 2.0f),
                 fs, RAYWHITE);

        return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    bool ui_minus_plus(const Rectangle& minus_r, const Rectangle& plus_r, float& value, float step, float minV, float maxV) {
        bool changed = false;
        if (ui_button(minus_r, "-")) { value = std::max(minV, value - step); changed = true; }
        if (ui_button(plus_r,  "+")) { value = std::min(maxV, value + step); changed = true; }
        return changed;
    }
}

ShaderEditor::ShaderEditor()  = default;
ShaderEditor::~ShaderEditor() { unload(); }

void ShaderEditor::open_window() { open = true; needsCompile = true; }
bool ShaderEditor::is_open() const { return open; }
bool ShaderEditor::has_shader() const { return shaderReady && shader.id > 0; }
bool ShaderEditor::is_enabled() const { return enabled && has_shader(); }
const Shader& ShaderEditor::get_shader() const { return shader; }

void ShaderEditor::unload() {
    if (shaderReady && shader.id > 0) UnloadShader(shader);
    shader = Shader{ 0 };
    shaderReady = false;
    locTime = -1;
    locIntensity = -1;
    locSpeed = -1;
}

void ShaderEditor::draw(float dpi)
{
    if (!open) return;

    const float s = dpi;
    const float w = BASE_W * s;
    const float headerH = 30.0f * s;
    
    const int totalItems = 4 + static_cast<int>(userShaderPaths.size());
    const int rows = (totalItems + 3) / 4; 
    const float slotH = 35.0f * s;
    const float slotMargin = 8.0f * s;
    
    const float libraryHeaderH = 40.0f * s;
    const float libraryHeight = rows * (slotH + slotMargin) + libraryHeaderH;
    const float h = std::max(BASE_H * s, 180.0f * s + libraryHeight);
    
    const float x = windowPos.x;
    const float y = windowPos.y;
    const Vector2 mouse = GetMousePosition();
    const Rectangle headerRect = { x, y, w, headerH };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, headerRect)) {
        isDragging = true;
        dragOffset = { mouse.x - x, mouse.y - y };
    }
    if (isDragging) {
        windowPos.x = mouse.x - dragOffset.x;
        windowPos.y = mouse.y - dragOffset.y;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isDragging = false;
    }

    DrawRectangleRec({ x, y, w, h }, { 28, 28, 32, 255 });
    DrawRectangleLinesEx({ x, y, w, h }, 1.0f, { 90, 90, 100, 255 });
    DrawRectangleRec(headerRect, isDragging ? Color{ 60, 60, 70, 255 } : Color{ 45, 45, 50, 255 });

    const int titleFS = static_cast<int>(16.0f * s);
    DrawText("Shader Editor", static_cast<int>(x + 14.0f * s), static_cast<int>(y + (headerH - titleFS) / 2.0f), titleFS, RAYWHITE);

    const float btnSz = 22.0f * s;
    if (ui_button({ x + w - (btnSz + 6.0f * s), y + (headerH - btnSz) / 2.0f, btnSz, btnSz }, "X")) {
        open = false; 
        return;
    }

    const auto lx = [&](float v) { return x + v * s; };
    const auto ly = [&](float v) { return y + v * s; };
    const int fs  = static_cast<int>(14.0f * s);

    if (ui_button({ lx(14), ly(45), 82 * s, 24 * s }, "Enabled", enabled)) {
        enabled = !enabled;
    }

    DrawText("Intensity", static_cast<int>(lx(14)), static_cast<int>(ly(80) + (22 * s - fs) / 2.0f), fs, RAYWHITE);
    ui_minus_plus({ lx(108), ly(80), 24 * s, 22 * s }, { lx(136), ly(80), 24 * s, 22 * s }, intensity, 0.1f, 0.0f, 2.5f);
    DrawText(TextFormat("%.2f", intensity), static_cast<int>(lx(168)), static_cast<int>(ly(80) + (22 * s - fs) / 2.0f), fs, LIGHTGRAY);

    DrawText("Speed", static_cast<int>(lx(14)), static_cast<int>(ly(115) + (22 * s - fs) / 2.0f), fs, RAYWHITE);
    ui_minus_plus({ lx(108), ly(115), 24 * s, 22 * s }, { lx(136), ly(115), 24 * s, 22 * s }, speed, 0.1f, 0.1f, 6.0f);
    DrawText(TextFormat("%.2f", speed), static_cast<int>(lx(168)), static_cast<int>(ly(115) + (22 * s - fs) / 2.0f), fs, LIGHTGRAY);

    if (ui_button({ x + w - 130 * s, ly(80), 116 * s, 57 * s }, "Apply")) {
        needsCompile = true;
    }

    const float libStartY = ly(160.0f);
    DrawRectangle(static_cast<int>(lx(14)), static_cast<int>(libStartY - 10 * s), static_cast<int>(w - 28 * s), 1, Color{ 50, 50, 62, 255 });
    DrawText("SHADER LIBRARY", static_cast<int>(lx(14)), static_cast<int>(libStartY), static_cast<int>(12 * s), GRAY);

    const float slotW = (w - 56.0f * s) / 4.0f;
    const std::array<const char*, 4> presetLabels = { "Wave", "RGB", "Scan", "CRT" };

    for (int i = 0; i < 4; ++i) {
        Rectangle r = { lx(14) + i * (slotW + 8 * s), libStartY + 20 * s, slotW, slotH };
        if (ui_button(r, presetLabels[i], (!useCustomShader && presetIndex == i))) {
            presetIndex = i; 
            useCustomShader = false; 
            needsCompile = true;
        }
    }

    for (int i = 0; i < static_cast<int>(userShaderPaths.size()); ++i) {
        int idx = i + 4;
        int row = idx / 4;
        int col = idx % 4;
        Rectangle r = { lx(14) + col * (slotW + 8 * s), libStartY + 20 * s + row * (slotH + slotMargin), slotW, slotH };

        std::string name = std::filesystem::path(userShaderPaths[i]).stem().string();
        if (name.length() > 8) name = name.substr(0, 6) + "..";

        bool isSelected = (useCustomShader && customShaderPath == userShaderPaths[i]);
        
        if (ui_button(r, name.c_str(), isSelected)) {
            if (load_custom_shader_from_file(userShaderPaths[i])) {
                needsCompile = true;
            }
        }

        Rectangle delRect = { r.x + r.width - 14*s, r.y, 14*s, 14*s };
        if (CheckCollisionPointRec(mouse, r) && ui_button(delRect, "x")) {
            userShaderPaths.erase(userShaderPaths.begin() + i);
            if (isSelected) { useCustomShader = false; needsCompile = true; }
            break; 
        }
    }

    int nextIdx = 4 + static_cast<int>(userShaderPaths.size());
    Rectangle addRect = { 
        lx(14) + (nextIdx % 4) * (slotW + 8 * s), 
        libStartY + 20 * s + (nextIdx / 4) * (slotH + slotMargin), 
        slotW, slotH 
    };
    
    if (ui_button(addRect, "+ New")) {
        std::string file;
        if (OsUtils::open_file_dialog(GetWindowHandle(), file, "Shaders")) {
            userShaderPaths.push_back(file);
            load_custom_shader_from_file(file);
            needsCompile = true;
        }
    }

    if (needsCompile) { 
        compile(); 
        needsCompile = false; 
    }

    #ifdef _DEBUG
        const char* statusText = shaderReady ? "Status: OK" : "Status: ERROR";
        const char* currentInfo = useCustomShader ? customShaderPath.c_str() : preset_name();
        
        DrawText(statusText, static_cast<int>(lx(14)), static_cast<int>(y + h - 22 * s), static_cast<int>(12 * s), shaderReady ? GREEN : RED);
        
        if (shaderReady) {
            DrawText(TextFormat("| Active: %s", currentInfo), 
                     static_cast<int>(lx(100)), 
                     static_cast<int>(y + h - 22 * s), 
                     static_cast<int>(12 * s), GRAY);
        }
    #endif
}

void ShaderEditor::apply_uniforms(float time) {
    if (!is_enabled()) return;
    if (locTime      >= 0) SetShaderValue(shader, locTime,      &time,      SHADER_UNIFORM_FLOAT);
    if (locIntensity >= 0) SetShaderValue(shader, locIntensity, &intensity, SHADER_UNIFORM_FLOAT);
    if (locSpeed     >= 0) SetShaderValue(shader, locSpeed,     &speed,     SHADER_UNIFORM_FLOAT);
}

/*static*/ const char* ShaderEditor::vertex_source() {
    return R"(#version 330
              in vec3 vertexPosition;
              in vec2 vertexTexCoord;
              in vec4 vertexColor;
              out vec2 fragTexCoord;
              out vec4 fragColor;
              uniform mat4 mvp;
              void main() {
                  fragTexCoord = vertexTexCoord;
                  fragColor    = vertexColor;
                  gl_Position  = mvp * vec4(vertexPosition, 1.0);
              }
            )";
}

bool ShaderEditor::load_custom_shader_from_file(const std::string& path)
{
    if (!std::filesystem::exists(path)) return false;

    std::ifstream file(path);
    if (!file.is_open()) return false;

    customFragmentSource = std::string(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    customShaderPath = path;
    useCustomShader = true;

    return true;
}   

const char* ShaderEditor::preset_name() const {
    switch (presetIndex) {
        case 0: return "Wave";
        case 1: return "RGB Shift";
        case 2: return "Scanlines";
        case 3: return "CRT";
    }
    return "Wave";
}

std::string ShaderEditor::build_fragment_source() const {
    if (useCustomShader && !customFragmentSource.empty()) {
        return customFragmentSource;
    }

    switch (presetIndex) {
        case 1: return R"(#version 330
                           in vec2 fragTexCoord; in vec4 fragColor; out vec4 finalColor;
                           uniform sampler2D texture0; uniform float time; uniform float intensity; uniform float speed;
                           void main() {
                               vec2 uv = fragTexCoord;
                               float shift = 0.0025 * intensity * sin(time * speed * 5.0);
                               vec4 base = texture(texture0, uv) * fragColor;
                               float r = texture(texture0, uv + vec2(shift, 0.0)).r;
                               float b = texture(texture0, uv - vec2(shift, 0.0)).b;
                               finalColor = vec4(r, base.g, b, base.a);
                           }
                        )";

        case 2: return R"(#version 330
                           in vec2 fragTexCoord; in vec4 fragColor; out vec4 finalColor;
                           uniform sampler2D texture0; uniform float time; uniform float intensity; uniform float speed;
                           void main() {
                               vec2 uv = fragTexCoord;
                               vec4 base = texture(texture0, uv) * fragColor;
                               float scan = sin((uv.y * 900.0) + time * speed * 8.0) * 0.05 * intensity;
                               finalColor = vec4(base.rgb - scan, base.a);
                           }
                        )";

        case 3: return R"(#version 330
                        in vec2 fragTexCoord; in vec4 fragColor; out vec4 finalColor;
                        uniform sampler2D texture0; uniform float time; uniform float intensity; uniform float speed;
                        void main() {
                            vec2 uv = fragTexCoord - 0.5;
                            float r2 = dot(uv, uv);
                            uv *= 1.0 + r2 * 0.12 * intensity;
                            uv += 0.5;
                            vec4 base = texture(texture0, uv) * fragColor;
                            float scan    = sin((fragTexCoord.y * 1000.0) + time * speed * 10.0) * 0.03 * intensity;
                            float vignette= clamp(1.0 - r2 * 1.2, 0.2, 1.0);
                            finalColor = vec4((base.rgb - scan) * vignette, base.a);
                        }
                    )";

        case 0:
        default: return R"(#version 330
                            in vec2 fragTexCoord; in vec4 fragColor; out vec4 finalColor;
                            uniform sampler2D texture0; uniform float time; uniform float intensity; uniform float speed;
                            void main() {
                                vec2 uv = fragTexCoord;
                                uv.y += sin((uv.x * 14.0) + time * speed * 4.0)  * 0.01  * intensity;
                                uv.x += cos((uv.y * 10.0) + time * speed * 2.5)  * 0.005 * intensity;
                                finalColor = texture(texture0, uv) * fragColor;
                            }
                        )";
    }
}

bool ShaderEditor::compile() {
    Shader candidate = LoadShaderFromMemory(vertex_source(), build_fragment_source().c_str());
    if (candidate.id == 0) {
        shaderReady = false;
        return false;
    }

    unload();
    shader = candidate;
    shaderReady = true;
    locTime = GetShaderLocation(shader, "time");
    locIntensity = GetShaderLocation(shader, "intensity");
    locSpeed = GetShaderLocation(shader, "speed");
    return true;
}