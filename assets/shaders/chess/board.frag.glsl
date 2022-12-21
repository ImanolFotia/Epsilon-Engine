#version 450

layout(location = 0) out vec4 outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;


layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

vec3 Gamma(in vec3 img) {
    return pow(img, vec3(1.0/2.2));
}
   
vec3 Degamma(in vec3 img) {
    return pow(img, vec3(2.2));
}

const vec3 dark_color = vec3(181.0/255.0, 136.0/255.0, 99.0/255.0);
const vec3 light_color = vec3(240.0/255.0, 217.0/255.0, 181.0/255.0);

void main() {

    ivec2 tiles = ivec2(texCoords.xy * 8);
    outColor.a = 1.0;
    bool x = (tiles.x % 8) % 2 == 0;
    bool y = (tiles.y % 8) % 2 == 0;

    if(x == false) {
        outColor.rgb = dark_color;
    } else {
        outColor.rgb = light_color;
    }

    if(y == true) {
        if(x == false)
            outColor.rgb = light_color;
        else
            outColor.rgb = dark_color;

        if(x == true)
            outColor.rgb = dark_color;
        else
            outColor.rgb = light_color;
    }
    
    outColor.rgb = outColor.rgb;
}
