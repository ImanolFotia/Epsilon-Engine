#version 450

layout(location = 0) out float outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;


layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

void main() {
    outColor = gl_FragCoord.z;
}