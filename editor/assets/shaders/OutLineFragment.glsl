#version 440 core

in vec2 TexCoords;
in vec3 FragPos;
layout (binding = 0) uniform sampler2D texture_diffuse;
out vec4 outColor;

uniform float time;

#define Scale vec3(8.03, 8.01, 8.01)
#define K 19.19
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
            vec3 f  = 0.5000*noise( q ); q = m*q*2.01;
            f += 0.2500*noise( q ); q = m*q*2.02;
            f += 0.1250*noise( q ); q = m*q*2.03;
            f += 0.0625*noise( q ); q = m*q*2.04; 
            f += 0.03125*noise( q ); q = m*q*2.05; 
            f += 0.015625*noise( q ); q = m*q*2.06; 
            f += 0.0078125*noise( q ); q = m*q*2.07; 
            f += 0.00390625*noise( q ); q = m*q*2.08;  
    return vec3(f);
}

void main() {
        vec4 tex = vec4(texture(texture_diffuse, TexCoords));
      if(tex.a < 0.3) discard;
    
    /*vec3 bordercolor = fbm(vec3(time*0.1+FragPos.x*0.2, time*0.1+FragPos.y*0.1, time*0.1+FragPos.z*0.3));*/
    
    outColor = vec4(normalize(vec3(255, 133, 27)), 1.0);
}