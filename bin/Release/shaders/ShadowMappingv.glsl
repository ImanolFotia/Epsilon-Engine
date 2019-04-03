#version 330 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 uv;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
out vec2 TexCoords;

void main()
{
	gl_Position = lightSpaceMatrix * model * vec4(position, 1.0f);
	TexCoords = uv;

}