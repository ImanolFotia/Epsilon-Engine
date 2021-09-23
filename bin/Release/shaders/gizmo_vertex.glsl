#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;

layout (std140, binding = 1) uniform CameraData_t
{
    vec4 position;
    vec4 direction;
    mat4 projection;
    mat4 view;
    mat4 invprojection;
    mat4 invview;
    int CurrentSelectedEntity; float padding2[3];
    float near; float padding0[3];
    float far; float padding1[3];
} CameraData;

uniform mat4 ModelMatrix;

void main() {
    gl_Position = CameraData.projection * CameraData.view * ModelMatrix * vec4(position, 1.0);
}