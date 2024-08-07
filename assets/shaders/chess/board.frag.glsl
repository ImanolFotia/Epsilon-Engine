#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;



layout(binding = 1) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
    vec4 lastMove;
} ubo;

layout(binding = 2) uniform sampler2D texSampler;

layout (set = 1, binding = 0) uniform sampler2D textures[];

vec3 Gamma(in vec3 img) {
    return pow(img, vec3(1.0/2.2));
}
   
vec3 Degamma(in vec3 img) {
    return pow(img, vec3(2.2));
}

const vec3 dark_color = vec3(119.0/255.0, 149.0/255.0, 86.0/255.0);
const vec3 light_color = vec3(235.0/255.0, 236.0/255.0, 208.0/255.0);

void main() {

    ivec2 tiles = ivec2(texCoords.xy * 8);
    outColor.a = 1.0;

    if((tiles.x == ubo.lastMove.z-1 && tiles.y == 7-ubo.lastMove.x) || (tiles.x == ubo.lastMove.w-1 && tiles.y == 7-ubo.lastMove.y)) {
        outColor.rgb = vec3(1.0, 0.5, 0.5);
    } else {
        bool x = (tiles.x % 8) % 2 == 0;
        bool y = (tiles.y % 8) % 2 == 0;

        outColor.rgb = (x ^^ y) ? dark_color : light_color;
    }
    outColor.a = 1.0;
    outColor.rgb = outColor.rgb;
}
