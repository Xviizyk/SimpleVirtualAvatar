#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;

void main() {
    vec2 uv = fragTexCoord - 0.5;

    float angle = length(uv) * 4.0 - time * 2.0;
    float s = sin(angle);
    float c = cos(angle);

    uv = mat2(c, -s, s, c) * uv;
    uv += 0.5;

    finalColor = texture(texture0, uv) * fragColor;
}