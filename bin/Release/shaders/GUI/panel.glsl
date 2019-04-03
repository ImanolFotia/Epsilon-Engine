#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform vec4 color = vec4(vec3(0.0, 0.0, 0.0), 0.5) * 0.5;

void main()
{
	FragColor = color;
}