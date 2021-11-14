#version 450 core
/*
120 - h => 600
68 - v => 340
*/
const int MAX_SURFELS = 204000

layout (local_size_x = 16, local_size_y = 16) in;

layout (binding = 0) uniform sampler2D gDepth;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAlbedoSpec;

struct Surfel {
    vec3 position; float padding0;
    vec3 normal; float padding1;
    vec3 color;
    float radius;
};

layout(std430, binding = 1) buffer outputSurfelSSBO
{
    Surfel outSurfels[MAX_SURFELS];

} surfelBuffer;

vec3 PositionFromDepth(float depth, vec2 texCoords) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(texCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

void main() {
    ivec2 texCoords = ivec2(gl_GlobalInvocationID.xy);
    
}