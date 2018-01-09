#version 440 core

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_specular;
layout (binding = 2) uniform sampler2D texture_normal;
layout (binding = 3) uniform sampler2D texture_height;
layout (binding = 4) uniform samplerCube skybox;

out vec4 Color;

uniform vec3 lightDir;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in mat3 TBN;

const int NR_LIGHTS = 32;
//uniform vec3 LightPositions[NR_LIGHTS];
struct t_light {

        vec4 position; // 4/*
        vec4 direction; // 8
        vec4 color; // 12
        float radius; float padding0[3]; // 13
        float watts;  float padding1[3]; // 14
        int type;     float padding2[3];//15*/
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


	if(alpha < 0.2)
    	discard; 

	if(ExtraComponents.y > 0.1)
	{
		Color = vec4(Diffuse*0.5, 1.0);
		return;
	}

	vec3 NormalTexture = texture(texture_normal, TexCoords).rgb * 2.0 - 1.0;

	vec3 mNormal;

  if(gl_FrontFacing)
    mNormal = normalize(NormalTexture * TBN);
  else
    mNormal = -normalize(NormalTexture * TBN);

	vec3 light = vec3(0.0);

    for(int i = 0 ; i < outBuffer.Lights.length() ; i++)
    {
        if(outBuffer.Lights[i].type == 0)
            light += calculatePointPBR(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].color.rgb);
        else if(outBuffer.Lights[i].type == 1)
            light += SpotLightPBR(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].direction.rgb, outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb);
        else if(outBuffer.Lights[i].type == 2)
            light += SphereAreaLight(outBuffer.Lights[i].position.rgb,  outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb, outBuffer.Lights[i].watts);
    }

	Color = (vec4(light, 1.0));
}



