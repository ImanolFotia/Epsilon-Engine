#version 460
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#include "tonemappers.glsl"
#include "bindings.glsl"

layout(location = 0) out vec4 outColor;
layout (location = 1) out vec2 velocityBuffer;

layout (location=10) in vec4 Position;
layout (location=11) in vec4 PrevPosition;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
void main() {
    vec3 col = textureLod(textures[0], SampleSphericalMap(normalize(vec3(-fs_in.position.x, -fs_in.position.y, -fs_in.position.z))), 0).rgb;
    outColor = vec4(tonemapACES(col), 1.0);

    vec2 a = (Position.xy / Position.w) * 0.5 + 0.5;
    vec2 b = (PrevPosition.xy / PrevPosition.w) * 0.5 + 0.5;

    velocityBuffer = vec2(a - b);
}