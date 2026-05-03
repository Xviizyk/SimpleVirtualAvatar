#pragma once

#include "raylib.h"
#include <string>
#include <filesystem>
#include <vector> // <-- Добавлено для списка шейдеров

class ShaderEditor {
public:
    ShaderEditor();
    ~ShaderEditor();

    void open_window();
    bool is_open() const;

    void draw(float dpi);

    bool has_shader() const;
    bool is_enabled() const;
    const Shader& get_shader() const;
    void apply_uniforms(float time);

private:
    Vector2 windowPos  = { 100.0f, 100.0f };
    bool isDragging = false;
    Vector2 dragOffset = { 0.0f, 0.0f };

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

    Shader shader { 0 };
    int locTime = -1;
    int locIntensity = -1;
    int locSpeed = -1;

    static constexpr float BASE_W = 450.0f;
    static constexpr float BASE_H = 248.0f;

    void unload();
    bool compile();
    std::string build_fragment_source() const;
    static const char* vertex_source();
    const char* preset_name() const;
    bool load_custom_shader_from_file(const std::string& path);

    bool useCustomShader = false;
    std::string customShaderPath;
    std::string customFragmentSource;

    std::vector<std::string> userShaderPaths;
};