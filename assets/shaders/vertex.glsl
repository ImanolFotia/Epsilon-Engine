#version 450

const vec2 iResolution = vec2(1280.0, 720.0);


layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout (location = 0) out vec3 color;

void main() {
    color = inColor.rgb;
    //vec2 outVert = positions[gl_VertexIndex];
    //outVert.y *= (iResolution.x / iResolution.y);

    gl_Position = vec4(inPosition, 1.0);
}