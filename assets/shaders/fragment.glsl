#version 450

layout(location = 0) out vec4 outColor;

layout (location = 0) in vec3 pos;

void main() {
    outColor = vec4(pos, 1.0);
}