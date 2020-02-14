#version 440 core

layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoords;
layout(location=2) in vec3 normal;

out vec2 texCoords;
out vec4 VertexColor;
out vec3 Normal;
uniform mat4 MVP;
uniform float time;
uniform int repeat;
/*
float hash( float n ) {
    return fract(sin(n)*43758.5453123);
}

float noise( in vec2 x ) {
    vec2 p = floor(x);
    vec2 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0;

    return mix(mix( hash(n+  0.0), hash(n+  1.0),f.x), mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y);
}﻿

float fbm(vec2 p)
{
	float f = 0.0;
	f+=0.5 * noise(p); p*=2.02;
	f+=0.25 * noise(p); p*=2.03;
	f+=0.125 * noise(p); p*=2.01;
	f+=0.0625 * noise(p); p*=2.04;
	return f/0.9375;

}

const float height = 120.0;
*/
void main()
{

	//float n = fbm((position.xz) * 0.009) * height;
	//float m = n;
	texCoords  =texcoords * repeat;
	Normal = normal;
	gl_Position = MVP * vec4(position, 1.0f);
	VertexColor = vec4(position.y);

}