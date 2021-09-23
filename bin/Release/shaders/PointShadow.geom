#version 420 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 ViewMatrices[6];

out vec4 FragPos;

in vec2 UV[];
out vec2 TexCoords;

void main() {
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; 
        for(int i = 0; i < 3; ++i) 
        {
	        TexCoords = UV[0];
            FragPos = gl_in[i].gl_Position;
            gl_Position = ViewMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}