#version 450 core


layout (lines) in;
layout (line_strip, max_vertices = 8) out;

in VS_OUT {
    vec3 color;
    vec3 normal;
    vec3 tangent;
}gs_in[];

layout (std140, binding = 1) uniform CameraData_t
{
    vec4 position;
    vec4 direction;
    mat4 projection;
    mat4 view;
    mat4 invprojection;
    mat4 invview;
    int CurrentSelectedEntity; float padding2[3];
    float near; float padding0[3];
    float far; float padding1[3];
} CameraData;

void GenerateLine(vec3 vector)
{
    gl_Position = CameraData.projection * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = CameraData.projection * (gl_in[0].gl_Position + 
                                vec4(vector, 0.0) * 0.5);
    EmitVertex();
    EndPrimitive();
}

void GenerateOriginalLine() {
    gl_Position = CameraData.projection * gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = CameraData.projection * gl_in[1].gl_Position;

    EmitVertex();
    EndPrimitive();

}

out vec3 color;

void main() {    
    color = vec3(0.0, 0.0, 1.0);
    GenerateLine(cross(gs_in[0].tangent, gs_in[0].normal));
    color = vec3(0.0, 1.0, 0.0);
    GenerateLine(gs_in[0].normal);
    color = vec3(1.0, 0.0, 0.0);
    GenerateLine(gs_in[0].tangent);
    color = vec3(1.0, 1.0, 1.0);
    GenerateOriginalLine();
    /*
    GenerateLine(gs_in[1].normal);
    GenerateLine(gs_in[1].tangent);*/

} 