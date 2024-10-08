#version 460 core
#extension GL_EXT_nonuniform_qualifier : enable
#extension GL_GOOGLE_include_directive : enable
#include "bindings.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout(location = 9) out vec3 nearPoint;
layout(location = 10) out vec3 farPoint;

layout (location=11) out vec4 Position;
layout (location=12) out vec4 PrevPosition;

vec3 unprojectPoint(float x, float y, float z, mat4 viewProjectionInverse) {
    vec4 clipSpacePos = vec4(x, y, z, 1.0);
    vec4 eyeSpacePos = viewProjectionInverse * clipSpacePos;
    return eyeSpacePos.xyz / eyeSpacePos.w;
}


void main() {
    
    vec3 pos = inPosition.xyz;
    mat4 viewProjectionInverse = inverse(RenderPassUBO.data.proj * RenderPassUBO.data.view);

    nearPoint = unprojectPoint(pos.x, pos.y, 0.0, viewProjectionInverse);
    farPoint = unprojectPoint(pos.x, pos.y, 1.0, viewProjectionInverse);


    gl_Position = vec4(pos, 1.0);
    
    vec4 worldPos = PushConstant.object.modelMatrix * vec4(inPosition, 1.0);
    
    mat4 ViewMatrix = RenderPassUBO.data.view;
    mat4 Projection = RenderPassUBO.data.proj;

    vec4 viewPos = ViewMatrix * worldPos;

    Position = Projection * viewPos;
    PrevPosition = Projection * RenderPassUBO.data.prev_view * /*PrevModel*/ PushConstant.object.modelMatrix * vec4(inPosition, 1.0);

}