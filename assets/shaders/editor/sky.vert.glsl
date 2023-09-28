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

void main() {
    gl_Position = RenderPassUBO.data.proj * mat4(mat3(RenderPassUBO.data.view)) * PushConstant.object.modelMatrix * vec4(inPosition, 1.0);
    vs_out.normal = inNormal;//normalize(mat3(PushConstant.object.modelMatrix) * inNormal);
    vs_out.position = vec3(PushConstant.object.modelMatrix * vec4(inPosition, 1.0));//normalize(mat3(PushConstant.object.modelMatrix) * inNormal);
}