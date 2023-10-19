#version 460
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#include "tonemappers.glsl"
#include "bindings.glsl"

layout(location = 0) out vec4 outColor;

layout(location = 9) in vec3 nearPoint;
layout(location = 10) in vec3 farPoint;


float ddxy(float x) {
    return abs(dFdx(x)) + abs(dFdy(x));
}

vec4 grid(vec3 pos) {
    vec2 coord = pos.xz;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.5, 0.5, 0.5, 1.0 - min(line, 1.0));

    if(abs(pos.x) < minimumx)
        color.y = 1;
    if(abs(pos.z) < minimumz)
        color.x = 1;
    return color;
}

float fadeFactor(vec3 pos) {
    float z = (RenderPassUBO.data.proj * RenderPassUBO.data.view * vec4(pos.xyz, 1.0)).z;
    // Empirical values are used to determine when to cut off the grid before moire patterns become visible.
    return z * 1.0 - 0.5;
}


void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 pos = nearPoint + t * (farPoint - nearPoint);

    // Display only the lower plane
    if(t < 1) {
        vec4 gridColor = grid(pos);
        outColor = vec4(gridColor.xyz, gridColor.w * fadeFactor(pos) * smoothstep(0.1, 0.01, length(pos.xz - RenderPassUBO.data.viewPosition.xz) * 0.002));
    }
    else
        outColor = vec4(0.0, 0.0, 0.0, 0.0);
}