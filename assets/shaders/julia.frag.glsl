#version 460
#extension GL_EXT_nonuniform_qualifier : enable
#extension VK_EXT_descriptor_indexing : enable


layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
} ubo;

layout (location = 0) in vec3 color;
layout (location = 1) in vec2 fragCoord;


layout( push_constant ) uniform constants
{
	mat4 model;
} PushConstants;

vec2 powi(vec2 axbi){
    return vec2(axbi.x*axbi.x - axbi.y*axbi.y, 2.0*axbi.x*axbi.y);
}

float JuliaSet(inout vec2 z, in vec2 c)
{
    float t = 0.0;
    for(int i = 0; i < 128; i++)
    {
        z = powi(z) + c;
        
        if(dot(z,z) > 1024.0)
            break;
        
        t++;
    }
    
    return t;
}

void main()
{
	vec2 uv = fragCoord * 2.0 - 1.0;

    const float time = ubo.iTime;
    
    vec2 c = vec2(-1.5, 1.5) * vec2(cos(ubo.iTime*0.287), sin(ubo.iTime*.587))*0.4;
    vec2 z = uv;
    
    float f = JuliaSet(z, c);
    
    //f = sin(log(f)*.5);
    //Smooth iteration count by Iñigo Quilez
    f = f - log2(log2(dot(z,z))) + 4.0;
    
    //color gradient
    vec3 col = vec3(cos(f*0.3), sin(f*0.2), cos(f*0.5));
    
	outColor = vec4(col,1.0);
}