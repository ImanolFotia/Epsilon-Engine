#pragma once
#include <string>

std::string fragmentshader = std::string("#version 330 core \n uniform sampler2D sampler; in vec2 TexCoords; out vec4 Color; void main() { Color = texture(sampler, vec2(-TexCoords.x, TexCoords.y), 0);}");

std::string vertexshader = std::string("#version 330 core \n layout (location = 0) in vec3 position;layout (location = 1) in vec2 UV;out vec2 TexCoords;void main(){gl_Position = vec4(position, 1.0f);TexCoords = UV;}");



