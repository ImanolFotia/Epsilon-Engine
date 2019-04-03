#version 430 core

layout(location = 0) out vec3 gAlbedoSpec;
layout(location = 1) out vec4 gExpensiveNormal;
layout(location = 2) out vec4 gDepth;
layout(location = 3) out vec4 gExtraComponents; 
layout(location = 4) out vec3 gLightAccumulation;

uniform sampler2D sampler;
uniform sampler2D n_sampler;
uniform sampler2D s_sampler;
uniform sampler2D m_sampler;
uniform vec3 viewPos;
uniform vec3 lightDir;
uniform float texmix;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;
in vec3 wFragPos;

float NEAR = 0.1;
float FAR = 3000.0;

const float Pi = 3.141592654f;
const float CosineA0 = Pi;
const float CosineA1 = (2.0f * Pi) / 3.0f;
const float CosineA2 = Pi * 0.25f;
// scale for restored amount of lighting
uniform float u_scaleFactor = 1.0;

// coefficients of spherical harmonics and possible values
vec3 u_L00 = vec3(0.000583486, 0.000571206, 0.00103499);
vec3 u_L1m1 = vec3(0.000285382, 0.000283877, 0.000577816);
vec3 u_L10 = vec3(-6.56216e-006, -6.49363e-006, -1.13807e-005);
vec3 u_L11 = vec3(-4.21726e-005, -4.90659e-005, -9.87656e-005);
vec3 u_L2m2 = vec3(-0.000108929, -0.000115347, -0.000255311);
vec3 u_L2m1 = vec3(-6.82438e-007, -5.38153e-007, 1.84106e-007);
vec3 u_L20 = vec3(-5.45785e-005, -5.16044e-005, -0.00011851);
vec3 u_L21 = vec3(-4.20471e-006, -3.81651e-006, -7.40711e-006);
vec3 u_L22 = vec3(-7.63505e-005, -7.64585e-005, -0.000186511);

struct SphericalHarmonicsFormat {
  vec3 u_L00; 
  vec3 u_L1m1;
  vec3 u_L10;
  vec3 u_L11;
  vec3 u_L2m2;
  vec3 u_L2m1;
  vec3 u_L20;
  vec3 u_L21;
  vec3 u_L22;
};

layout(std430, binding = 1) buffer AmbientLightSSBO
{
    //SphericalHarmonicsFormat ambientProbes[];

        vec3 u_L00; float padding0;
        vec3 u_L1m1; float padding1;
        vec3 u_L10; float padding2;
        vec3 u_L11; float padding3;
        vec3 u_L2m2; float padding4;
        vec3 u_L2m1; float padding5;
        vec3 u_L20; float padding6;
        vec3 u_L21; float padding7;
        vec3 u_L22; float padding8;

} lp;

const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;

vec3 sphericalHarmonics(vec3 N)
{
   return
      // band 0, constant value, details of lowest frequency
      C4 * lp.u_L00 +

      // band 1, oriented along main axes
      2.0 * C2 * lp.u_L11 * N.x +
      2.0 * C2 * lp.u_L1m1 * N.y +
      2.0 * C2 * lp.u_L10 * N.z +

      // band 2, values depend on multiple axes, higher frequency details
      C1 * lp.u_L22 * (N.x * N.x - N.y * N.y) +
      C3 * lp.u_L20 * N.z * N.z - C5 * lp.u_L20 +
      2.0 * C1 * lp.u_L2m2 * N.x * N.y +
      2.0 * C1 * lp.u_L21 * N.x * N.z +
      2.0 * C1 * lp.u_L2m1 * N.y * N.z;
}

void main()
{
	vec3 norm = normalize(Normal.rgb);
	vec2 metallic_emissive = texture(m_sampler, TexCoords).xy;
	gExtraComponents.rg = metallic_emissive;

  	gExtraComponents.zw = vec2(0.0);

	gAlbedoSpec = texture(sampler, TexCoords).rgb;

	gExpensiveNormal.rgb = normalize((texture(n_sampler, TexCoords).rgb * 2.0 - 1.0) * TBN);

	gExpensiveNormal.a = texture(s_sampler, TexCoords).r;

  	gDepth.x = gl_FragCoord.z;

    gDepth.yzw = vec3(0.0);

  	gLightAccumulation = sphericalHarmonics(gExpensiveNormal.rgb) * u_scaleFactor;
}
