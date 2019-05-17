#version 440 core

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_specular;
layout (binding = 2) uniform sampler2D texture_normal;
layout (binding = 3) uniform sampler2D texture_height;
layout (binding = 4) uniform samplerCube skybox;
layout (binding = 5) uniform sampler2DShadow shadowMap;

out vec4 Color;

uniform vec3 lightDir;

in vec2 TexCoords;
vec3 Normal = vec3(0.0);
//in vec3 Normal;
in vec3 FragPos;
in mat3 TBN;

uniform mat4 lightSpaceMatrix;
const float shadowDistance = 100.0f;
const float fadeDistance = 10.0f;
const int NR_LIGHTS = 32;
//uniform vec3 LightPositions[NR_LIGHTS];
struct t_light {

        vec4 position; // 4/*
        vec4 direction; // 8
        vec4 color; // 12
        float radius; float padding0[3]; // 16
        float watts;  float padding1[3]; // 20
        int type;     float padding2[3];//24
    };

layout(std430, binding = 0) buffer ssbo
 {
    t_light Lights[];

 } outBuffer;

vec3 F0;
vec3 viewPos = vec3(0,8,0);
vec4 ExtraComponents;
float Specular;
vec3 Diffuse;
float shadow = 0.0;

#include <PBRFunctions.glsl>

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

#define SEED 1.123456789
#define HASHM mat3(40.15384,31.973157,31.179219,10.72341,13.123009,41.441023,-311.61923,10.41234,178.127121)

float hash(vec3 p, int index) {
    p = fract((vec3(p.x, p.y, p.z) + SEED+index * 1e-3) * HASHM);
    p += dot(p, p.yzx + 41.19);
    return fract((p.x + p.y) * p.z);
}

float poissonShadowMapping(vec4 fragPosLightSpace){

    float visibility = 1.0;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    fragPosLightSpace = fragPosLightSpace * 0.5 + 0.5;
    
    vec3 normal = normalize(Normal);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);

    for (int i=0;i<4;i++){
        int index = int(16.0*random(floor(FragPos.xyz*1000.0), i))%16;
        visibility -= 0.2*(texture(shadowMap, vec3(fragPosLightSpace.xy+ poissonDisk[index]/700.0, fragPosLightSpace.z - bias)/fragPosLightSpace.w).r);
    }
    //visibility -= step(fragPosLightSpace.z, texture(shadowMap, vec3(fragPosLightSpace.xy, fragPosLightSpace.z-bias)).r);
    return visibility;
}


void main()
{


	vec3 LightPosition = vec3(37, 6.5, 3.5);

	ExtraComponents.xy = texture(texture_height, TexCoords).xy;
	Diffuse = texture(texture_diffuse, TexCoords).rgb;
	float alpha = texture(texture_diffuse, TexCoords).a;

	Specular = texture(texture_specular, TexCoords).r;
    	vec3 SpecDiff = Diffuse * Specular;
    
    	F0 = vec3(0.04);
    	F0      = mix(F0, Diffuse, ExtraComponents.x);


	if(alpha < 0.5)
    		discard; 

	if(ExtraComponents.y > 0.1)
	{
		Color = vec4(Diffuse, 1.0);
		return;
	}

	vec3 NormalTexture = normalize(texture(texture_normal, TexCoords).rgb * 2.0 - 1.0);

	vec3 mNormal = vec3(0.0);

    //mNormal = normalize(NormalTexture * TBN);


    if(gl_FrontFacing)
        mNormal = normalize(NormalTexture * TBN);
    else
        mNormal = -normalize(NormalTexture * TBN);


    Normal = mNormal;

    vec4 FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0f);
    shadow = poissonShadowMapping(FragPosLightSpace);

	vec3 light = vec3(0.0);

    light = CalculateDirectionalPBR();

    for(int i = 0 ; i < outBuffer.Lights.length() ; i++)
    {
        if(outBuffer.Lights[i].type == 0)
            light += calculatePointPBR(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].color.rgb);
        else if(outBuffer.Lights[i].type == 1)
            light += SpotLightPBR(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].direction.rgb, outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb);
        else if(outBuffer.Lights[i].type == 2)
            light += SphereAreaLight(outBuffer.Lights[i].position.rgb,  outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb, outBuffer.Lights[i].watts);
    }

    light += TubeAreaLight(vec3(0, 0, 0), vec3(-89,15,-23.5), vec3(70,15,-23.5),  0.1, normalize(vec3(1,1,1)), 250);
    light += TubeAreaLight(vec3(0, 0, 0), vec3(-89,15,-15), vec3(70,15,-15),  0.1, normalize(vec3(1,1,1)), 250);
    //light += TubeAreaLight(vec3(0, 0, 0), vec3(-38,11,6), vec3(63,11,6),  0.25, normalize(vec3(200,147,3)), 500);
    //light += TubeAreaLight(vec3(0, 0, 0), vec3(-38,11,-12), vec3(63,11,-12),  0.25, normalize(vec3(200,147,3)), 500);

	Color = (vec4(max(vec3(light), vec3(0.001)), 1.0));
}



