#include "ShaderEditor.hpp"

#include <algorithm>
#include <string>
#include <array>

namespace {
    static bool UiButton(const Rectangle& r, const char* text, bool selected = false) {
        const bool hovered = CheckCollisionPointRec(GetMousePosition(), r);
        Color fill = selected ? Color{80, 90, 120, 255} : (hovered ? Color{75, 75, 90, 255} : Color{55, 55, 65, 255});
        DrawRectangleRec(r, fill);
        DrawRectangleLinesEx(r, 1.0f, Color{120, 120, 130, 255});

        const int fs = 16;
        const int tw = MeasureText(text, fs);
        DrawText(text, static_cast<int>(r.x + (r.width - tw) / 2.0f), static_cast<int>(r.y + 4.0f), fs, RAYWHITE);

        return hovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    }

    static bool UiMinusPlus(const Rectangle& minusRect, const Rectangle& plusRect, float& value, float step, float minV, float maxV) {
        bool changed = false;
        if (UiButton(minusRect, "-")) {
            value = std::max(minV, value - step);
            changed = true;
        }
        if (UiButton(plusRect, "+")) {
            value = std::min(maxV, value + step);
            changed = true;
        }
        return changed;
    }
}

ShaderEditor::ShaderEditor() = default;

ShaderEditor::~ShaderEditor() {
    Unload();
}

void ShaderEditor::Open() {
    open = true;
    needsCompile = true;
}

bool ShaderEditor::IsOpen() const {
    return open;
}

bool ShaderEditor::HasShader() const {
    return shaderReady && shader.id > 0;
}

bool ShaderEditor::IsEnabled() const {
    return enabled && HasShader();
}

const Shader& ShaderEditor::GetShader() const {
    return shader;
}

void ShaderEditor::Unload() {
    if (shaderReady && shader.id > 0) {
        UnloadShader(shader);
    }
    shader = Shader{0};
    shaderReady = false;
    locTime = -1;
    locIntensity = -1;
    locSpeed = -1;
}

const char* ShaderEditor::VertexSource() {
    return R"(#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec4 vertexColor;

out vec2 fragTexCoord;
out vec4 fragColor;

uniform mat4 mvp;

void main()
{
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    gl_Position = mvp * vec4(vertexPosition, 1.0);
}
)";
}

const char* ShaderEditor::PresetName() const {
    switch (presetIndex) {
        case 0: return "Wave";
        case 1: return "RGB Shift";
        case 2: return "Scanlines";
        case 3: return "CRT";
    }
    return "Wave";
}

std::string ShaderEditor::BuildFragmentSource() const {
    switch (presetIndex) {
        case 1:
            return R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;
uniform float intensity;
uniform float speed;

void main()
{
    vec2 uv = fragTexCoord;
    float shift = 0.0025 * intensity * sin(time * speed * 5.0);
    vec4 base = texture(texture0, uv) * fragColor;
    float r = texture(texture0, uv + vec2(shift, 0.0)).r;
    float g = base.g;
    float b = texture(texture0, uv - vec2(shift, 0.0)).b;
    finalColor = vec4(r, g, b, base.a);
}
)";
        case 2:
            return R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;
uniform float intensity;
uniform float speed;

void main()
{
    vec2 uv = fragTexCoord;
    vec4 base = texture(texture0, uv) * fragColor;
    float scan = sin((uv.y * 900.0) + time * speed * 8.0) * 0.05 * intensity;
    finalColor = vec4(base.rgb - scan, base.a);
}
)";
        case 3:
            return R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;
uniform float intensity;
uniform float speed;

void main()
{
    vec2 uv = fragTexCoord - 0.5;
    float r2 = dot(uv, uv);
    uv *= 1.0 + r2 * 0.12 * intensity;
    uv += 0.5;

    vec4 base = texture(texture0, uv) * fragColor;
    float scan = sin((fragTexCoord.y * 1000.0) + time * speed * 10.0) * 0.03 * intensity;
    float vignette = clamp(1.0 - r2 * 1.2, 0.2, 1.0);
    finalColor = vec4((base.rgb - scan) * vignette, base.a);
}
)";
        case 0:
        default:
            return R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;
uniform float intensity;
uniform float speed;

void main()
{
    vec2 uv = fragTexCoord;
    uv.y += sin((uv.x * 14.0) + time * speed * 4.0) * 0.01 * intensity;
    uv.x += cos((uv.y * 10.0) + time * speed * 2.5) * 0.005 * intensity;
    finalColor = texture(texture0, uv) * fragColor;
}
)";
    }
}

bool ShaderEditor::Compile() {
    Shader candidate = LoadShaderFromMemory(VertexSource(), BuildFragmentSource().c_str());
    if (candidate.id == 0) {
        return false;
    }

    Unload();

    shader = candidate;
    shaderReady = true;
    locTime = GetShaderLocation(shader, "time");
    locIntensity = GetShaderLocation(shader, "intensity");
    locSpeed = GetShaderLocation(shader, "speed");

    return true;
}

void ShaderEditor::ApplyUniforms(float time) {
    if (!IsEnabled()) {
        return;
    }

    if (locTime >= 0) {
        SetShaderValue(shader, locTime, &time, SHADER_UNIFORM_FLOAT);
    }
    if (locIntensity >= 0) {
        SetShaderValue(shader, locIntensity, &intensity, SHADER_UNIFORM_FLOAT);
    }
    if (locSpeed >= 0) {
        SetShaderValue(shader, locSpeed, &speed, SHADER_UNIFORM_FLOAT);
    }
}

void ShaderEditor::Draw() {
    if (!open) {
        return;
    }

    Vector2 mouse = GetMousePosition();
    Rectangle headerRect = { windowRect.x, windowRect.y, windowRect.width, 30.0f };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, headerRect)) {
        isDragging = true;
        dragOffset = { mouse.x - windowRect.x, mouse.y - windowRect.y };
    }

    if (isDragging) {
        windowRect.x = mouse.x - dragOffset.x;
        windowRect.y = mouse.y - dragOffset.y;
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) isDragging = false;
    }

    const float x = windowRect.x;
    const float y = windowRect.y;
    const float w = windowRect.width;
    const float h = windowRect.height;

    DrawRectangleRec(Rectangle{x, y, w, h}, Color{28, 28, 32, 255});
    DrawRectangleLinesEx(Rectangle{x, y, w, h}, 1.0f, Color{90, 90, 100, 255});
    
    DrawRectangleRec(headerRect, isDragging ? Color{60, 60, 70, 255} : Color{45, 45, 50, 255});
    DrawText("Shader Editor", static_cast<int>(x + 14.0f), static_cast<int>(y + 6.0f), 22, RAYWHITE);

    if (UiButton(Rectangle{x + w - 34.0f, y + 4.0f, 22.0f, 22.0f}, "X")) {
        open = false;
        return;
    }

    if (UiButton(Rectangle{x + 14.0f, y + 46.0f, 90.0f, 28.0f}, "Enabled", enabled)) {
        enabled = !enabled;
    }

    DrawText(("Preset: " + std::string(PresetName())).c_str(), static_cast<int>(x + 120.0f), static_cast<int>(y + 50.0f), 18, LIGHTGRAY);

    const std::array<const char*, 4> presetButtons = {"Wave", "RGB", "Scan", "CRT"};
    for (int i = 0; i < 4; ++i) {
        const float bx = x + 14.0f + i * 104.0f;
        if (UiButton(Rectangle{bx, y + 82.0f, 96.0f, 28.0f}, presetButtons[i], presetIndex == i)) {
            presetIndex = i;
            needsCompile = true;
        }
    }

    DrawText("Intensity", static_cast<int>(x + 14.0f), static_cast<int>(y + 132.0f), 18, RAYWHITE);
    if (UiMinusPlus(Rectangle{x + 120.0f, y + 128.0f, 28.0f, 24.0f},
                    Rectangle{x + 152.0f, y + 128.0f, 28.0f, 24.0f},
                    intensity, 0.1f, 0.0f, 2.5f)) {
    }
    DrawText(TextFormat("%.2f", intensity), static_cast<int>(x + 190.0f), static_cast<int>(y + 130.0f), 18, LIGHTGRAY);

    DrawText("Speed", static_cast<int>(x + 14.0f), static_cast<int>(y + 170.0f), 18, RAYWHITE);
    if (UiMinusPlus(Rectangle{x + 120.0f, y + 166.0f, 28.0f, 24.0f},
                    Rectangle{x + 152.0f, y + 166.0f, 28.0f, 24.0f},
                    speed, 0.1f, 0.1f, 6.0f)) {
    }
    DrawText(TextFormat("%.2f", speed), static_cast<int>(x + 190.0f), static_cast<int>(y + 168.0f), 18, LIGHTGRAY);

    if (UiButton(Rectangle{x + 290.0f, y + 160.0f, 130.0f, 34.0f}, "Compile / Apply")) {
        needsCompile = true;
    }

    if (needsCompile) {
        Compile();
        needsCompile = false;
    }

    DrawText(shaderReady ? "Shader ready." : "Shader not compiled.", 
             static_cast<int>(x + 14.0f), static_cast<int>(y + 210.0f), 16, 
             shaderReady ? Color{140, 220, 140, 255} : Color{220, 140, 140, 255});
}