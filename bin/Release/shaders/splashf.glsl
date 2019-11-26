#version 440 core

in vec2 TexCoords;

out vec4 Color;

layout (binding = 0) uniform sampler2D tex;

void main()
{
	Color.rgb = texture(tex, vec2(TexCoords.x, -TexCoords.y)).rgb;
	Color.a = 1.0f;
}