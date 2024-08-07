#version 450 core

layout (location = 0) out vec4 fragColor;

layout(location = 0) in VARYING  {
    vec4 color;
    vec2 texCoords;
} fs_in;

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

layout(binding = 1) uniform sampler2D tex;

void main() {
    fragColor = fs_in.color * texture(tex, fs_in.texCoords);
}