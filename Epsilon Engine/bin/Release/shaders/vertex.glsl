#version 420 core

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_vertexUV;
layout(location = 2) in vec3 in_normal;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 MVP;

out vec2 TexCoords;
//out vec3 Normal;
out vec3 FragPos;
out mat3 TBN;
out vec3 WorldPos;
vec3 T;
vec3 B;
vec3 N;

mat3 CreateTBNMatrix(mat3 normalMatrix)
{
     T = normalize(normalMatrix * in_tangent);
     B = normalize(normalMatrix * in_bitangent);
     N = normalize(normalMatrix * in_normal);

    return transpose(mat3(T, B, N));
}
void main()
{
	vec4 pos = vec4(in_position, 1.0);

	vec4 viewPos = model * pos;

	FragPos = viewPos.xyz;

	//Normal = in_normal;

	TexCoords = in_vertexUV;

	mat3 NormalMatrix = transpose(inverse(mat3(model)));
	TBN = CreateTBNMatrix(NormalMatrix);

    WorldPos = in_position;  
	gl_Position = projection * view * viewPos;


}