#version 450 core

layout (location = 0) out vec4 fragColor;

layout(location = 0) in VARYING  {
    vec4 color;
    vec2 texCoords;
} fs_in;

layout(set=0, binding=0) uniform sampler2D tex;

void main() {
    fragColor = fs_in.color * texture(tex, fs_in.texCoords);
}