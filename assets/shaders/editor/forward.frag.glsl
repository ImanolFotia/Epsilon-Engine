#version 460

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "tonemappers.glsl"
#include "bindings.glsl"
#include "PBRFunctions.glsl"

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec2 velocityBuffer;

layout (location=10) in vec4 Position;
layout (location=11) in vec4 PrevPosition;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

const vec3 lightDir = normalize(vec3(23.6365, 25.117, -45.772));


const float fAlphaMultiplier = 1.4;
const float fAlphaTest = 0.8;

//Parallax corrected cubemap limits, should be converted into uniforms
const vec3 BoxMax = vec3(30.0, 15.0, 30.0);
const vec3 BoxMin = vec3(-30.0, -1.0, -30.0);
vec3 bpcem (in vec3 v, vec3 Emax, vec3 Emin, vec3 Epos)
{ 
  vec3 nrdir = normalize(v);
  vec3 rbmax = (Emax - fs_in.position)/nrdir;
  vec3 rbmin = (Emin - fs_in.position)/nrdir;
  
  vec3 rbminmax;
  rbminmax.x = (nrdir.x>0.0)?rbmax.x:rbmin.x;
  rbminmax.y = (nrdir.y>0.0)?rbmax.y:rbmin.y;
  rbminmax.z = (nrdir.z>0.0)?rbmax.z:rbmin.z;   
  float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);
  vec3 posonbox = fs_in.position + nrdir * fa;
  return posonbox - Epos;
}

float NEAR = 0.01;
float FAR = 3000.0;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return ((2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR)));
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

    ivec2 invFragCoord = ivec2(gl_FragCoord.xy);
    float lDepth = LinearizeDepth(gl_FragCoord.z);

    int mult = 40;
    int cycle = 3;

    int frame = RenderPassUBO.data.iFrame % 64;

    int indexX = invFragCoord.x+int(lDepth);
    int indexY = invFragCoord.y+int(lDepth);

    return IGN(indexX, indexY, frame);
}

//from filament renderer
float perceptualRoughnessToLod(float perceptualRoughness) {
    // The mapping below is a quadratic fit for log2(perceptualRoughness)+iblRoughnessOneLevel when
    // iblRoughnessOneLevel is 4. We found empirically that this mapping works very well for
    // a 256 cubemap with 5 levels used. But also scales well for other iblRoughnessOneLevel values.
    return 5 * perceptualRoughness * (2.0 - perceptualRoughness);
}
float F0_from_ior(float eta)
{
  float A = (eta - 1.0) / (eta + 1.0);
  return A * A;
}
float ior_from_F0(float F0)
{
  float f = sqrt(clamp(F0, 0.0, 0.99));
  return (-f - 1.0) / (f - 1.0);
}



void main() {
    Surface surface = InitSurface();
    
    vec4 Albedo = getAlbedo(surface);
    Albedo.rgb = Gamma(Albedo.rgb);
    vec3 Normal = normalize(getNormal(surface));
    float specular = getSpecular(surface);
    float roughness = getRoughness(surface);
    float metallic = getMetallic(surface);

    //vec3 diffuseColor = (1.0 - metallic) * Albedo.rgb;

    if(gl_FrontFacing == false) {
        Normal *= -1.0;
    }

    float alpha = Albedo.a;
    float newAlpha = alpha * fAlphaMultiplier;

    if(ResolveTransparency() < 1.0 - newAlpha)
        discard; 

    
    vec3 viewDir = vec3(RenderPassUBO.data.view[2][0], RenderPassUBO.data.view[2][1], RenderPassUBO.data.view[2][2]);
    vec3 viewPos = vec3(RenderPassUBO.data.view[3][0], RenderPassUBO.data.view[3][1], RenderPassUBO.data.view[3][2]);


    roughness = clamp(roughness, 0.001, 1.0);
    float perceptualRoughness = roughness * roughness;

    perceptualRoughness = pow(perceptualRoughness, 1.0 / 2.2);


    vec3 V = normalize(RenderPassUBO.data.viewPosition  - fs_in.position);

    vec3 ref = bpcem(reflect(normalize(-V), Normal), BoxMax, BoxMin, vec3(0.0, 5.0, 0.0));
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo.rgb, max(metallic, specular));
    
    vec3 F = fresnelSchlickRoughness(max(dot(Normal, V), 0.0), F0, perceptualRoughness);
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);

    float lod = perceptualRoughnessToLod(perceptualRoughness);

    lod =  clamp(lod, 0, 4.999);

    vec3 radiance0 = Gamma(textureLod(textures[0], SampleSphericalMap(-normalize(ref)), floor(lod)).rgb);
    vec3 radiance1 = Gamma(textureLod(textures[0], SampleSphericalMap(-normalize(ref)), ceil(lod)).rgb);

    vec3 radiance = mix(radiance0, radiance1, fract(lod));
    
    vec3 irradiance = (Gamma(textureLod(textures[1], SampleSphericalMap(-Normal), 0).rgb));

    vec2 lut = textureLod(textures[2], vec2(max(dot(Normal, V), 0.0), perceptualRoughness), 0).rg;

    float att = 1.0;
    
    if(lod > 5.0) {
        att = 1.0-(fract(lod));
        vec3 radiance2 = Gamma(textureLod(textures[1], SampleSphericalMap(-normalize(ref)), clamp(lod, 0, 6)).rgb);
        radiance = mix(radiance2, radiance, att);
    }

    radiance = (radiance*metallic + radiance*specular) * 0.5;

    vec3 reflec = radiance  * (F * lut.x + lut.y);

    vec3 diffuse = irradiance * Albedo.rgb;

    vec3 ambient = (kD * diffuse + reflec);

    vec3 light = CalculateDirectionalPBR(lightDir, vec3(1.0), 1.0, RenderPassUBO.data.viewPosition, fs_in.position, F0, Normal, perceptualRoughness, Albedo.rgb, specular);
    
    fragColor = vec4(tonemapACES(light + ambient), 1.0);
    fragColor.a = 1.0;

    vec2 a = (Position.xy / Position.w) * 0.5 + 0.5;
    vec2 b = (PrevPosition.xy / PrevPosition.w) * 0.5 + 0.5;

    velocityBuffer = vec2(a - b);
}