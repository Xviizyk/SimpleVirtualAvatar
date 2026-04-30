#pragma once

#include "raylib.h"
#include <string>

class ShaderEditor {
public:
    ShaderEditor();
    ~ShaderEditor();

    void Open();
    bool IsOpen() const;

    void Draw();

    bool HasShader() const;
    bool IsEnabled() const;
    const Shader& GetShader() const;
    void ApplyUniforms(float time);

private:
    Rectangle windowRect = { 100, 100, 600, 400 };
    bool isDragging = false;
    bool isResizing = false;
    Vector2 dragOffset = { 0, 0 };
    const float RESIZE_BORDER = 10.0f;

    enum class Preset {
        Wave = 0,
        RGBShift = 1,
        Scanlines = 2,
        CRT = 3
    };

    bool open = false;
    bool enabled = true;
    bool shaderReady = false;
    bool needsCompile = true;

    int presetIndex = 0;
    float intensity = 1.0f;
    float speed = 1.0f;

    Shader shader{0};
    int locTime = -1;
    int locIntensity = -1;
    int locSpeed = -1;

    void Unload();
    bool Compile();
    std::string BuildFragmentSource() const;
    static const char* VertexSource();
    const char* PresetName() const;
};