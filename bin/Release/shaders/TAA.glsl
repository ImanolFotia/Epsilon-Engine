#version 440 core

layout (binding = 0) uniform sampler2D sCurrentFrame;
layout (binding = 1) uniform sampler2D sLastFrame;
layout (binding = 2) uniform sampler2D sVelocityBuffer;
layout (binding = 2) uniform sampler2D sDepthBuffer;

layout(location = 0) out vec4 FragColor;

in vec2 TexCoords;

uniform vec2 iResolution;
uniform float lerpAmount;
uniform int clampingKernelSize;
uniform int TotalFrames;

vec3 Gamma(in vec3 img) {
    return pow(img, vec3(1.0/2.2));
}
   
vec3 Degamma(in vec3 img) {
    return pow(img, vec3(2.2));
}


vec3 RGBToYCoCg( vec3 RGB )
{
	float Y = dot(RGB, vec3(  1, 2,  1 )) * 0.25;
	float Co= dot(RGB, vec3(  2, 0, -2 )) * 0.25 + ( 0.5 * 256.0/255.0 );
	float Cg= dot(RGB, vec3( -1, 2, -1 )) * 0.25 + ( 0.5 * 256.0/255.0 );
	return vec3(Y, Co, Cg);
}

vec3 YCoCgToRGB( vec3 YCoCg )
{
	float Y= YCoCg.x;
	float Co= YCoCg.y - ( 0.5 * 256.0 / 255.0 );
	float Cg= YCoCg.z - ( 0.5 * 256.0 / 255.0 );
	float R= Y + Co-Cg;
	float G= Y + Cg;
	float B= Y - Co-Cg;
	return vec3(R,G,B);
}

void main()
{ 
    vec2 fragCoord = gl_FragCoord.xy;
    vec2 velocity = texture(sVelocityBuffer, TexCoords).zw;
    
    // get the neighborhood min / max from this frame's render
    vec3 center = (Degamma(texture(sCurrentFrame, TexCoords).rgb));
    vec3 minColor = center;
    vec3 maxColor = center;
    for (int iy = -clampingKernelSize; iy <= clampingKernelSize; ++iy)
    {
        for (int ix = -clampingKernelSize; ix <= clampingKernelSize; ++ix)
        {
            if (ix == 0 && iy == 0)
                continue;
           
            vec2 offsetUV = ((fragCoord + vec2(ix, iy)) / iResolution.xy);
            vec3 color = (Degamma(texture(sCurrentFrame, offsetUV).rgb));
            minColor = min(minColor, color);
            maxColor = max(maxColor, color);
        }
    }
    
    // get last frame's pixel and clamp it to the neighborhood of this frame
    vec3 old = (Degamma(texture(sLastFrame, TexCoords - velocity).rgb));    
    old = max(minColor, old);
    old = min(maxColor, old);
    
    // interpolate from the clamped old color to the new color.
    vec3 pixelColor = mix(old, center, lerpAmount);   
    
    FragColor = vec4((Gamma(pixelColor)), 1.0);

    FragColor.a = 1.0;
}