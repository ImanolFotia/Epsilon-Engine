#version 440 core

out vec4 FragColor;

layout (binding = 0) uniform sampler2D texture_diffuse;
uniform int hasTexture = 1;

in vec4 VertexColor;
in vec2 texCoords;
in vec3 Normal;

vec3 LightDir = vec3(0.5, -1.0, 0.5);

vec3 Uncharted2Tonemap(vec3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}
	float W = 11.2;
const float gamma = 2.2;
const float contrast = 0.1;
const float exposure = 5.5;
vec3 HDR(vec3 image)
{
    float ExposureBias = 2.0f;
    image*= exposure;
   vec3 curr = Uncharted2Tonemap(ExposureBias*image);

   vec3 whiteScale = 1.0f/Uncharted2Tonemap(vec3(W));
   vec3 color = curr*whiteScale;
      
   vec3 result = pow(color,vec3(1.0/gamma));
   return (result.xyz - 0.5f) * (1.0 + contrast) + 0.5f;
}

void main()
{

	float lambert = max(dot(normalize(-LightDir), normalize(Normal)), 0.0);
	vec3 tex = vec3(0.0);

	vec3 ambient = vec3(0.2, 0.2, 0.4)*0.1;

	//if(hasTexture == 1)
		tex = texture(texture_diffuse, texCoords).rgb;

	FragColor = vec4(HDR(ambient + vec3(lambert) * tex), 1.0);
}