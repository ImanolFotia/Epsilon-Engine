#version 460
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#include "tonemappers.glsl"
#include "bindings.glsl"

layout(location = 0) out vec4 outColor;


float ddxy(float x) {
    return abs(dFdx(x)) + abs(dFdy(x));
}

void main() {
    
    vec2 uv = vec2(fs_in.position.x,fs_in.position.z);
    vec2 gradient = vec2(ddxy(uv.x), ddxy(uv.y));
    
    vec2 placer_grid = gradient;

    vec2 w = max(abs(dFdx(uv)), abs(dFdy(uv))) + 0.01;

	// analytic (box) filtering
    const float N = 15.0;
    vec2 a = uv + 0.5*w;                        
    vec2 b = uv - 0.5*w;           
    vec2 i = (floor(a)+min(fract(a)*N,1.0)-
              floor(b)-min(fract(b)*N,1.0))/(N*w);

    float grid = (1.0-i.x)*(1.0-i.y);
    grid = 1.0-grid;

    if(grid < 0.5+dFdy(uv.y/uv.x)) discard;

    outColor = vec4(grid);
}