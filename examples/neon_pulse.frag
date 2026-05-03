#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
out vec4 finalColor;

uniform sampler2D texture0;
uniform float time;

void main() {
    vec4 col = texture(texture0, fragTexCoord) * fragColor;

    float pulse = sin(time * 4.0) * 0.5 + 0.5;
    col.rgb += vec3(0.2, 0.8, 1.0) * pulse * 0.3;

    finalColor = col;
}