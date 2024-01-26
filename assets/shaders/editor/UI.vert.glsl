#version 450 core


layout(location = 0) in vec4 pos_coords;
layout(location = 1) in vec4 color;

layout(location = 0) out VARYING  {
    vec4 color;
    vec2 texCoords;
} vs_out;

void main() {
    gl_Position = vec4(pos_coords.xy, 0.0, 1.0);
    vs_out.color = color;
    vs_out.texCoords = pos_coords.zw;
}