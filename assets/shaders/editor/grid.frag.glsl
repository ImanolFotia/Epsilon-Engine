#version 460
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#include "tonemappers.glsl"
#include "bindings.glsl"

layout(location = 0) out vec4 outColor;
layout (location = 1) out vec2 velocityBuffer;
layout(location = 9) in vec3 nearPoint;
layout(location = 10) in vec3 farPoint;
layout (location=11) in vec4 Position;
layout (location=12) in vec4 PrevPosition;


float ddxy(float x) {
    return abs(dFdx(x)) + abs(dFdy(x));
}

float fadeFactor(vec3 pos) {
    float z = (RenderPassUBO.data.proj * RenderPassUBO.data.view * vec4(pos.xyz, 1.0)).z;
    // Empirical values are used to determine when to cut off the grid before moire patterns become visible.
    return z;
}

vec4 grid(vec3 pos) {
    vec2 coord = pos.xz;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord)) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.25, 0.25, 0.25, 1.0 - min(line, 1.0));

    if(abs(pos.x) < minimumx)
        color.y = 1;
    if(abs(pos.z) < minimumz)
        color.x = 1;

    color.w = clamp(color.w * fadeFactor(pos), 0.0, 1.0);
    return color;
}

void main() {

    #ifdef REVERSE_Z
    float t = farPoint.y / (nearPoint.y - farPoint.y);
    vec3 pos = -farPoint + t * (nearPoint - farPoint );
    #else
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 pos = nearPoint + t * (farPoint - nearPoint);

    #endif

    outColor = vec4(0.0, 0.0, 0.0, 0.0);
    if(t >= 0) {
        outColor = grid(pos);
    }

    vec2 a = (Position.xy / Position.w) * 0.5 + 0.5;
    vec2 b = (PrevPosition.xy / PrevPosition.w) * 0.5 + 0.5;

    velocityBuffer = vec2(0.0);
}