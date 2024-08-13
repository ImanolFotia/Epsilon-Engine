#version 460 core

layout(location = 0) out vec4 fragColor;


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

void main() {
    fragColor = vec4(1.0);
}