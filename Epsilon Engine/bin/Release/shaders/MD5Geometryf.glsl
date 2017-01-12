#version 330


layout(location = 0) out vec4 gAlbedoSpec;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec3 gPosition;
layout(location = 3) out vec4 gExpensiveNormal;
layout(location = 4) out float gDepth;

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

void main()
{
	vec3 norm = normalize(Normal.rgb);
	vec3 metallic_emissive = texture(m_sampler, TexCoords).xyz;
	gNormal.rgb = norm;
    gNormal.a = metallic_emissive.r;

	gAlbedoSpec.rgb = texture(sampler, TexCoords).rgb;

	gAlbedoSpec.a = texture(s_sampler, TexCoords).g;

	gExpensiveNormal.rgb = normalize((texture(n_sampler, TexCoords).rgb * 2.0 - 1.0) * TBN);
	gExpensiveNormal.a = metallic_emissive.g;

	gPosition.rgb = FragPos;

  	gDepth = gl_FragCoord.z;
}
