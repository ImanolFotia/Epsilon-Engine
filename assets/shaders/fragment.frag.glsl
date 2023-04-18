#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "PBRFunctions.glsl"

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
    mat4 lightMatrix;
    int iFrame;
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


#define BLOCKER_SEARCH_NUM_SAMPLES 16
#define PCF_NUM_SAMPLES 32
#define NEAR_PLANE 0.1
float LIGHT_WORLD_SIZE = ubo.lightPosition.x; // 1.0
float LIGHT_FRUSTUM_WIDTH = ubo.lightPosition.y; // 50.0
// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
float LIGHT_SIZE_UV = (LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH);

vec2 poissonDisk[64] = {
    vec2(-0.613392, 0.617481),
    vec2(0.170019, -0.040254),
    vec2(-0.299417, 0.791925),
    vec2(0.645680, 0.493210),
    vec2(-0.651784, 0.717887),
    vec2(0.421003, 0.027070),
    vec2(-0.817194, -0.271096),
    vec2(-0.705374, -0.668203),
    vec2(0.977050, -0.108615),
    vec2(0.063326, 0.142369),
     vec2(0.203528, 0.214331),
     vec2(-0.667531, 0.326090),
     vec2(-0.098422, -0.295755),
     vec2(-0.885922, 0.215369),
     vec2(0.566637, 0.605213),
     vec2(0.039766, -0.396100),
     vec2(0.751946, 0.453352),
     vec2(0.078707, -0.715323),
     vec2(-0.075838, -0.529344),
     vec2(0.724479, -0.580798),
     vec2(0.222999, -0.215125),
     vec2(-0.467574, -0.405438),
     vec2(-0.248268, -0.814753),
     vec2(0.354411, -0.887570),
     vec2(0.175817, 0.382366),
     vec2(0.487472, -0.063082),
     vec2(-0.084078, 0.898312),
     vec2(0.488876, -0.783441),
     vec2(0.470016, 0.217933),
     vec2(-0.696890, -0.549791),
     vec2(-0.149693, 0.605762),
     vec2(0.034211, 0.979980),
     vec2(0.503098, -0.308878),
     vec2(-0.016205, -0.872921),
     vec2(0.385784, -0.393902),
     vec2(-0.146886, -0.859249),
     vec2(0.643361, 0.164098),
     vec2(0.634388, -0.049471),
     vec2(-0.688894, 0.007843),
     vec2(0.464034, -0.188818),
     vec2(-0.440840, 0.137486),
     vec2(0.364483, 0.511704),
     vec2(0.034028, 0.325968),
     vec2(0.099094, -0.308023),
     vec2(0.693960, -0.366253),
     vec2(0.678884, -0.204688),
     vec2(0.001801, 0.780328),
     vec2(0.145177, -0.898984),
     vec2(0.062655, -0.611866),
     vec2(0.315226, -0.604297),
     vec2(-0.780145, 0.486251),
     vec2(-0.371868, 0.882138),
     vec2(0.200476, 0.494430),
     vec2(-0.494552, -0.711051),
     vec2(0.612476, 0.705252),
     vec2(-0.578845, -0.768792),
     vec2(-0.772454, -0.090976),
     vec2(0.504440, 0.372295),
     vec2(0.155736, 0.065157),
     vec2(0.391522, 0.849605),
     vec2(-0.620106, -0.328104),
     vec2(0.789239, -0.419965),
     vec2(-0.545396, 0.538133),
     vec2(-0.178564, -0.596057)
};

float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5433);
}

float rand(vec4 co) {
    float dot_product = dot(co, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dot_product) * 43758.5433);
}

float PenumbraSize(float zReceiver, float zBlocker) // Parallel plane estimation
{
    return (zReceiver - zBlocker) / zBlocker;
}

void FindBlocker(out float avgBlockerDepth,
                 out float numBlockers,
                 vec2 uv, float zReceiver)
{
    // This uses similar triangles to compute what
    // area of the shadow map we should search
    float searchWidth =  LIGHT_SIZE_UV; //*((zReceiver - NEAR_PLANE) / zReceiver);
    float blockerSum = 0;
    numBlockers = 0;

    
    float current = shadowCoord.z;
    float closest = texture(shadowMap, shadowCoord).r;

    for (int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i)
    {
        float shadowMapDepth = texture(depthMap,
            shadowCoord.xy + vec2(((poissonDisk[i]/1000.0)) * searchWidth)).r;

        if (shadowMapDepth < zReceiver)
        {
            blockerSum += shadowMapDepth;
            numBlockers++;
        }
    }

    avgBlockerDepth = blockerSum / numBlockers;
}
float PCF_Filter(vec2 uv, float zReceiver, float filterRadiusUV)
{
    float sum = 0.0;
    float theta = rand(vec4(uv, texCoords.xy));
    mat2 rotation = mat2(vec2(cos(theta), sin(theta)), vec2(-sin(theta), cos(theta)));
    for (int i = 0; i < PCF_NUM_SAMPLES; ++i)
    {
        vec2 offset = (rotation*(poissonDisk[i]/1000.0)) * filterRadiusUV;
        vec2 texOffset = uv * offset;
        bvec2 outside = greaterThan(texOffset, vec2(1.0));
        bvec2 inside = lessThan(texOffset, vec2(0.0));
        float isShadow = texture(shadowMap, shadowCoord + vec3(texOffset, 0.0));
        float z = texture(shadowMap, shadowCoord + vec3(offset, 0.0)).r;
        sum += z;//((any(outside)||any(inside)) ? 1.0 : isShadow);//z;// > shadowCoord.z + 0.01 ? 1.0 : 0.0;
    }
    return sum / PCF_NUM_SAMPLES;
}
float PCSS(vec4 coords)
{
    vec2 uv = coords.xy / coords.w;
    float zReceiver = shadowCoord.z; // Assumed to be eye-space z in this code
    // STEP 1: blocker search
    float avgBlockerDepth = 0;
    float numBlockers = 0;
    FindBlocker(avgBlockerDepth, numBlockers, uv, zReceiver);
    //avgBlockerDepth = avgBlockerDepth;
    if (numBlockers < 1)
        // There are no occluders so early out (this saves filtering)
        return 1.0;
    // STEP 2: penumbra size
    float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
    
    float filterRadiusUV = min(penumbraRatio * ubo.lightPosition.z, 9.0); /** NEAR_PLANE / zReceiver*/;
    // STEP 3: filtering
    return PCF_Filter(uv, zReceiver, filterRadiusUV);
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
    roughness = sqrt(roughness*4.0);
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
    

    sum = PCSS(shadowCoords);
    
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


    outColor.rgb = tonemapACES(outColor.rgb);
    outColor.a = diffuse.a;
    //cross[{{1472, 768, -3584}}-{{1856, 768, -3584}}, {{1728, 768, -3584}}-{{1856, 768, -3584}}]
}
