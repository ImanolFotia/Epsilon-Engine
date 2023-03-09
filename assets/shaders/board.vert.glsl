#version 460
#extension GL_EXT_nonuniform_qualifier : enable
#extension VK_EXT_descriptor_indexing : enable

const vec2 iResolution = vec2(1280.0, 720.0);

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout (location = 0) out vec3 position;
layout (location = 1) out vec2 texCoords;
layout (location = 2) out vec3 normal;
layout (location = 3) out vec4 color;

layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
} ubo;

layout( push_constant ) uniform constants
{
	mat4 model;
} PushConstants;

void main() {
    color = inColor;
    position = inPosition;
    texCoords = inTexCoord;
    normal = inNormal; //normalize(mat3(transpose(inverse(PushConstants.model))) * inNormal);
    //vec3 outVert = inPosition;
    //outVert.y *= (iResolution.x / iResolution.y);

    gl_Position = ubo.proj * ubo.view * PushConstants.model  * vec4(inPosition, 1.0);
}