#pragma once

#include "raylib.h"
#include <string>

class ShaderEditor {
public:
    ShaderEditor();
    ~ShaderEditor();

    void open_window();
    bool is_open() const;

    void draw();

    bool has_shader() const;
    bool is_enabled() const;
    const Shader& get_shader() const;
    void apply_uniforms(float time);

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

    void unload();
    bool compile();
    std::string build_fragment_source() const;
    static const char* vertex_source();
    const char* preset_name() const;
};