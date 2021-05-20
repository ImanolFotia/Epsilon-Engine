#version 440 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 uv;

uniform mat4 model;
out vec2 UV;

void main() {
    gl_Position = model * vec4(pos, 1.0);
	UV = uv;
}