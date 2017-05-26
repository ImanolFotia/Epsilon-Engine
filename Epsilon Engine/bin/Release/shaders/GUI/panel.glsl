#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

void main()
{
	FragColor = vec4(vec3(0.0, 0.0, 0.0), 0.5) * 0.5;
}