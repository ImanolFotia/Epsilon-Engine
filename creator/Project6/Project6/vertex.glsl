#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 TexCoord;

//out vec2 fragCoord;

void main() {
    gl_Position = vec4(position, 1.0);

    //fragCoord = TexCoord;
}