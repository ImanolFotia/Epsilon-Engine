#version 330 core

uniform sampler2D tex;

in vec2 TexCoords;

void main()
{
	if(texture(tex, TexCoords).a < 0.9)
		discard;

	
}