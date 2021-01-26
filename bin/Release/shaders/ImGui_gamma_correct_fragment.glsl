#version 410 core

in vec2 Frag_UV;
in vec4 Frag_Color;
uniform sampler2D Texture;
layout (location = 0) out vec4 Out_Color;
void main()
{
    Out_Color = vec4(pow(Frag_Color * texture(Texture, Frag_UV.st), vec4(2.2)).rgb, 1.0);
}