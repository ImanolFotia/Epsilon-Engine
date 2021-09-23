#version 420 core

#extension GL_EXT_geometry_shader4 : enable
#extension GL_NV_viewport_array2 : enable
#extension GL_NV_geometry_shader_passthrough : require

layout(triangles) in;
layout(passthrough) in gl_PerVertex {
	vec4 gl_Position;
};

layout(passthrough) in VS_OUT {
    vec2 TexCoords;
    vec3 Normal;
} passthroughData[];


layout(viewport_relative) out int gl_Layer;

void main()
{
	// Required by spec to set this...
	gl_Layer = 0;

	gl_ViewportMask[0] = 0x3f;
}
