#version 460 core

#extension GL_EXT_nonuniform_qualifier : enable
#include "bindings.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;


mat3 CreateTBNMatrix(mat3 normalMatrix)
{
     vec3 T = normalize(normalMatrix * inTangent);
     vec3 B = normalize(normalMatrix * inBitangent);
     vec3 N = normalize(normalMatrix * inNormal);

    return transpose(mat3(T, B, N));
}

void main() {
    gl_Position = RenderPassUBO.data.proj * RenderPassUBO.data.view * PushConstant.object.modelMatrix * vec4(inPosition, 1.0);
    
  	mat3 normalMatrix = transpose(inverse(mat3(PushConstant.object.modelMatrix)));
    
    vs_out.TBN = CreateTBNMatrix(normalMatrix);
    vs_out.normal = normalize(mat3(PushConstant.object.modelMatrix) * inNormal);
    vs_out.material_index = PushConstant.object.material_index;
    vs_out.color = inColor;
    vs_out.texCoords = inTexCoord;
    vs_out.position = vec3(PushConstant.object.modelMatrix * vec4(inPosition, 1.0));
}