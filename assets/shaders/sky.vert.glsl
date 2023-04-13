#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable

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
layout (location = 4) out mat3 invView;

layout(binding = 0) uniform UniformBufferObject {

    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
	vec3 viewPosition;
    mat4 view;
    mat4 proj;
    mat4 lightMatrix;
} ubo;

layout (set = 1, binding = 0) uniform sampler2D textures[];

layout( push_constant ) uniform constants
{
	mat4 model;
} PushConstants;

void main() {
    color = inColor;
    position = vec3(-inPosition.x, inPosition.z, inPosition.y);
    texCoords = inTexCoord;
    
			mat4 model = mat4(
				0.0125, 0.0, 0.0, 0.0,
				0.0, 0.0125, 0.0, 0.0, 
				0.0, 0.0, 0.0125, 0.0,
				0.0, 0.0, 0.0, 1.0);
                invView = inverse(mat3(ubo.view));
    normal = normalize(mat3(transpose(inverse(/*PushConstants.model*/model))) * inNormal);

    vec3 outVert = position;
    //outVert.y *= (iResolution.x / iResolution.y);

    gl_Position = ubo.proj * ubo.view * /*PushConstants.model*/model * vec4(outVert, 1.0);
}