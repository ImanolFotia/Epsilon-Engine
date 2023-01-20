#version 450

layout(location = 0) out vec4 outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in flat vec4 color;


layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
} ubo;

//layout(binding = 1) uniform sampler2D texSampler;

vec3 Gamma(in vec3 img) {
    return pow(img, vec3(1.0/2.2));
}
   
vec3 Degamma(in vec3 img) {
    return pow(img, vec3(2.2));
}

void main() {
    /*vec3 lightPos = vec3(sin(4.0 * ubo.iTime), cos(4.0 * ubo.iTime), 3.0);
    float l = length(lightPos - position);
    float NoL = max(dot(lightPos, normal), 0.0);
    float i = 5.0 / (l*l);
    
    vec4 texCol = texture(texSampler, texCoords);
    vec3 ambient = vec3(0.05) * texCol.rgb;
    outColor.rgb = texCol.rgb * (vec3(NoL) * i) + ambient;
*/
    outColor.a = color.a;//texCol.a;
    outColor.rgb = vec3(color.rgb);//(Gamma(outColor.rgb));
}
