#version 440 core

/*  Horizon-Based Indirect Lighting */
/*  Implemented from the paper of the same name by Benoit "Patapom" Mayaux - March 2018
    b.mayaux@gmail.com
*/

layout (binding = 0) uniform sampler2D gDepth;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D texNoise;

layout(location = 0) out vec3 FragColor;

const float PI = 3.14159;
const int NumDirections = 5;
const float NEAR = 0.1;
const float FAR = 3000.0;
in vec2 ViewRay;

in vec2 TexCoords;

uniform mat4 ViewMatrix;

uniform vec2 FocalLen;
uniform vec2 UVToViewA;
uniform vec2 UVToViewB;
uniform vec2 Resolution;
uniform vec2 LinMAD;

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
void main() {



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
    FragColor = vec3(D.x * dPdu + D.y * dPdv);
}