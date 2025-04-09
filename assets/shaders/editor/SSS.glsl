#version 450 core

#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable

#include "bindings.glsl"


layout (binding = 2) uniform sampler2D gDepth;


#pragma optionNV (unroll all)
#define USE_YCOCG

layout(location = 0) out vec4 FragColor;

layout(binding = 1) uniform SAAData_t {
	float step_count;
	float depth_threshold;
	float step_size;
} SSSData;

float saturate(in float x) {
    return clamp(x, 0.0, 1.0);
}

void main()
{ 
   FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}