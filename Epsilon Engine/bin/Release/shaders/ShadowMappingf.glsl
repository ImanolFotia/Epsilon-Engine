#version 330 core


uniform sampler2D texture_diffuse;

in vec2 TexCoords;

void main()
{
      if(texture(texture_diffuse, TexCoords).a < 0.3)
            discard;    

}