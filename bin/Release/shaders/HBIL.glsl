#version 440 core

layout (binding = 0) uniform sampler2D gDepth;
layout (binding = 1) uniform sampler2DShadow shadowMap;
layout (binding = 2) uniform samplerCube pointShadow;

layout(location = 0) out vec3 FragColor;

const float PI = 3.14159;
const int NumDirections = 5;
const float NEAR = 0.1;
const float FAR = 3000.0;
in vec2 ViewRay;

in vec2 TexCoords;

uniform vec4 uLightPosition;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 invprojection;
uniform mat4 invView;

uniform vec2 FocalLen;
uniform vec2 UVToViewA;
uniform vec2 UVToViewB;
uniform vec2 Resolution;
uniform vec2 LinMAD;
uniform float iTime;


uniform mat4 depthBias;
uniform mat4 lightSpaceMatrix;

uniform vec3 camPos;
uniform vec3 camDir;


float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return ((2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR)));
}

vec3 PositionFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}


vec3 WorldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = invView * viewSpacePosition;

    return worldSpacePosition.xyz;
}


uvec2 murmurHash23(uvec3 src) {
    const uint M = 0x5bd1e995u;
    uvec2 h = uvec2(1190494759u, 2147483647u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

vec2 hash23(vec3 src) {
    uvec2 h = murmurHash23(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

float shadow(vec4 FragPosLightSpace, vec3 offset) {
    float visibility = 1.0;
    float w = FragPosLightSpace.w;
    vec4 projCoords = FragPosLightSpace / FragPosLightSpace.w;
    FragPosLightSpace = projCoords * 0.5 + 0.5;
    FragPosLightSpace.w = w;

    visibility = texture(shadowMap, vec3(FragPosLightSpace.xy, FragPosLightSpace.z)/FragPosLightSpace.w).r;

    return visibility;
}

float PointShadowCalculation(vec3 fragPos, vec3 LightPos, vec3 viewPos)
{
    float far_plane = 100.;
    vec3 fragToLight = fragPos - LightPos;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
        
    float strength = pow(currentDepth / far_plane, 0.1);

        float closestDepth = texture(pointShadow, fragToLight).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;

    return (1.0 - shadow) * smoothstep(25.0, 0.0, length(fragToLight));//mix(, 0.0, strength);
}

float getDensity(vec3 p) {

    const float density = 0.05;
    const float gradient = 0.05;

    const float bottomLimit = -1.5;
    const float upperLimit = 30.0;
    
    float dist = length(camPos - p);
    float visibility = exp(-pow((dist*density), gradient));

    float heightInfluence = smoothstep(bottomLimit, upperLimit, p.y);

    vec3 sunpos = vec3(13.0, 12.0, 0.0);

    return heightInfluence * visibility + density;//exp( (-p.y - 1.0) / (length(p - sunpos)) * 5.0 );
}

const int MAX_STEPS = 100;
const float max_d = 100000.0;
const float eps = 1.0;

vec3 lightPos = vec3(-31.0, 11.2, 5.8);

vec3 sampleClosestLight(vec3 p) {
    return (1.0 / pow(length(lightPos - p), 2.0) + 0.001) * vec3(1.0);
}

vec3 intersect(in vec3 ro, in vec3 rd, out float dd) {

    float h = 1.0;
    float t = 0.1;
    float stepSize = 1.5;

    vec3 lightAccum = vec3(0.0);

    float depth = 0.0;
    vec3 samplingPos = ro;
    vec3 ambientAccum = vec3(0.0);

    int steps = 0;

    vec2 ShadowTexelSize = 1.0 / textureSize(shadowMap, 0);

    for(int i= 0; i < MAX_STEPS; i++) {
        samplingPos = ro + rd * t;
        
        vec3 FoC = (samplingPos - camPos);
        if(dot(FoC, camDir) < 0.0 || length(FoC) > max_d) break;

        vec4 FragPosLightSpace = lightSpaceMatrix * vec4(samplingPos, 1.0);
        vec3 sh = vec3(0.0);
        sh += vec3(shadow(FragPosLightSpace, ro));
        sh += vec3(PointShadowCalculation(samplingPos, uLightPosition.rgb, camPos) * vec3(1.0, 47.0/255.0, 0.0));
        lightAccum += sh * getDensity(samplingPos); 
        t += stepSize * hash23(FragPosLightSpace.rgb).x;
        stepSize *= 0.99;
        steps++;
    }

    return (lightAccum / steps);
}

float calcAttenuation(vec3 ro) {
    return clamp((length(ro - camPos)) * 0.05, 0.0, 0.5);
}
void main() {
    
    vec3 FragPos = WorldPosFromDepth(textureLod(gDepth, TexCoords, 0).r);
    vec3 viewPos = PositionFromDepth(textureLod(gDepth, TexCoords, 0).x);
    
    //float sh = shadow(FragPosLightSpace);

    vec3 col = vec3(0.0);
    float dd;
    vec3 ro = FragPos, rd = normalize(camPos - FragPos);

    vec3 t = intersect(ro, rd, dd);

    vec3 lightColor = normalize(vec3(60, 50, 50));

    col = ((vec3(t)* lightColor) * calcAttenuation(ro)) + vec3(0.005 * getDensity(FragPos)) ;
    
    FragColor = col;
}
