#version 440 core

#define Scale vec3(.8, .8, .8)
#define K 19.19

uniform sampler2D gFinalImage;
uniform sampler2D gNormal;
uniform sampler2D gExtraComponents;
uniform sampler2D gDepth; 
uniform sampler2D BRDF;
uniform sampler2D noiseTexture;
uniform sampler2D PreviousReflection;
uniform sampler2D gAlbedo;

uniform mat4 invView;
uniform mat4 projection;
uniform mat4 invprojection;
uniform mat4 view;
uniform vec3 camDir;
uniform vec3 camPos;
uniform vec2 Resolution;
uniform vec2 LinMAD;
uniform float Time;
uniform int ID;
uniform bool isMoving;

uniform int maxSteps = 100;
uniform int numBinarySearchSteps = 10;
uniform int SSROn;
uniform float roughnessCutoff = 0.8;
uniform float depthCutoff = 0.0;

uniform float minRayStep = 0.01;
uniform float maxRayStep = 0.05;

noperspective in vec2 TexCoords;

out vec4 outColor;

const float rayIncrement = 1.04;
const float searchDist = 5;
const float searchDistInv = 0.2;
const float maxDDepth = 1.0;
const float maxDDepthInv = 1.0;
const float reflectionSpecularFalloffExponent = 3.0;

const float MAX_REFLECTION_LOD = 4.0;

float Roughness = 0.0;
float rayStep = 0.05;
float Metallic = 0.0;
float near = 0.1;
float far = 3000.0;

vec3 PositionFromDepth(float depth);

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth);
 
vec4 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth);

vec3 fresnelSchlick(float cosTheta, vec3 F0);

vec3 hash(vec3 a);

vec3 hash31(float a)
{
   vec3 p3 = fract(vec3(a) * Scale);
   p3 += dot(p3, p3.yzx+K);
   return clamp(fract((p3.xxy+p3.yzz)*p3.zyx), -1.0, 1.0); 
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
} 

float ViewSpaceZFromDepth(float d)
{
    // [0,1] -> [-1,1] clip space
    d = d * 2.0 - 1.0;

    // Get view space Z
    return -1.0 / (LinMAD.x * d + LinMAD.y);
}

vec3 random_in_unit_sphere(float x) {
    const int max_tries = 5;
    int tries = 0;
    vec3 p = vec3(0,0,0);
    while (tries < max_tries) {
        p = hash31(x);
        float l = p.length();
        tries++;
        if (l*l >= 1) continue;
        return p;
    }
    return p;
}
uvec3 murmurHash33(uvec3 src) {
    const uint M = 0x5bd1e995u;
    uvec3 h = uvec3(1190494759u, 2147483647u, 3559788179u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

// 3 outputs, 3 inputs
vec3 hash33(vec3 src) {
    uvec3 h = murmurHash33(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}

void main()
{

    vec4 PrevReflection = texture(PreviousReflection, TexCoords);

    if(ID > 251){
    	outColor = PrevReflection;
    	return;
    }

    if(ID == 251){
    	outColor = PrevReflection / 251;
    	return;
    }

    rayStep = minRayStep;

    vec2 MetallicEmmissive = texture(gExtraComponents, TexCoords).rg;
    vec2 texelSize = 1.0 / vec2(textureSize(gExtraComponents, 0));
    Metallic = MetallicEmmissive.r;

    bool isPlastic = false;
    vec4 AlbedoSpec = texture(gAlbedo, TexCoords);
    vec4 NormalSpecular = texture(gNormal, TexCoords);
    vec3 worldNormal = normalize(NormalSpecular.rgb);
    float Spec = NormalSpecular.w;//clamp(NormalSpecular.w - AlbedoSpec.w, 0.0, 1.0);

    if(Metallic <= 0.1 || Spec > roughnessCutoff){
        isPlastic = true;
    }
 
    vec3 viewNormal = vec3(vec4(worldNormal, 1.0) * invView);
    vec3 viewPos = PositionFromDepth( texture(gDepth, TexCoords).x);
    vec3 worldPos = vec3(invView * vec4(viewPos, 1.0));

    if(texture(gDepth, TexCoords).x > 0.9999999)
    {
        outColor = vec4(0.0);
        return;
    }

    vec3 viewDir = normalize(camPos - worldPos);

    Roughness = texture(gNormal, TexCoords).w;
    
    vec3 hitPos = viewPos;
 
    float iTime = Time;
    vec3 SSR;
    vec2 brdf  = texture(BRDF, vec2(max(dot(worldNormal, viewDir), 0.0), Roughness)).rg;
    iTime = iTime;//!isMoving ? iTime : 1.0;
    if(SSROn == 1 && !isPlastic ){
        vec2 NoiseScale = Resolution / 4.0;
        vec3 random = hash33(worldPos + iTime);//vec3(texture(noiseTexture, (TexCoords.xy*10.0) + (1.0 - iTime)).rgb);
        random = dot(random, viewNormal) > 0.0 ? random : -random;
        float factor = Roughness*0.20;
        vec3 hs = random * 2.0 - 1.0;
        vec3 jitt = hs * factor;
        vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));
        float dDepth = 0.0;
        vec4 coords = RayCast(normalize(reflected + jitt) * max(minRayStep, -viewPos.z), hitPos, dDepth);
        vec2 centered_coords = abs(coords.xy * 2.0 - 1.0);
        float mixer = min(1.0, max(centered_coords.x, centered_coords.y));

        float screenEdgefactor = float(mixer >= 0.9);

        vec2 pixelSize = 1.0 / textureSize(gFinalImage, 0).xy;

        vec3 samples = textureLod(gFinalImage, coords.xy, 0).rgb;

        SSR = mix(samples, texture(gDepth, TexCoords).gba, screenEdgefactor);
    }
    else
    {
        SSR = texture(gDepth, TexCoords, 0).gba;
    }
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, pow(AlbedoSpec.rgb, vec3(1.0)), Metallic);
    vec3 Fresnel = fresnelSchlick(max(dot(worldNormal, viewDir), 0.0), F0);

    outColor = vec4(SSR * (1.0 - AlbedoSpec.w) * (Fresnel * brdf.x + brdf.y), 1.0);

    if(!isMoving && ID <= 250)
        outColor = outColor + PrevReflection;
        
}

vec3 PositionFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = ViewSpaceZFromDepth(textureLod(gDepth, projectedCoord.xy, 0.0).x);
        dDepth = hitCoord.z - depth;

        dir *= 0.5;

        hitCoord += (dDepth > 0.0) ? dir : -dir;
    }
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, dDepth);
}

bool isSignificant(float dd) {
    return dd < 0.05 && dd > depthCutoff;
}

vec4 RayCast(vec3 dir, inout vec3 hitCoord, out float dDepth)
{
    dir *= rayStep;
 
    float depth;
    int steps = 0;
    vec4 projectedCoord;

    hitCoord += dir;
    int miplevel = 0;
    for(int i = 0; i < maxSteps; i++)
    {
 
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = ViewSpaceZFromDepth(texture(gDepth, projectedCoord.xy, miplevel).x);

        if(depth > 1000.0){
            continue;
        }
 
        dDepth = hitCoord.z - depth;
    
        if(isSignificant(dDepth) || (dir.z - dDepth) < 1.5) {
            do {

                depth = ViewSpaceZFromDepth(texture(gDepth, projectedCoord.xy, miplevel).x);
                dDepth = hitCoord.z - depth;

                if(miplevel > 0 && isSignificant(dDepth)){   
                    miplevel--;
                } else if(miplevel == 0 && isSignificant(dDepth)) {
                    return vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);
                    
                } else break;
            
            } while(miplevel > 0 && isSignificant(dDepth));
            miplevel--;
        }
        
        hitCoord += dir;
        dir *= rayIncrement;
        steps++;
        miplevel++;
        miplevel = clamp(miplevel, 0, 5);
    }
 
    if(depth<1000) return vec4(0.0);

    return vec4(BinarySearch(dir, hitCoord, dDepth), 1.0);
    //return vec4(projectedCoord.xy, 0.0, 0.0);
}

float saturate(float x) {
  return clamp(x, 0.0, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    float F90 = saturate(dot(F0, vec3(50.0 * 0.33)));
    return F0 + (F90 - F0) * pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
}

vec3 hash(vec3 a)
{
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}