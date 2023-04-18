#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out float outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;



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
}
ubo;
layout (set = 1, binding = 0) uniform sampler2D textures[];

float NEAR = 0.1;
float FAR  = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return ((2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR)));
}

void main() {
    if(color.a < 1.0) discard;
    outColor = gl_FragCoord.z;
}