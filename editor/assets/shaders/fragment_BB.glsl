#version 330 core

out vec4 Color;

in vec3 color;
in vec2 TexCoords;
in vec3 FragPos;

void main()
{
	Color = vec4(normalize(vec3(255, 133, 27)),1.0);

}