#version 430 core

out vec4 FragColor;
out vec4 BrightFragColor;

uniform vec3 inColor;

uniform int subObjectId;
uniform int EntityId;

void main() {
    FragColor = vec4(inColor, 1.0);
    BrightFragColor = vec4(0.0);
}