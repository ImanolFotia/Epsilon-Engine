#version 440 core

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

out VS_OUT{
out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out noperspective mat3 TBN;
out mat3 TBN2;
out vec3 wFragPos;
} vs_out;

mat3 CreateTBNMatrix(mat3 normalMatrix)
{
     T = normalize(normalMatrix * tangent);
     B = normalize(normalMatrix * bitangent);
     N = normalize(normalMatrix * normal);

    return transpose(mat3(T, B, N));
}

uniform mat4 model;

out vec3 vNormal;

void main() {
    gl_Position = CameraData.projection * CameraData.view * model * vec4(position, 1.0);
    vNormal = normal;
}