#version 430 core

out vec4 FragColor;

uniform sampler2D texture0;

in vec2 TexCoords;

void main()
{
	FragColor = textureLod(texture0, TexCoords, 0.0);
}