#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;
layout (location = 4) in mat3 invView;



layout(binding = 0) uniform UniformBufferObject
{

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
}
ubo;
layout (set = 1, binding = 0) uniform sampler2D textures[];

float NEAR = 0.1;
float FAR  = 1000.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return ((2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR)));
}

const vec3 color0 = vec3(1.0, 0.0, 0.0);
const vec3 color1 = vec3(0.0, 1.0, 0.0);

void main() {
     vec2 uv = gl_FragCoord.xy / ubo.iResolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= ubo.iResolution.x / ubo.iResolution.y;

    vec3 ray = normalize(invView * vec3(uv, 1.0));
    
        
        vec3 backcol = mix(vec3( 0.92, 0.95, 0.99 ), vec3( 0.45, 0.68, 0.88 ), clamp(-ray.y +0.5, 0.0, 1.));

        outColor = vec4(backcol, 1.0);
}