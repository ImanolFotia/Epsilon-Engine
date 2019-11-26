#version 430 core

in vec3 position;
in vec2 TexCoord;

out vec2 TexCoords;
uniform mat4 projection;
uniform vec2 Position;
void main()
{
	gl_Position = projection * vec4(position.xy+Position, 1.0, 1.0);
	TexCoords = TexCoord;
}