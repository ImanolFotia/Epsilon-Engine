#version 440 core

layout(location = 0) in highp vec3 position;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;


layout(binding = 3) uniform sampler2D texture_height;

out vec3 Normal;
out mat3 TBN;

uniform mat4 MVP;
uniform mat4 model;
out vec2 TexCoords;
out vec3 FragPos;

out flat int isX;
out flat int isZ;

uniform int Picked;
uniform float scaleFactor;
uniform int StencilPass;
vec3 T;
vec3 B;
vec3 N;
mat3 CreateTBNMatrix(mat3 normalMatrix)
{
     T = normalize(normalMatrix * tangent);
     B = normalize(normalMatrix * bitangent);
     N = normalize(normalMatrix * normal);

    return transpose(mat3(T, B, N));
}

void main()
{
	isX = 0;
	isZ = 0;

	if(position.z > -0.5 && position.z < 0.5) isZ = 1;
	if(position.x > -0.5 && position.x < 0.5) isX = 1;

	vec3 newPosition = position;

	/*newPosition += normalize(normal)*texture(texture_height, vertexUV).r * 1.25;*/

	if(Picked == 1 && StencilPass == 1) {
		newPosition += normalize(normal)*scaleFactor*0.0075;
	}

	gl_Position = MVP * vec4(newPosition, 1.0f);

  	mat3 normalMatrix = transpose(inverse(mat3(model)));
  	TBN = CreateTBNMatrix(normalMatrix);

	FragPos = vec3(model * vec4(newPosition, 1.0f));
	Normal = normal;
	TexCoords = vertexUV;
}
