#version 430 core

out vec4 FragColor;
out vec4 BrightFragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    noperspective mat3 TBN;
    mat3 TBN2;
    vec3 wFragPos;
} fs_in;

layout (binding = 0) uniform sampler2D blurredTexture;

uniform float roughness;

//Inputs from previous shader stage, mostly used for motion blur
in vec4 PrevPosition;
in vec4 Position;
in flat int varCurrEnt;

uniform int resX;
uniform int resY;

void main() {
    const int MAX_MIPS = 4;

    vec2 iResolution = vec2(resX, resY);
    vec2 ndc = gl_FragCoord.xy/iResolution;
    vec2 uv = ndc*iResolution + 0.5;
    vec2 iuv = floor( uv );
    vec2 fuv = fract( uv );
    uv = iuv + fuv*fuv*(3.0-2.0*fuv); // fuv*fuv*fuv*(fuv*(fuv*6.0-15.0)+10.0);;
    uv = (uv - 0.5)/iResolution;

    vec2 coords = vec2(gl_FragCoord.x / iResolution.x, gl_FragCoord.y/iResolution.y);

    FragColor = vec4(texture(blurredTexture, coords, roughness * MAX_MIPS).rgb, 1.0);
    BrightFragColor = vec4(vec3(0.0), 1.0);
}