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

out VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
    vec4 FragPos;
} vs_out;

out vec4 pos;

void main()
{
	pos = vec4(in_position, 1.0);

	vec4 viewPos = view * model * pos;

	vs_out.FragPos = viewPos;

	vs_out.Normal = in_normal;
	vs_out.TexCoords = in_vertexUV;

	gl_Position = projection * viewPos;


}