#version 440 core

layout(location = 0) out vec4 Color; 
in vec3 Normal; 
in vec2 TexCoords; 
in vec3 FragPos;

in flat int isX; 
in flat int isZ; 
in mat3 TBN;

layout(binding = 0) uniform sampler2D texture_diffuse;
layout(binding = 1) uniform sampler2D texture_specular;
layout(binding = 2) uniform sampler2D texture_normal;
layout(binding = 3) uniform sampler2D texture_height;
layout(binding = 4) uniform samplerCube texture_cubemap;
layout(binding = 5) uniform sampler2D brdfLUT;

uniform int mode;
uniform vec3 camPos;
uniform int Picked;
uniform bool isLine;
vec3 LightDir = vec3(0.5, -1.0, 0.5);

uniform bool animated_skybox = false;

const float PI = 3.14159265359;
const float MAX_REFLECTION_LOD = 8.0;
float metallic = 1.0;
float roughness = 0.15;

float orenNayarDiffuse(
  vec3 ld,
  vec3 vd,
  vec3 sn,
  float r,
  float a) {

  float LdotV = dot(ld, vd);
  float NdotL = dot(ld, sn);
  float NdotV = dot(sn, vd);

  float s = LdotV - NdotL * NdotV;
  float t = mix(1., max(NdotL, NdotV), step(.0, s));

  float sigma2 = r * r;
  float A = 1. - .5 * (sigma2 / ((sigma2 + .33) + .000001));
  float B = .45 * sigma2 / (sigma2 + .09) + .00001;

  float ga = dot(vd - sn * NdotV, sn - sn * NdotL);

  return max(0., NdotL) * (A + B * max(0., ga) * sqrt((1.0 - NdotV * NdotV) * (1.0 - NdotL * NdotL)) / max(NdotL, NdotV));
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;

  float nom = a2;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = PI * denom * denom;

  return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;

  float nom = NdotV;
  float denom = NdotV * (1.0 - k) + k;

  return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx2 = GeometrySchlickGGX(NdotV, roughness);
  float ggx1 = GeometrySchlickGGX(NdotL, roughness);

  return ggx1 * ggx2;
}

float saturate(float x) {
  return clamp(x, 0.0, 1.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness) {
  return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    float F90 = 1.0;//saturate(dot(F0, vec3(50.0 * 0.33)));
    return F0 + (F90 - F0) * pow(1.0 - clamp(cosTheta, 0.0, 1.0), 5.0);
}

vec3 NormalTex = vec3(0.0);
vec4 tex = vec4(0.0);
vec3 F = vec3(0.0);
vec3 kS = vec3(0.0);
vec3 kD = vec3(0.0);

vec3 CalculateDirectionalPBR() {
  // calculate per-light radiance
  vec3 lightcolor = normalize(vec3(205, 109, 39));
  vec3 V = normalize(camPos - FragPos);
  vec3 L = normalize(-LightDir);
  vec3 H = normalize(V + L);
  vec3 N = NormalTex;

  // cook-torrance brdf
  float NDF = DistributionGGX(N, H, clamp(roughness, 0.03, 1.0));
  float G = GeometrySmith(N, V, L, roughness);

  vec3 nominator = NDF * G * F;
  float denominator = (4 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0)) + 0.001;
  vec3 brdf = nominator / denominator+0.001;

  // add to outgoing radiance Lo
  float NdotL = max(orenNayarDiffuse(L, V, N, clamp(roughness, 0.03, 1.0), 1.0), 0.0); // 
  vec3 Lo;
  if (mode == 1)
    Lo = (kD / PI + brdf) * NdotL;
  else
    Lo = (kD * tex.xyz / PI + brdf) * NdotL;

  return Lo /** lightcolor*/ ;
}



vec3 Uncharted2Tonemap(vec3 x) {
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}
float W = 11.2;
const float gamma = 2.2;
const float contrast = 0.3;
const float exposure = 5.5;

vec3 HDR(vec3 image) {
  float ExposureBias = 2.0;
  image *= exposure;
  vec3 curr = Uncharted2Tonemap(ExposureBias * image);

  vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
  vec3 color = curr * whiteScale;

  vec3 result = pow(color, vec3(1.0 / gamma));
  return (result.xyz - 0.5) * (1.0 + contrast) + 0.5;
}


const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;

// Constants for Old Town Square lighting
const vec3 L00 =  vec3( 0.871297,  0.875222,  0.864470);
const vec3 L1m1 = vec3( 0.175058,  0.245335,  0.312891);
const vec3 L10 =  vec3( 0.034675,  0.036107,  0.037362);
const vec3 L11 =  vec3(-0.004629, -0.029448, -0.048028);
const vec3 L2m2 = vec3(-0.120535, -0.121160, -0.117507);
const vec3 L2m1 = vec3( 0.003242,  0.003624,  0.007511);
const vec3 L20 =  vec3(-0.028667, -0.024926, -0.020998);
const vec3 L21 =  vec3(-0.077539, -0.086325, -0.091591);
const vec3 L22 =  vec3(-0.161784, -0.191783, -0.219152);

float calculateAttenuation(float power, float distance)
{
    return power / (4.0f * PI * (distance*distance));
}

//https://colinbarrebrisebois.com/2011/03/07/gdc-2011-approximating-translucency-for-a-fast-cheap-and-convincing-subsurface-scattering-look/
float SubsurfaceScattering(in vec3 L, in vec3 N, in vec3 V, in vec3 P, in bool A){
    
	int iLPower = 5;
  float fLTScale = 10.;
  float fLTThickness = 0.5;
  float fLTAmbient = 0.0;
  float fLightAttenuation = (A ? 1.0/pow(length(P + L), 2.0) : 1.0);
  vec3 vLTLight = normalize(L + N * 1.0);
  float fLTDot = pow(saturate(dot(V, -vLTLight)), float(iLPower)) * fLTScale; 
  float fLT = fLightAttenuation * (fLTDot + fLTAmbient) * fLTThickness;
  
  return fLT;
}

float fAlphaMultiplier = 1.5;

void main() {
  Color = vec4(0.0);
    if (isLine) {
      Color.rgb = vec3(0.5);
      if (isZ == 1) {
        Color.rgb = vec3(230.0/255.0, 50.0/255.0, 0);
      } else if (isX == 1) {
        Color.rgb = vec3(0, 138.0/255.0, 230.0/255.0);
      }
      Color.a = 1.0;
      return;
    }

    if (isZ == 1) {
      Color.rgb = vec3(0.0, 0.0, 0.5);
    }

    if (isX == 1) {
      Color.rgb = vec3(0.5, 0.0, 0.0);
    }

    NormalTex = normalize((texture(texture_normal, TexCoords).rgb * 2.0 - 1.0) * TBN);
    if(!gl_FrontFacing) {
      NormalTex = -NormalTex;
    }
    metallic = texture(texture_height, TexCoords).r;
    float emmisive = texture(texture_height, TexCoords).g;
    roughness = texture(texture_specular, TexCoords).r;
    //roughness = 1.0;
    //metallic = 1.;
    tex = pow(vec4(texture(texture_diffuse, TexCoords)), vec4(1));
    vec3 V = normalize(camPos - FragPos);
    vec3 N = NormalTex;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    
    //if(tex.a < 0.999999999 )
    //  tex.rgb = textureLod(texture_diffuse, TexCoords, 2).rgb;

    //tex.a = textureLod(texture_diffuse, TexCoords, 0).a;


    //tex.a = smoothstep(0.5, 1.0, tex.a);
      
    //if (tex.a < 0.999999 ) discard;
/*
    if (emmisive > 0.5) {
      Color = tex;
      return;
    }*/

    float fAlphaTest = 0.75;

    float fNewAlpha = tex.a * fAlphaMultiplier;
    
    if(fNewAlpha < fAlphaTest)
      discard;
      
   if(fNewAlpha > 1.0f)
      fNewAlpha = 1.0f; 

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, tex.xyz, metallic);

    vec3 L = normalize(-LightDir);
    vec3 H = normalize(V + L);
    F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    kS = F;
    kD = vec3(1.0) - kS;
    vec3 Lo = CalculateDirectionalPBR();

    vec3 L0 = normalize(-FragPos);

    //Lo += max(dot(N, L0), 0.0) * (1.0/pow(length(-FragPos), 2.0)) * kD * tex.rgb;

    F0 = vec3(0.04);
    F0 = mix(F0, tex.rgb, metallic);
    F = fresnelSchlick(max(dot(N, V), 0.0), F0);
    //F = clamp(F, 0.0, 0.5);


    vec3 ambient = vec3(0.05);//(vec3(0.2, 0.2, 0.4) + tex.rgb) * 0.01;

    if(animated_skybox == true) {

      vec3 tnorm = normalize(N);

      ambient = C1 * L22 *(tnorm.x * tnorm.x - tnorm.y * tnorm.y) +
                C3 * L20 * tnorm.z * tnorm.z +
                C4 * L00 - C5 * L20 + 2.0 * C1 * L2m2 * tnorm.x * tnorm.y + 
                2.0 * C1 * L21 * tnorm.x * tnorm.z + 
                2.0 * C1 * L2m1 * tnorm.y * tnorm.z + 
                2.0 * C2 * L11 * tnorm.x +
                2.0 * C2 * L1m1 * tnorm.y +
                2.0 * C2 * L10 * tnorm.z;

      ambient *= 0.1;
    }

    vec4 color = vec4(1.0);
    Color.a = 1.0;
    if (mode == 0) {
      Color.rgb = HDR(tex.xyz);
    }

    if (mode == 1) {
      Color.rgb = HDR(vec3(vec3(Lo)) + vec3(ambient)) * kD;
    }

    if (mode == 2) {
      //tex.rgb = textureLod(texture_diffuse, TexCoords, 0).rgb;
      vec4 texCube = vec4(vec3(0.1), 1.0);
      if(animated_skybox == true) {
        vec4 texCube = pow(texture(texture_cubemap, -normalize(reflect(V, N)),  roughness * MAX_REFLECTION_LOD), vec4(2.2));
      }
      vec3 diffuse = ambient * tex.rgb;
      vec3 specular = texCube.rgb * (F * brdf.x + brdf.y);
      vec3 ibl = (kD * diffuse + specular);
      
      vec3 P = FragPos;
      vec3 LP = vec3(0., 0., 0.);
      V = normalize(P - camPos);

      float SSS = SubsurfaceScattering(LightDir, N, V, P, false);

      L = P - LP;
      
      SSS += SubsurfaceScattering(L, N, V, P, true);

      Color.rgb = HDR((ibl + Lo) + vec3(SSS) * /*vec3(1.0, 0.2, 0.1) **/ pow(tex.rgb, vec3(2.0)));
      

      Color.a = fNewAlpha;
    }

    if (mode == 3) {
      Color.rgb = vec3(N)*0.5+0.5;
    }


}