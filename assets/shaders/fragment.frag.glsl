#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "PBRFunctions.glsl"
#include "VarianceShadowMapping.glsl"
#include "PCSS.glsl"


layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in flat vec3 normal;
layout(location = 3) in flat vec4 color;
layout(location = 4) in vec4 shadowCoords;
layout(location = 5) in mat3 TBN;
layout(location = 8) in flat int InstanceIndex;

layout(binding = 0) uniform UniformBufferObject
{

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
}
ubo;

struct Material
{
	int diffuse;
	int normal;
};

const float NEAR_PLANE = 0.1;

const float FAR_PLANE = 3000.0;

// layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2DShadow shadowMap;
layout(binding = 2) uniform sampler2D depthMap;

layout(std430, set = 0,  binding = 3) buffer MaterialsIn {
   Material materials[];
} ssbo;

layout (set = 1, binding = 0) uniform sampler2D textures[];

vec3 Gamma(in vec3 img)
{
    return pow(img, vec3(1.0 / 2.2));
}

vec3 Degamma(in vec3 img)
{
    return pow(img, vec3(2.2));
}

const vec3 lightPos = vec3(25.6363, 25.117, -25.7729);
const vec3 lightDir = normalize(vec3(23.6365, 25.117, -45.772));//normalize(lightPos - vec3(26.4674, 1.16102, 22.1613));

vec3 shadowCoord = shadowCoords.xyz / shadowCoords.w;

float LinearizeDepth(float depth, float near, float far)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return ((2.0 * near * far) / (far + near - z * (far - near)));
}


const vec3 dark_color = vec3(119.0/255.0, 149.0/255.0, 86.0/255.0);
const vec3 light_color = vec3(235.0/255.0, 236.0/255.0, 208.0/255.0);
vec3 drawCheckers(vec2 coords, float size) {

    ivec2 tiles = ivec2(coords/size);
    
    bool x = (tiles.x % 8) % 2 == 0;
    bool y = (tiles.y % 8) % 2 == 0;

    return (x ^^ y) ? dark_color : light_color;
}

const vec3 sunColor = normalize(vec3(255, 212, 148));

//Narkowicz 2015, "ACES Filmic Tone Mapping Curve"
vec3 tonemapACES(vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return (x * (a * x + b)) / (x * (c*x+d) + e);
}


float IGN(int pixelX, int pixelY, int frame)
{
    //return mod(52.9829189 * mod(0.06711056*float(pixelX) + 0.00583715*float(pixelY), 1.0), 1.0);
    frame = frame % 64; // need to periodically reset frame to avoid numerical issues
    float x = float(pixelX) + 5.588238 * float(frame);
    float y = float(pixelY) + 5.588238 * float(frame);
    return mod(52.9829189 * mod(0.06711056*float(x) + 0.00583715*float(y), 1.0), 1.0);
}

float ResolveTransparency() {

    ivec2 invFragCoord = ivec2(gl_FragCoord);
    float lDepth = LinearizeDepth(gl_FragCoord.z, NEAR_PLANE, FAR_PLANE);

    int mult = 40;
    int cycle = 3;

    int frame = ubo.iFrame % 64;

    int indexX = invFragCoord.x+int(lDepth);
    int indexY = invFragCoord.y+int(lDepth);

    return IGN(indexX, indexY, ubo.iFrame);
}


void main()
{
    if(color.a < 0.5)
    {
        outColor.a = 0.3;
    }

    int diffuse_index = ssbo.materials[InstanceIndex].diffuse;
    int normal_index = ssbo.materials[InstanceIndex].normal;
    

    const vec2 texDivisor = textureSize(textures[diffuse_index],0);

    
    vec4 diffuse = texture(textures[diffuse_index], texCoords/texDivisor).rgba;

    diffuse.rgb = Gamma(diffuse.rgb);

    const float fAlphaMultiplier = 1.5;
    const float fAlphaTest = 0.25;
    float fNewAlpha = diffuse.a * fAlphaMultiplier;
    /*
    if(fNewAlpha < fAlphaTest)
      discard;*/


    //float fragDist = length(ubo.viewPosition - position.xyz) - 1.0;

    //if(ResolveTransparency() < 1.0 - fragDist)
    //  discard;  
        
    if(fNewAlpha > 1.0)
      fNewAlpha = 1.0; 

    //texture(textures[1], texCoords/texDivisor).r;
    vec3 normal_tex = vec3(0.5, 0.5, 1.0);
    float roughness = diffuse.r*0.35+ diffuse.g*0.71+ diffuse.r*0.12;
    roughness = sqrt(roughness*2.0);
    if(normal_index != -1) {
        normal_tex = texture(textures[normal_index], abs(texCoords/texDivisor)).rgb;
        roughness = roughness * 0.5;//texture(textures[3], texCoords/texDivisor).r;
    }
    float metallic = 0.0;

    vec3 mNormal = normalize(normalize(normal_tex* 2.0-1.0)  * TBN);
    float SdotN = dot(vec3(0.0, 1.0, 0.0), mNormal);
    
        vec3 backcol = mix(vec3( 0.92, 0.95, 0.99 ), vec3( 0.45, 0.68, 0.88 ), clamp(vec3(SdotN) +0.5, 0.0, 1.));
    vec3 skyLight = vec3(max(SdotN, 0.0)) + backcol;

    //skyLight = pow(skyLight * 0.5 + 0.5,vec3(2.0)) * normalize(vec3(111, 159, 199));
    
    vec4 FragPosLightSpace = ubo.lightMatrix * vec4(position.xyz, 1.0);

    float current = shadowCoord.z;
    float closest = texture(shadowMap, vec3(shadowCoord.xy, 0.501)).r;
    const float bias = 0.001;
    
    //float mult = clamp(abs(closest - current) * 10, 0.0, 5.0);
    float sum = 0;
    for(int i = 0; i < 16; i++) {
        float z = texture(shadowMap, shadowCoord + vec3((poissonDisk[i % 16] * 0.0015), 0.0)).r;
        sum += z;
    }

    sum /= 16.0;

    float shadow = 0.5;

    if (current > closest + 0.001)
    {
        shadow = 0.1;
    }

    //sum = PCF_Filter(shadowCoords.xy / shadowCoords.w, shadowCoord.z, 10.0);
    

    sum = PCSS(depthMap, shadowMap, shadowCoords, shadowCoord, texCoords);
    
    //sum = VSM(depthMap, shadowCoord);
    
    vec3 V = normalize(ubo.viewPosition - position.xyz);

    //fragDist = smoothstep(0.0, 2.0, fragDist);

    vec3 reflv = reflect( V, mNormal);
    vec3 reflc = mix(vec3( 0.45, 0.68, 0.88 ), vec3( 0.92, 0.95, 0.99 ), reflv.y);
    //sum = texture(shadowMap, shadowCoord);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, diffuse.rgb, vec3(0.0));
    //sum = textureGrad(shadowMap, shadowCoord, vec2(0.0),  vec2(0.0)).r;
    vec3 light = sunColor*CalculateDirectionalPBR(lightDir, vec3(1.0), 1.0, ubo.viewPosition, position.xyz, F0, mNormal, roughness, diffuse.rgb);


    outColor.rgb = pow(sum, 3.0) *  light * 5.0 + (skyLight*diffuse.rgb*0.4) + reflc*0.1; //(Gamma(outColor.rgb));


    outColor.rgb = Degamma(outColor.rgb);

    outColor.rgb = tonemapACES(outColor.rgb);
    outColor.a = diffuse.a;
    //cross[{{1472, 768, -3584}}-{{1856, 768, -3584}}, {{1728, 768, -3584}}-{{1856, 768, -3584}}]
}
