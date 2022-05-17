#version 450

const vec2 iResolution = vec2(1280.0, 720.0);

vec2 positions[6] = vec2[](
    vec2(0.5, 0.0),
    vec2(-0.5, 0.0),
    vec2(0.0, -0.5),

    vec2(0.5, 0.0),
    vec2(0.0, 0.5),
    vec2(-0.5, 0.0)
);

vec3 colors[6] = vec3[] (
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0),

    vec3(1.0, 0.0, 0.0),
    vec3(1.0, 1.0, 1.0),
    vec3(0.0, 1.0, 0.0)
);

layout (location = 0) out vec3 pos;

void main() {
    pos = colors[gl_VertexIndex];
    vec2 outVert = positions[gl_VertexIndex];
    outVert.y *= (iResolution.x / iResolution.y);
    gl_Position = vec4(outVert, 0.0, 1.0);
}