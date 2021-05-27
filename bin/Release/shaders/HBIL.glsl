

#version 440 core

/*  Horizon-Based Indirect Lighting */
/*  Implemented from the paper of the same name by Benoit "Patapom" Mayaux - March 2018
    b.mayaux@gmail.com
*/
#define Scale vec3(.8, .8, .8)
#define K 19.19

layout (binding = 0) uniform sampler2D gDepth;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D texNoise;
layout (binding = 3) uniform sampler2D gAlbedo;

layout(location = 0) out vec3 FragColor;

const float PI = 3.14159;
const int NumDirections = 5;
const float NEAR = 0.1;
const float FAR = 3000.0;
in vec2 ViewRay;

in vec2 TexCoords;

uniform mat4 ViewMatrix;
uniform mat4 invprojection;
uniform mat4 invView;

uniform vec2 FocalLen;
uniform vec2 UVToViewA;
uniform vec2 UVToViewB;
uniform vec2 Resolution;
uniform vec2 LinMAD;
uniform float iTime;

uvec2 murmurHash23(uvec3 src) {
    const uint M = 0x5bd1e995u;
    uvec2 h = uvec2(1190494759u, 2147483647u);
    src *= M; src ^= src>>24u; src *= M;
    h *= M; h ^= src.x; h *= M; h ^= src.y; h *= M; h ^= src.z;
    h ^= h>>13u; h *= M; h ^= h>>15u;
    return h;
}

vec2 hash23(vec3 src) {
    uvec2 h = murmurHash23(floatBitsToUint(src));
    return uintBitsToFloat(h & 0x007fffffu | 0x3f800000u) - 1.0;
}


float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC
    return ((2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR)));
}

float ViewSpaceZFromDepth(float d)
{
	// [0,1] -> [-1,1] clip space
	d = d * 2.0 - 1.0;

	// Get view space Z
	return -1.0 / (LinMAD.x * d + LinMAD.y);
}

vec3 UVToViewSpace(vec2 uv, float z)
{
	uv = UVToViewA * uv + UVToViewB;
	return vec3(uv * z, z);
}

vec3 ViewPosition() {
    return vec3(ViewMatrix[3][0], ViewMatrix[3][1], ViewMatrix[3][2]);
}

vec3 ViewDirection() {
    return vec3(ViewMatrix[2][0], ViewMatrix[2][1], ViewMatrix[2][2]);
}

vec3 ViewRight() {
    return vec3(ViewMatrix[0][0], ViewMatrix[0][1], ViewMatrix[0][2]);
}

vec3 ViewUp() {
    return vec3(ViewMatrix[1][0], ViewMatrix[1][1], ViewMatrix[1][2]);
}

vec3 GetViewPos(vec2 uv)
{
	float z = ViewSpaceZFromDepth(textureLod(gDepth, uv, 0.0).r);
	return UVToViewSpace(uv, z);
}

float Length2(vec3 V)
{
	return dot(V,V);
}

vec3 MinDiff(vec3 P, vec3 Pr, vec3 Pl)
{
    vec3 V1 = Pr - P;
    vec3 V2 = P - Pl;
    return (Length2(V1) < Length2(V2)) ? V1 : V2;
}


vec3 PositionFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = invprojection * clipSpacePosition;

    viewSpacePosition /= viewSpacePosition.w;

    return viewSpacePosition.xyz;
}

float lenSq(vec3 vec) {
    float l = length(vec);
    return l*l;
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

vec3 WorldPosFromDepth(float depth) {
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(TexCoords * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = inverse(invprojection) * clipSpacePosition;

    // Perspective division
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = ViewMatrix * viewSpacePosition;

    return worldSpacePosition.xyz;
}
void main() {

    const int numSamples = 8;

    vec3 color = vec3(0.0);


    vec2 texelSize = 1.0 / vec2(textureSize(gAlbedo, 0));
    vec3 position = WorldPosFromDepth(textureLod(gDepth, TexCoords, 0).r);
    vec3 normal = normalize(texture(gNormal, TexCoords.xy)).rgb;
    normal = normalize(normal);


    for(int i = 0; i < numSamples; i++) {
        vec2 offset = (vec2(i / numSamples, -i / numSamples) + 1.0) ;
        //vec3 random = normalize(textureLod(texNoise, (TexCoords.xy), 0.0).xyz - 1.0)* vec3(texelSize, 0.0) * 3.0/*+ vec3(offset, 1.0)*/;
        vec2 random = (hash23(position * (i+1) + iTime) * 2.0 - 1.0) * texelSize * 100.0;   

        vec3 rpos = WorldPosFromDepth(textureLod(gDepth, TexCoords + random.xy, 0.0).r);
        vec3 rnor = normalize(texture(gNormal, TexCoords.xy + random.xy)).rgb;
        //vec3 rnor = normalize(vec3(vec4(wrnor, 1.0) * invView));
        vec3 rcol = pow(texture(gAlbedo, TexCoords.xy + random.xy).rgb, vec3(2.2));

        vec3 path = rpos - position;
        vec3 dir = normalize(path);

        float emit = clamp((dot(dir, -rnor)), 0.0, 1.0); 
        float rec = clamp(dot(dir, normal) * 0.5 + 0.5, 0.0, 1.0);
        float distfall = 1.0 / (2.0 * (0.001+lenSq(path)));
        color += rcol * rec * distfall;
    }

    color /= numSamples;
    //color = random;

/*
	vec3 P, Pr, Pl, Pt, Pb;
	P 	= GetViewPos(TexCoords);

    vec2 AORes = Resolution;
    vec2 InvAORes = 1.0/AORes;

	// Sample neighboring pixels
    Pr 	= GetViewPos(TexCoords + vec2( InvAORes.x, 0));
    Pl 	= GetViewPos(TexCoords + vec2(-InvAORes.x, 0));
    Pt 	= GetViewPos(TexCoords + vec2( 0, InvAORes.y));
    Pb 	= GetViewPos(TexCoords + vec2( 0,-InvAORes.y));

    // Calculate tangent basis vectors using the minimu difference
    vec3 dPdu = normalize(MinDiff(P, Pr, Pl));
    vec3 dPdv = normalize(MinDiff(P, Pt, Pb) * (AORes.y * InvAORes.x));


    mat4 invMatrix = transpose(inverse(ViewMatrix));

    vec3 N = (texture(gNormal, TexCoords) * inverse(ViewMatrix)).rgb;
    
    // Create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(dPdu - N * dot(dPdu, N));
    vec3 bitangent = normalize(cross(N, tangent));

	//P = (vec4(GetViewPos(TexCoords), 1.0) * invMatrix).rgb;

    vec3 U = (vec4(ViewUp(), 1.0) * ViewMatrix).rgb;

    vec3 WP = (vec4(ViewPosition(), 1.0) * ViewMatrix).rgb;

    vec3 W0 = normalize(WP - P);

    vec3 Wx = normalize(cross(U, W0));
    vec3 Wy = normalize(cross(W0, Wx));

    float alpha = 2.0 * PI / NumDirections;

    //for(int d = 0; d < NumDirections; ++d) {
        float phi = alpha * 1.0;
        vec2 D = vec2(cos(phi), + sin(phi));
    //}
    //N = normalize(cross(dPdu, dPdv));
    FragColor = vec3(D.x * dPdu + D.y * dPdv);*/

    FragColor = pow(color, 1.0 / vec3(2.2)) * 0.05;
}