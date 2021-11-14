#version 450 core

out vec4 FragColor;

out vec4 BrightFragColor;

in vec3 color;

void main() {
    BrightFragColor = vec4(0.0);
    FragColor = vec4(color, 1.0);
}