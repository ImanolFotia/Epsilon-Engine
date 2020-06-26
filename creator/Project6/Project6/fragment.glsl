#version 330 core

out vec4 FragColor;
precision highp float;

/********************************

Very basic Mandelbrot fractal rendering.

It uses the smooth iteration count as described here: 
http://www.iquilezles.org/www/articles/mset_smooth/mset_smooth.htm


********************************/

#define MAX_ITERATIONS 128
#define B 4.0
#define ZOOM 1.15
#define POSITION vec2(-0.5, 0.0)

//float iTime = 1.0;

vec2 fragCoord = gl_FragCoord.xy;

uniform vec2 iResolution;
uniform float iTime;

//Function to square complex numbers
vec2 ipow2(vec2 i)
{
    return vec2(i.x*i.x - i.y*i.y, 2.0*i.x*i.y);
}

vec2 ipow3 (vec2 i)
{
    return vec2 ( pow(i.x, 3.0) - 3.0 * i.x * pow(i.y, 2.0), 
                 3.0 *pow(i.x, 2.0) * i.y - pow(i.y, 3.0));
}

vec2 ipow4 (vec2 i)
{
    return vec2(pow(i.x, 4.0) - 6.0 * pow(i.x, 2.0) * pow(i.y, 2.0) + pow(i.y, 4.0),
               4.0 * pow(i.x, 3.0) * i.y - 4.0 * i.x * pow(i.y, 3.0));
}

//Fractal calculation loop
float mandelbrot(in vec2 c, inout vec2 z){
    //Don't use floating point numbers in a for loop, it's evil.
    float t;
    for(int i=0;i<MAX_ITERATIONS;++i){
        z = ipow2(z) + c;
        //Does it go off bounds?
        if( dot(z,z)>B*B ) {
            break;
        }
        t+=1.0;
    }
    
    return t;
}

void main(){
	vec2 uv = ( (fragCoord) / iResolution.xy
        		* 2.0-1.0)
        		* vec2(iResolution.x/iResolution.y, 1.0);
    
    float time = iTime;
    
    //Center the uv coordinates
    vec2 c = uv * ZOOM + POSITION;
    vec3 col = vec3(0.0);
    vec2 z = vec2(0.0);
    
	float f = mandelbrot(c, z);
    
    //Smooth iteration count by Iñigo Quilez
    f = f - log2(log2(dot(z,z))) + 4.0;
    
    //color gradient
    col = vec3(cos(f*0.3+time), sin(f*0.2+time), cos(f*0.5+time));
    
	FragColor = vec4(normalize(col), 1.0);
/*
    fragCoord.x = fragCoord.x < 0.5 ? 0.0 : fragCoord.x;
    fragCoord.x = fragCoord.x > 1.0 ? 0.0 : fragCoord.x;
    
    fragCoord.y = fragCoord.y < 0.5 ? 0.0 : fragCoord.y;
    fragCoord.y = fragCoord.y > 1.0 ? 0.0 : fragCoord.y;

    fragCoord = (fragCoord*2.0-1.0) / iResolution;

    vec2 circle = smoothstep(fragCoord, vec2(0.0), vec2(1.0));

    FragColor = vec4(vec3(fragCoord, 0.0), 1.0);*/
}