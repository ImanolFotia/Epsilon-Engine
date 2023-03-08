#version 450

const vec2 iResolution = vec2(1280.0, 720.0);

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout (location = 0) out vec4 position;
layout (location = 1) out vec2 texCoords;
layout (location = 2) out flat vec3 normal;
layout (location = 3) out flat vec4 color;
layout (location = 4) out vec4 shadowCoords;

layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
    mat4 view;
    mat4 proj;
    mat4 lightMatrix;
} ubo;
/*
layout(binding = 3) readonly buffer MapMatrix 
{
	mat4 model;
} ssbo;*/

layout( push_constant ) uniform constants
{
	mat4 model;
} PushConstants;

vec3 colors[27] = {
	vec3(1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),
	vec3(1.0, 0.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 1.0, 0.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.0, 1.0),
	vec3(0.0, 0.0, 1.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(1.0, 1.0, 0.0),
	vec3(0.0, 1.0, 1.0),
	vec3(0.0, 1.0, 1.0),
	vec3(0.0, 1.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 0.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(0.6, 0.4, 0.2),
	vec3(0.6, 0.4, 0.2),
	vec3(0.6, 0.4, 0.2),
	vec3(0.2, 0.4, 0.5),
	vec3(0.2, 0.4, 0.5),
	vec3(0.2, 0.4, 0.5)
};

const float kEpsilon = 0.498;

const mat4 biasMat = mat4( 
	kEpsilon, 0.0, 0.0, 0.0,
	0.0, kEpsilon, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	kEpsilon, kEpsilon, 0.0, 1.0 );

void main() {
	if(inColor.a > 0.9)
    		color = vec4(colors[gl_VertexIndex % 27], inColor.a);
    	else
    		color = inColor;

			mat4 model = mat4(
				0.0125, 0.0, 0.0, 0.0,
				0.0, 0.0125, 0.0, 0.0, 
				0.0, 0.0, 0.0125, 0.0,
				0.0, 0.0, 0.0, 1.0);
    		
    position = /*(PushConstants.model)*/model * vec4(inPosition, 1.0);
	shadowCoords = (biasMat * ubo.lightMatrix * /*(PushConstants.model)*/model) * vec4(inPosition, 1.0);
    texCoords = inTexCoord;
    normal = inNormal; //normalize(mat3(transpose(inverse(PushConstants.model))) * inNormal);
    //vec3 outVert = inPosition;
    //outVert.y *= (iResolution.x / iResolution.y);

    gl_Position = ubo.proj * ubo.view * /*(PushConstants.model)*/model * vec4(inPosition, 1.0);
}
