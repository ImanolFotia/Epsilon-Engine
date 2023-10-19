#version 460

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "tonemappers.glsl"
#include "bindings.glsl"
#include "PBRFunctions.glsl"

layout (location = 0) out vec4 fragColor;


const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

const vec3 lightDir = normalize(vec3(23.6365, 25.117, -45.772));



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

    ivec2 invFragCoord = ivec2(gl_FragCoord);
    float lDepth = LinearizeDepth(gl_FragCoord.z);

    int mult = 40;
    int cycle = 3;

    int frame = RenderPassUBO.data.iFrame % 64;

    int indexX = invFragCoord.x+int(lDepth);
    int indexY = invFragCoord.y+int(lDepth);

    return IGN(indexX, indexY, frame);
}
void main() {
    Surface surface = InitSurface();

    

    //if(alpha < 1.0)/*ResolveTransparency()*///fAlphaTest < 1.0 - newAlpha)
   //  discard;  

    vec3 viewDir = vec3(RenderPassUBO.data.view[2][0], RenderPassUBO.data.view[2][1], RenderPassUBO.data.view[2][2]);
    vec3 viewPos = vec3(RenderPassUBO.data.view[3][0], RenderPassUBO.data.view[3][1], RenderPassUBO.data.view[3][2]);

    vec4 Albedo = getAlbedo(surface);
    
    const float fAlphaMultiplier = 1.4;
    const float fAlphaTest = 0.8;

    float alpha = Albedo.a;//surface.alpha;
    float newAlpha = alpha * fAlphaMultiplier;
    if(fAlphaTest < 1.0 - newAlpha)
        discard; 

    vec3 Normal = normalize(getNormal(surface));

    if(gl_FrontFacing == false) {
        Normal *= -1.0;
    }
    float roughness = getRoughness(surface);
    roughness *= roughness;
    roughness = clamp(roughness, 0.001, 1.0);
    float metallic = getMetallic(surface);

    vec3 V = normalize(RenderPassUBO.data.viewPosition  - fs_in.position);

    vec3 ref = bpcem(reflect(normalize(-V), Normal), BoxMax, BoxMin, vec3(0.0, 5.0, 0.0));
    
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, Albedo.rgb, metallic);
    
    vec3 F = fresnelSchlickRoughness(max(dot(Normal, V), 0.0), F0, roughness);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	

    vec3 radiance = textureLod(textures[0], SampleSphericalMap(-normalize(ref)), 4.0*roughness).rgb;
    vec3 irradiance = (texture(textures[1], SampleSphericalMap(-Normal)).rgb);
    vec2 lut = textureLod(textures[2], vec2(max(dot(Normal, V), 0.0), 1.0-roughness), 0).rg;

    vec3 reflec = radiance * (F * lut.x + lut.y);
    vec3 diffuse = irradiance * Albedo.rgb;

    vec3 ambient = (kD * diffuse + reflec);

    vec3 light = CalculateDirectionalPBR(lightDir, vec3(1.0), 1.0, RenderPassUBO.data.viewPosition, fs_in.position, F0, Normal, roughness, Albedo.rgb);
    
    fragColor = vec4(tonemapACES(light + ambient), 1.0);

    fragColor.a = 1.0;
}