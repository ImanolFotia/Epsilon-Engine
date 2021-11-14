#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 tangent;
layout(location = 2) in vec3 normal;

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

out VS_OUT {
    vec3 color;
    vec3 normal;
    vec3 tangent;
}vs_out;

void main() {
    vs_out.color = normalize(vec3(0.00001, 0.00001, 0.00001));
    mat3 normalMatrix = mat3(transpose(inverse(CameraData.view)));
    vs_out.tangent = normalize(vec3(vec4(normalMatrix * tangent, 0.0)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * normal, 0.0)));
    gl_Position = CameraData.view * vec4(position, 1.0);
}