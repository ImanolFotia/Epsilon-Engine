#version 420 core


layout(location = 0) out vec4 FragColor;

#define Scale vec3(0.3, 0.1, 0.1)
#define K 19.19

in vec3 FragPos;

uniform float time;
uniform vec3 LightDirection;

const vec3 Blue = normalize(vec3(29,91,200));
const vec3 Cian = normalize(vec3(125, 228, 247));
const vec3 Orange = normalize(vec3(224,115,36));
const vec3 Gray = normalize(vec3(0.0001,0.0001,0.0001));
const vec3 Black = normalize(vec3(0.0,0.0,0.0));
const vec3 Brown = normalize(vec3(123.0,91.0,68.0));
const vec3 DarkRootwood = normalize(vec3(43, 52, 81));

const float lowerlimit = 0.0;
const float upperlimit = 0.03;

uniform mat4 projection;
uniform mat4 view;
uniform bool state;
in vec3 Normal;

vec3 hash(vec3 p3)
{
	p3 = fract(p3 * Scale);
    p3 += dot(p3, p3.yxz+19.19);
    return fract((p3.xxy + p3.yxx)*p3.zyx);

}

vec3 noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
	
    return mix(mix(mix( hash(p+vec3(0,0,0)), 
                        hash(p+vec3(1,0,0)),f.x),
                   mix( hash(p+vec3(0,1,0)), 
                        hash(p+vec3(1,1,0)),f.x),f.y),
               mix(mix( hash(p+vec3(0,0,1)), 
                        hash(p+vec3(1,0,1)),f.x),
                   mix( hash(p+vec3(0,1,1)), 
                        hash(p+vec3(1,1,1)),f.x),f.y),f.z);
}

const mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );
vec3 fbm(in vec3 q)
{
            vec3    f  = 0.5000*noise( q ); q = m*q*2.01;
                    f += 0.2500*noise( q ); q = m*q*2.02;
                    f += 0.1250*noise( q ); q = m*q*2.03;
                    f += 0.0625*noise( q ); q = m*q*2.04;  
                    f += 0.03125*noise( q ); q = m*q*2.05; 
                    //f += 0.015625*noise( q ); q = m*q*2.06; 
    return vec3(f);
}

float getAverage(in vec3 color)
{
	return dot(color, vec3(0.2126, 0.7152, 0.0722));
}


void main()
{
        vec3 lFragPos = normalize(FragPos);
    	vec3 Clouds = fbm(vec3(lFragPos.x*4.0+time*0.08, lFragPos.y*9.0+time*0.08, lFragPos.z*4.0+time*0.08));

    	float scatteringFactor = clamp((lFragPos.y - lowerlimit) / (upperlimit - lowerlimit), 0.0, 1.0);

        vec3 SunDir = LightDirection; 

        float Sun = smoothstep(0.95, 1.0, max(dot(-Normal, SunDir), 0.0) * 0.80);

    	vec3 Background = mix(Cian, Blue, clamp(lFragPos.y*5.0+0.15, 0.0, 1.0));

        vec3 SkyColor = pow(mix(Gray, Background, scatteringFactor), vec3(2.0));

    	vec3 TimeColor = mix(Gray, SkyColor, clamp(LightDirection.y, 0.0, 1.0));//DarkRootwood*0.2;

        vec3 MixedClouds = sqrt(vec3(vec3(Clouds.x * 0.2126 + Clouds.y * 0.7152 + Clouds.z * 0.0722)*1.5));

    	vec3 color = mix(TimeColor, MixedClouds, clamp(pow(mix(0.0, Clouds.x*3.0, Clouds.z), 3.0), 0.0, 1.0)) * clamp(LightDirection.y, 0.1, 0.7);
        //color = TimeColor;
        vec3 LightColor = normalize(vec3(218, 205, 118));
        float scattering  = max(dot(lFragPos, LightDirection), 0.0);
    	FragColor.rgb = TimeColor*0.05;//clamp(LightDirection.y, 0.01, 1.0);//color*0.2/*+(scattering*0.5)*/ /** max(0.1, LightDirection.y+0.2) */;
        FragColor.a = 1.0;
}