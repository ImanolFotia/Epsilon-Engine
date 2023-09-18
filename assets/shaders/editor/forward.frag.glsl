#version 460

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "tonemappers.glsl"

layout (location = 0) out vec4 fragColor;


layout(binding = 0) uniform UniformBufferObject
{
    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
	vec3 viewPosition;
    mat4 view;
    mat4 proj;
    mat4 lightMatrix;
    int iFrame;
    ivec2 mouse;
}
ubo;


layout(location = 0) in VARYING {
    flat uint material_index;
    flat int object_id;
    vec4 color;
    vec2 texCoords;
    vec3 normal;
    vec3 position;
    mat3 TBN;
} fs_in;

void main() {
    vec3 viewDir = vec3(ubo.view[2][0], ubo.view[2][1], ubo.view[2][2]);
    float solid_shading = max(dot(fs_in.normal, normalize(ubo.viewPosition - fs_in.position)), 0.001);
    fragColor = vec4( tonemapACES(vec3(min(solid_shading, 0.8)) * 0.4), 1.0);
}