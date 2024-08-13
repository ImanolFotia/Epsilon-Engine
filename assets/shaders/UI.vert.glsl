#version 450 core


layout(location = 0) in vec4 pos_coords;
layout(location = 1) in vec4 color;

layout(location = 0) out VARYING  {
    vec4 color;
    vec2 texCoords;
} vs_out;


struct RenderPass {
    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
	vec3 viewPosition;
    mat4 view;
    mat4 proj;
    mat4 prev_view;
    mat4 prev_proj;
    mat4 lightMatrix;
    int iFrame;
    ivec2 iMouse;
};

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    RenderPass data;
}
RenderPassUBO;

void main() {
    gl_Position = vec4(pos_coords.xy * 2.0 - 1.0, 0.0, 1.0);
    vs_out.color = color;
    vs_out.texCoords = pos_coords.zw;
}