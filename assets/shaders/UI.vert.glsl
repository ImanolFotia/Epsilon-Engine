#version 450 core


layout(location = 0) in vec4 pos_coords;
layout(location = 1) in vec4 color;
layout(location = 2) in vec2 vtx_coords;
layout(location = 3) in vec2 padding;
layout(location = 4) in vec4 attributes;

layout(location = 0) out VARYING  {
    vec4 color;
    vec4 vtx_coord;
    vec4 attributes;
    vec2 texCoords;
    float radius;
} vs_out;


struct RenderPass {
    float iTime;
    vec2 iResolution;
    vec3 lightPosition;
	vec3 viewPosition;
    mat4 view;
    mat4 proj;
    mat4 prev_view;
    mat4 prev_proj;
    mat4 lightMatrix;
    int iFrame;
    ivec2 iMouse;
};

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    RenderPass data;
}
RenderPassUBO;


vec2 corners[] = {
    vec2(0.0, 0.0),
    vec2(1.0, 0.0),
    vec2(0.0, 1.0),
    vec2(0.0, 1.0),
    vec2(1.0, 0.0),
    vec2(1.0, 1.0)
};


layout( push_constant ) uniform constants
{
  mat4 transform;
  uint material_index;
  uint animation_offset;
} PushConstants;

void main() {
    gl_Position = /*RenderPassUBO.data.proj*/PushConstants.transform * vec4(pos_coords.xy * 2.0 - 1.0, 0.0, 1.0);
    vs_out.color = color;
    vs_out.texCoords = pos_coords.zw;
    vs_out.vtx_coord.xy = corners[int(vtx_coords.x)];
    vs_out.vtx_coord.zw = padding;
    vs_out.radius = vtx_coords.y;
    vs_out.attributes = attributes;
}
