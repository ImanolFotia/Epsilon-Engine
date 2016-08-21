#version 330


layout(location = 0) out vec4 gAlbedoSpec;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gPosition;
layout(location = 3) out vec3 gExpensiveNormal;
layout(location = 4) out vec3 gWorldSpacePosition;

uniform sampler2D sampler;
uniform sampler2D n_sampler;
uniform sampler2D s_sampler;
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

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return ((2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR)));
}

void main()
{


	vec3 norm = normalize(Normal.rgb);

	gNormal.rgb = norm;
    gNormal.a = gl_FragCoord.z;

	gAlbedoSpec.rgb = clamp(texture2D(sampler, TexCoords).rgb, 0, 10000);

	gAlbedoSpec.a = clamp(texture2D(s_sampler, TexCoords).g, 0, 10000);

	gExpensiveNormal = clamp(normalize((texture2D(n_sampler, TexCoords).rgb * 2.0 - 1.0) * TBN), 0, 10000);

	gPosition.rgb = FragPos;

  	float Depth = LinearizeDepth(gl_FragCoord.z);
  	gPosition.a = Depth;

  	gWorldSpacePosition.rgb = wFragPos;
}
