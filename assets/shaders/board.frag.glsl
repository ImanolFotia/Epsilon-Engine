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

void main() {
    /*
    vec3 lightPos = vec3(sin(4.0 * ubo.iTime), cos(4.0 * ubo.iTime), 3.0);
    float l = length(lightPos - position);
    float NoL = max(dot(lightPos, normal), 0.0);
    float i = 5.0 / (l*l);
    
    vec3 texCol = texture(texSampler, texCoords).rgb;
    vec3 ambient = vec3(0.05) * texCol;
    outColor = vec4(texCol * (vec3(NoL) * i) + ambient, 1.0) ;*/

    ivec2 tiles = ivec2(texCoords.xy * 8);
    outColor.a = 1.0;
    bool x = (tiles.x % 8) % 2 == 0;
    bool y = (tiles.y % 8) % 2 == 0;

    if(x == false) {
        outColor.rgb = vec3(0.1);
    } else {
        outColor.rgb = vec3(1.0);
    }

    if(y == true) {
        if(x == false)
            outColor.rgb = vec3(1.0);
        else
            outColor.rgb = vec3(0.1);

        if(x == true)
            outColor.rgb = vec3(0.1);
        else
            outColor.rgb = vec3(1.0);
    }
}