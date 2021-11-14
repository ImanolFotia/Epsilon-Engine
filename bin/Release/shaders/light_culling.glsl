#version 450 core

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0) uniform sampler2D gDepth;

layout (rgba32i, binding = 0) uniform image2D outImage;

struct light_t {
    vec4 position;  // 4
    vec4 direction; // 8
    vec4 color;     // 12
    float radius; float padding0[3]; // 13
    float watts;  float padding1[3]; // 14
    int type;     float padding2[3];    //15
};

layout(std430, binding = 0) buffer ssbo
{
    light_t Lights[];

} LightBuffer;

void main() {
    ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

    imageStore(outImage, pixel_coords, vec4(1.0, 0.0, 0.0, 1.0));
}