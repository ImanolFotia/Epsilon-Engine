#define pi 3.141592653589793

// Rotate a vec2
vec2 rotate(vec2 vec, float rot)
{
    float s = sin(rot), c = cos(rot);
    return vec2(vec.x*c-vec.y*s, vec.x*s+vec.y*c);
}

// Hash13 Hash without Sine: https://www.shadertoy.com/view/4djSRW
float hash(vec2 p, float t)
{
    vec3 p3 = vec3(p, t);
    p3  = fract(p3*0.1031);
    p3 += dot(p3, p3.zyx+31.32);
    return fract((p3.x+p3.y)*p3.z);
}

// Value Noise
/*
float noise(vec2 p, float t)
{
    vec2 f = smoothstep(0.0, 1.0, fract(p));
    p = floor(p);
    float t0 = hash(vec2(0.0, 0.0)+p, t);
    float t1 = hash(vec2(1.0, 0.0)+p, t);
    float t2 = hash(vec2(0.0, 1.0)+p, t);
    float t3 = hash(vec2(1.0, 1.0)+p, t);
    return mix(mix(t0, t1, f.x), mix(t2, t3, f.x), f.y);
}
*/

// manu210404's Improved Version
float noise(vec2 p, float t)
{
    vec4 b = vec4(floor(p), ceil(p));
    vec2 f = smoothstep(0.0, 1.0, fract(p));
    return mix(mix(hash(b.xy, t), hash(b.zy, t), f.x), mix(hash(b.xw, t), hash(b.zw, t), f.x), f.y);
}

// Number of FBM Octaves
#define num_octaves 16

// Fractal Brownian Motion Noise
vec3 fbm(vec2 pos, int octaves)
{
    float value = 0.0;
    float scale = 1.0;
    float atten = 0.5;
    float t = 0.0;
    for(int i = 0; i < octaves; i++)
    {
        t += atten;
        value += noise(pos*scale, float(i))*atten;
        scale *= 2.0;
        atten *= 0.5;
        pos = rotate(pos, 0.125*pi);
    }
    return vec3(value/t);
}