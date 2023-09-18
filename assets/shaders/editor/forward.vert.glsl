#version 460 core

#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout( push_constant ) uniform constants
{
	mat4 transform;
} push_constants;


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


layout(location = 0) out VARYING {
    flat uint material_index;
    flat int object_id;
    vec4 color;
    vec2 texCoords;
    vec3 normal;
    vec3 position;
    mat3 TBN;
} vs_out;

void main() {
    gl_Position = ubo.proj * ubo.view * push_constants.transform * vec4(inPosition, 1.0);

    vs_out.normal = normalize(mat3(push_constants.transform) * inNormal);
    vs_out.position = vec3(push_constants.transform[3][0], push_constants.transform[3][1], push_constants.transform[3][2]).xyz;
}