#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;

void main() {
    vec2 uv = fragTexCoord;

    float wave = sin(uv.y * 30.0 + time * 6.0) * 0.01;
    uv.x += wave;

    finalColor = texture(texture0, uv) * fragColor;
}