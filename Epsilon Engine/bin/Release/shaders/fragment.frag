#version 420 core

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_specular;
layout (binding = 2) uniform sampler2D texture_normal;
layout (binding = 3) uniform sampler2D texture_height;
layout (binding = 4) uniform samplerCube skybox;

out vec4 Color;

uniform vec3 lightDir;

in vec2 TexCoords;
in vec3 Normal;
in vec4 FragPos;
in mat3 TBN;

void main()
{

	vec3 LightPosition = vec3(37, 6.5, 3.5);
	vec3 lightDirection = normalize(LightPosition - FragPos.xyz);

	float emissive = texture(texture_height, TexCoords).y;
	vec4 Albedo = texture(texture_diffuse, TexCoords);

	if(Albedo.a < 0.2)
    	discard; 

	if(emissive > 0.0)
	{
		Color = Albedo;
		return;
	}

	vec3 NormalTexture = texture(texture_normal, TexCoords).rgb * 2.0 - 1.0;
	vec3 mNormal = normalize(NormalTexture * TBN);

        vec3 lightcolor = normalize(vec3(205, 109, 39));
	vec3 lambert = max(dot(mNormal, lightDirection), 0.0)*vec3(0.3)/* * lightcolor*/;

	Color = (vec4(lambert, 1.0) * Albedo);
}



