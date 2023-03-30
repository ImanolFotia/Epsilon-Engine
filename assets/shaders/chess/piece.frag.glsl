#version 460
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) out vec4 outColor;

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec4 color;
layout (location = 4) flat in int piece;



layout(binding = 0) uniform UniformBufferObject {
    float iTime;
    vec2 iResolution;
    mat4 view;
    mat4 proj;
    vec4 lastMove;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout (set = 1, binding = 0) uniform sampler2D textures[];

vec3 Gamma(in vec3 img) {
    return pow(img, vec3(1.0/2.2));
}
   
vec3 Degamma(in vec3 img) {
    return pow(img, vec3(2.2));
}

const float piece_x = 6.0;
const float piece_y = 2.0;

const vec2 cWhiteKing = vec2(0,1/2);
const vec2 cWhiteQueen = vec2(1/piece_x,1);
const vec2 cWhiteBishop = vec2(2/piece_x,1);
const vec2 cWhiteKnight = vec2(3/piece_x,1);
const vec2 cWhiteRook = vec2(4/piece_x,1.0/2.0);
const vec2 cWhitePawn = vec2(5/piece_x,1);

const vec2 cBlackKing = vec2(0,1);
const vec2 cBlackQueen = vec2(1,1);
const vec2 cBlackBishop = vec2(2,1);
const vec2 cBlackKnight = vec2(3,1);
const vec2 cBlackRook = vec2(4,1);
const vec2 cBlackPawn = vec2(5,1);

const vec2 pieces[12] = {
	vec2(0.0,1.0/piece_y),
	vec2(1.0/piece_x,1.0/piece_y),
	vec2(2.0/piece_x,1.0/piece_y),
	vec2(3.0/piece_x,1.0/piece_y),
	vec2(4.0/piece_x,1.0/piece_y),
	vec2(5.0/piece_x,1.0/piece_y),
	
	vec2(0.0,0.0),
	vec2(1.0/piece_x,0.0),
	vec2(2.0/piece_x,0.0),
	vec2(3.0/piece_x,0.0),
	vec2(4.0/piece_x,0.0),
	vec2(5.0/piece_x,0.0),
};

void main() {
   
    vec2 correctedCoords = vec2(texCoords.y, -texCoords.x);
    correctedCoords /= vec2(piece_x, piece_y);
    correctedCoords += pieces[piece];
    
    
    vec4 texCol = textureLod(textures[0], correctedCoords, 0.0);
    
    if(texCol.a < 0.01) discard;
    

    outColor.a = texCol.a;
    outColor.rgb = (Gamma(texCol.rgb));
}
