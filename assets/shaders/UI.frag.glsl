#version 450 core

layout (location = 0) out vec4 fragColor;

layout(location = 0) in VARYING  {
    vec4 color;
    vec4 vtx_coord;
    vec4 attributes;
    vec2 texCoords;
    float radius;
} fs_in;

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

layout(binding = 1) uniform sampler2D tex;
float sdBox( in vec2 p, in vec2 b )
{
    vec2 d = abs(p)-b;
    return length(max(d,0.0)) + min(max(d.x,d.y),0.0);
}

float udRoundBox( vec2 p, vec2 b, float r ) { return length(max(vec2(abs(p.x), abs(p.y))-b+r,0.0))-r; }


float screenPxRange(sampler2D tex, vec2 uvCoord) {
	// Precalculate unitRange and pass it as 
	// a uniform for better performance.
	vec2 unitRange = vec2(50.0)/vec2(textureSize(tex, 0));
	vec2 screenTexSize = vec2(1.0)/fwidth(uvCoord);
	return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}
void main() {

	vec4 texel;
	float pxRange; 

    vec2 uv = fs_in.texCoords;
    //uv.y = 1.0 - uv.y;

	texel = texture(tex, uv);
	pxRange = screenPxRange(tex, uv);
    float dist = median(texel.r, texel.g, texel.b);
	
	// Distance (in pixels) to the body edge and calculate opacity
  	float pxDist = pxRange * (dist - 0.5);
	float opacity = clamp(pxDist + 0.5, 0.0, 1.0);

    
    //texel.rgb = pow(texel.rgb, vec3(1.0/2.2));
    if(fs_in.attributes.x >= 0.9) {
        fragColor = fs_in.color * vec4(1.0); 
    } else {
        fragColor = fs_in.color * vec4(texel.rgb, dist); //textureLod(tex, uv, 0);
    }

    //fragColor = vec4(vec3(texel.a), 1.0);
    if(fs_in.radius >= 0) {
        vec2 size = fs_in.vtx_coord.zw;
        float radius = fs_in.radius * 100;//(fs_in.radius / dot(fs_in.vtx_coord.zw, fs_in.vtx_coord.zw)) * 50000.0;
        float circle_sdf = udRoundBox(fs_in.vtx_coord.xy * size - 0.5 * size, 0.5 * size - 0.0005, radius);
        float circle = circle_sdf / fwidth(circle_sdf);
        fragColor.a *= clamp(1.0-circle, 0.0, 1.0);
    }
}
