#version 430 core

layout(location = 0) out vec3 gAlbedoSpec;
layout(location = 1) out vec3 gPosition;
layout(location = 2) out vec4 gExpensiveNormal;
layout(location = 3) out vec4 gDepth;
layout(location = 4) out vec4 gExtraComponents; 
layout(location = 5) out vec3 gLightAccumulation;

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
uniform float u_scaleFactor = 5.0;

// coefficients of spherical harmonics and possible values
vec3 u_L00 = vec3(0.000324516, 0.000124163, 4.67499e-005);
vec3 u_L1m1 = vec3(2.02443e-005, 1.07191e-005, 2.09899e-006);
vec3 u_L10 = vec3(-1.7756e-005, 9.64536e-006, 2.02017e-005);
vec3 u_L11 = vec3(-2.55291e-005, -4.84169e-007, 8.4612e-006);
vec3 u_L2m2 = vec3(-1.76976e-005, -6.2409e-006, -8.05847e-007);
vec3 u_L2m1 = vec3(2.42043e-005, 9.53509e-006, 1.13099e-006);
vec3 u_L20 = vec3(0.000132081, 7.43471e-005, 5.5592e-005);
vec3 u_L21 = vec3(-1.5689e-005, -4.61771e-007, 5.65383e-006);
vec3 u_L22 = vec3(0.000184862, 5.432e-005, 1.24763e-005);


const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;

vec3 sphericalHarmonics(vec3 N)
{
   return
      // band 0, constant value, details of lowest frequency
      C4 * u_L00 +

      // band 1, oriented along main axes
      2.0 * C2 * u_L11 * N.x +
      2.0 * C2 * u_L1m1 * N.y +
      2.0 * C2 * u_L10 * N.z +

      // band 2, values depend on multiple axes, higher frequency details
      C1 * u_L22 * (N.x * N.x - N.y * N.y) +
      C3 * u_L20 * N.z * N.z - C5 * u_L20 +
      2.0 * C1 * u_L2m2 * N.x * N.y +
      2.0 * C1 * u_L21 * N.x * N.z +
      2.0 * C1 * u_L2m1 * N.y * N.z;
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

	gPosition = FragPos;

  	gDepth.x = gl_FragCoord.z;

    gDepth.yzw = vec3(0.0);

  	gLightAccumulation = sphericalHarmonics(gExpensiveNormal.rgb) * u_scaleFactor;
}
