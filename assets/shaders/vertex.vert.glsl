#version 460 core

#extension GL_EXT_nonuniform_qualifier : enable

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
layout (location = 5) out mat3 TBN;
layout (location = 8) out flat int InstanceIndex;


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
/*
layout(binding = 3) readonly buffer MapMatrix 
{
	mat4 model;
} ssbo;*/

layout (set = 1, binding = 0) uniform sampler2D textures[];

layout( push_constant ) uniform constants
{
	mat4 model;
} PushConstants;
struct Material
{
	int diffuse;
	int normal;
};

layout(std430, binding = 3) buffer MaterialsIn {
   Material materials[];
} ssbo;

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
	0.5, 0.0, 0.0, 0.0,
	0.0, 0.5, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.5, 0.5, 0.0, 1.0 );

	
mat3 CreateTBNMatrix(mat3 normalMatrix)
{
     vec3 T = normalize(normalMatrix * inTangent);
     vec3 B = normalize(normalMatrix * inBitangent);
     vec3 N = normalize(normalMatrix * inNormal);

    return transpose(mat3(T, B, N));
}

void main() {
	if(inColor.a > 0.9)
    		color = vec4(colors[gl_InstanceIndex % 27], inColor.a);
    	else
    		color = inColor;

			mat4 model = mat4(
				0.0125, 0.0, 0.0, 0.0,
				0.0, 0.0125, 0.0, 0.0, 
				0.0, 0.0, 0.0125, 0.0,
				0.0, 0.0, 0.0, 1.0);

				
    InstanceIndex = gl_InstanceIndex;
  	mat3 normalMatrix = mat3(inverse(model));//transpose(inverse(mat3(model)));
  	TBN = CreateTBNMatrix(normalMatrix);

    position = /*(PushConstants.model)*/model * vec4(inPosition, 1.0);
	//shadowCoords = (biasMat * ubo.lightMatrix * /*(PushConstants.model)*/model) * vec4(inPosition, 1.0);
	shadowCoords = (biasMat * ubo.lightMatrix * model) * vec4(inPosition, 1.0);
	//shadowCoords = shadowCoords * 0.5 + 0.5;
    texCoords = inTexCoord;
    normal = inNormal; //normalize(mat3(transpose(inverse(PushConstants.model))) * inNormal);
    //vec3 outVert = inPosition;
    //outVert.y *= (iResolution.x / iResolution.y);

    gl_Position = ubo.proj * ubo.view * /*(PushConstants.model)*/model * vec4(inPosition, 1.0);
}
