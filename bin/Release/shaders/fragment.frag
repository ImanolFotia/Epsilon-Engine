#version 430 core

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_specular;
layout (binding = 2) uniform sampler2D texture_normal;
layout (binding = 3) uniform sampler2D texture_height;
layout (binding = 4) uniform samplerCube skybox;
layout (binding = 5) uniform sampler2DShadow shadowMap;
layout (binding = 6) uniform samplerCube gPointShadowMap;

uniform bool using_color_diffuse = false;
uniform bool using_color_specular = false;
uniform bool using_color_normal = false;
uniform bool using_color_height = false;

uniform vec4 color_diffuse;
uniform vec4 color_specular;
uniform vec4 color_normal;
uniform vec4 color_height;

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

const vec2 BlueNoiseInDisk[64] = vec2[64](
    vec2(0.478712,0.875764),
    vec2(-0.337956,-0.793959),
    vec2(-0.955259,-0.028164),
    vec2(0.864527,0.325689),
    vec2(0.209342,-0.395657),
    vec2(-0.106779,0.672585),
    vec2(0.156213,0.235113),
    vec2(-0.413644,-0.082856),
    vec2(-0.415667,0.323909),
    vec2(0.141896,-0.939980),
    vec2(0.954932,-0.182516),
    vec2(-0.766184,0.410799),
    vec2(-0.434912,-0.458845),
    vec2(0.415242,-0.078724),
    vec2(0.728335,-0.491777),
    vec2(-0.058086,-0.066401),
    vec2(0.202990,0.686837),
    vec2(-0.808362,-0.556402),
    vec2(0.507386,-0.640839),
    vec2(-0.723494,-0.229240),
    vec2(0.489740,0.317826),
    vec2(-0.622663,0.765301),
    vec2(-0.010640,0.929347),
    vec2(0.663146,0.647618),
    vec2(-0.096674,-0.413835),
    vec2(0.525945,-0.321063),
    vec2(-0.122533,0.366019),
    vec2(0.195235,-0.687983),
    vec2(-0.563203,0.098748),
    vec2(0.418563,0.561335),
    vec2(-0.378595,0.800367),
    vec2(0.826922,0.001024),
    vec2(-0.085372,-0.766651),
    vec2(-0.921920,0.183673),
    vec2(-0.590008,-0.721799),
    vec2(0.167751,-0.164393),
    vec2(0.032961,-0.562530),
    vec2(0.632900,-0.107059),
    vec2(-0.464080,0.569669),
    vec2(-0.173676,-0.958758),
    vec2(-0.242648,-0.234303),
    vec2(-0.275362,0.157163),
    vec2(0.382295,-0.795131),
    vec2(0.562955,0.115562),
    vec2(0.190586,0.470121),
    vec2(0.770764,-0.297576),
    vec2(0.237281,0.931050),
    vec2(-0.666642,-0.455871),
    vec2(-0.905649,-0.298379),
    vec2(0.339520,0.157829),
    vec2(0.701438,-0.704100),
    vec2(-0.062758,0.160346),
    vec2(-0.220674,0.957141),
    vec2(0.642692,0.432706),
    vec2(-0.773390,-0.015272),
    vec2(-0.671467,0.246880),
    vec2(0.158051,0.062859),
    vec2(0.806009,0.527232),
    vec2(-0.057620,-0.247071),
    vec2(0.333436,-0.516710),
    vec2(-0.550658,-0.315773),
    vec2(-0.652078,0.589846),
    vec2(0.008818,0.530556),
    vec2(-0.210004,0.519896) 
);


vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float PointShadowCalculation(vec3 fragPos, vec3 LightPos, inout bool in_range)
{
    float far_plane = 100.;
    vec3 fragToLight = fragPos - LightPos;
    float currentDepth = length(fragToLight);
    float cshadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    in_range = true;
    if(currentDepth >= far_plane) {
        in_range = false;
        return 1.0;
    }
        
    float strength = pow(currentDepth / far_plane, 20.0);

    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(gPointShadowMap, fragToLight + gridSamplingDisk[i] * BlueNoiseInDisk[i].xyx * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            cshadow += 1.0;
    }
    cshadow /= float(samples);
    return mix((1.0 - cshadow), 1.0, strength);
}


void main()
{
	vec3 LightPosition = vec3(37, 6.5, 3.5);

    
  vec2 MetallicHeight = vec2(0.0);
  vec3 normal_tex = vec3(0.0);
  vec3 r_tex = vec3(0.0);
  
  if(!using_color_diffuse) Diffuse = texture(texture_diffuse,TexCoords).rgb;
  else Diffuse = color_diffuse.rgb;
  
  if(!using_color_specular) r_tex = texture(texture_specular,TexCoords).xyz;
  else r_tex = color_specular.xyz;
  
  if(!using_color_normal) normal_tex = texture(texture_normal,TexCoords).xyz;
  else normal_tex = color_normal.xyz;
  
  if(!using_color_height) MetallicHeight = texture(texture_height,TexCoords).xy;
  else MetallicHeight = color_height.xy;

	ExtraComponents.xy = MetallicHeight;
	//Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
	float alpha = texture(texture_diffuse, TexCoords).a;

	Specular = 1.0;//texture(texture_specular, TexCoords).r;
    	vec3 SpecDiff = Diffuse.rgb * Specular;
    
    	F0 = vec3(0.04);
    	F0      = mix(F0, Diffuse.rgb, ExtraComponents.x);


	//if(alpha < 0.5)
    //		discard; 

	if(ExtraComponents.y > 0.1)
	{
		Color = vec4(Diffuse, 1.0);
		return;
	}

	vec3 NormalTexture = normalize(normal_tex.rgb * 2.0 - 1.0);

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

    light = CalculateDirectionalPBR() * 3.0;
    bool in_range = false;
    for(int i = 0 ; i < outBuffer.Lights.length() ; i++)
    {
        if(outBuffer.Lights[i].type == 0)
            light += calculatePointPBR(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].color.rgb, outBuffer.Lights[i].watts);
        else if(outBuffer.Lights[i].type == 1)
            light += SpotLightPBR(outBuffer.Lights[i].position.rgb, outBuffer.Lights[i].direction.rgb, outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb);
        else if(outBuffer.Lights[i].type == 2){
            
            float pointShadow = PointShadowCalculation(FragPos, outBuffer.Lights[i].position.rgb, in_range);
            vec3 res = SphereAreaLight(outBuffer.Lights[i].position.rgb,  outBuffer.Lights[i].radius, outBuffer.Lights[i].color.rgb, outBuffer.Lights[i].watts);
            if(i == 0 && in_range) {
                res *= pointShadow;
            }

            light += res;
        }
    }

    //light += TubeAreaLight(vec3(0, 0, 0), vec3(-89,15,-23.5), vec3(70,15,-23.5),  0.1, normalize(vec3(1,1,1)), 250);
    //light += TubeAreaLight(vec3(0, 0, 0), vec3(-89,15,-15), vec3(70,15,-15),  0.1, normalize(vec3(1,1,1)), 250);
    //light += TubeAreaLight(vec3(0, 0, 0), vec3(-38,11,6), vec3(63,11,6),  0.25, normalize(vec3(200,147,3)), 500);
    //light += TubeAreaLight(vec3(0, 0, 0), vec3(-38,11,-12), vec3(63,11,-12),  0.25, normalize(vec3(200,147,3)), 500);

	Color = (vec4(max(vec3(light), vec3(0.0)), 1.0));
}