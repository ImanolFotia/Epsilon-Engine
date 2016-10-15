#version 330 core

layout(location = 0) in vec3 positions;
layout(location = 1) in vec2 uv;

uniform vec2 position;
uniform vec2 scale;
uniform vec2 glyph;
uniform int viewportx;
uniform int viewporty;

out vec2 texCoords;
void main()
{
	//vec2 calcpos = vec2((position.x / viewportx), (position.y  / viewporty));
	//vec2 pos = ((positions.xy*0.5+0.5) * glyph + calcpos) * 2 - 1;

	gl_Position = vec4(((positions.xy * glyph) + position) , 0.0, 1.0);

	texCoords = vec2(-uv.x, uv.y);
}