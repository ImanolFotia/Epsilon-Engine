#version 450

layout(location = 0) out vec4 outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;


layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    vec3 fgfg;
    mat4 view;
    mat4 proj;
    mat4 matttt;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

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
    
    bool x = (tiles.x % 8) % 2 == 0;
    bool y = (tiles.y % 8) % 2 == 0;

    outColor.rgb = (x ^^ y) ? dark_color : light_color;
    
    outColor.a = 1.0;
    outColor.rgb = outColor.rgb;
}
