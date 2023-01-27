#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in flat vec3 normal;
layout(location = 3) in flat vec4 color;
layout(location = 4) in vec4 shadowCoords;

layout(binding = 0) uniform UniformBufferObject
{
    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
    mat4 view;
    mat4 proj;
    mat4 lightMatrix;
}
ubo;

// layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2DShadow shadowMap;

vec3 Gamma(in vec3 img)
{
    return pow(img, vec3(1.0 / 2.2));
}

vec3 Degamma(in vec3 img)
{
    return pow(img, vec3(2.2));
}

const vec3 lightPos = vec3(25.6363, 25.117, -25.7729);
const vec3 lightDir = normalize(lightPos - vec3(26.4674, 1.16102, 22.1613));

vec3 shadowCoord = shadowCoords.xyz / shadowCoords.w;
#define FAR 100
#define BLOCKER_SEARCH_NUM_SAMPLES 16
#define PCF_NUM_SAMPLES 16
#define NEAR_PLANE 0.1
#define LIGHT_WORLD_SIZE 50
#define LIGHT_FRUSTUM_WIDTH 50
// Assuming that LIGHT_FRUSTUM_WIDTH == LIGHT_FRUSTUM_HEIGHT
#define LIGHT_SIZE_UV 0.0001//(LIGHT_WORLD_SIZE / LIGHT_FRUSTUM_WIDTH)

    vec2 poissonDisk[16] = {
        vec2(-0.94201624, -0.39906216),
        vec2(0.94558609, -0.76890725),
        vec2(-0.094184101, -0.92938870),
        vec2(0.34495938, 0.29387760),
        vec2(-0.91588581, 0.45771432),
        vec2(-0.81544232, -0.87912464),
        vec2(-0.38277543, 0.27676845),
        vec2(0.97484398, 0.75648379),
        vec2(0.44323325, -0.97511554),
        vec2(0.53742981, -0.47373420),
        vec2(-0.26496911, -0.41893023),
        vec2(0.79197514, 0.19090188),
        vec2(-0.24188840, 0.99706507),
        vec2(-0.81409955, 0.91437590),
        vec2(0.19984126, 0.78641367),
        vec2(0.14383161, -0.14100790)};


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
    float searchWidth = 0.003;//LIGHT_SIZE_UV * (shadowCoord.z - NEAR_PLANE) / shadowCoord.z;
    float blockerSum = 0;
    numBlockers = 0;
    for (int i = 0; i < BLOCKER_SEARCH_NUM_SAMPLES; ++i)
    {
        float shadowMapDepth = textureGrad(shadowMap,
            shadowCoord + vec3(poissonDisk[i] * searchWidth, 0.0), vec2(0.0),  vec2(0.0)).r;
        if (shadowMapDepth < shadowCoord.z)
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
    for (int i = 0; i < PCF_NUM_SAMPLES; ++i)
    {
        vec2 offset = poissonDisk[i] * filterRadiusUV;
        float z = texture(shadowMap, shadowCoord + vec3(offset, 0.0), 0).r;
        sum += z;// > shadowCoord.z + 0.01 ? 1.0 : 0.0;
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
    if (numBlockers < 1)
        // There are no occluders so early out (this saves filtering)
        return 1.0;
    // STEP 2: penumbra size
    float penumbraRatio = PenumbraSize(zReceiver, avgBlockerDepth);
    float filterRadiusUV = penumbraRatio * LIGHT_SIZE_UV * NEAR_PLANE / zReceiver;
    // STEP 3: filtering
    return PCF_Filter(uv, zReceiver, filterRadiusUV );
}

void main()
{
    float NoL = max(dot(lightDir, normal), 0.0);
    
    vec4 FragPosLightSpace = ubo.lightMatrix * vec4(position.xyz, 1.0);

    float current = shadowCoord.z;
    float closest = texture(shadowMap, shadowCoord).r;
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

    sum = PCF_Filter(shadowCoords.xy / shadowCoords.w, shadowCoord.z, 0.001);
    

    //sum = PCSS(shadowCoords);

    //sum = textureGrad(shadowMap, shadowCoord, vec2(0.0),  vec2(0.0)).r;

    outColor.a = color.a; // texCol.a;
    // float shadow = poissonShadowMapping(FragPosLightSpace);

    // float shadow = texture(shadowMap, position.xyz).r;//textureProj(position / position.w, vec2(0.0));//clamp(1.0 - shadow, 0.0, 1.0);

    outColor.rgb = vec3((NoL * sum) + 0.1) * color.rgb; //(Gamma(outColor.rgb));
}
