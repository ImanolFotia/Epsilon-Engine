#version 460 core

#extension GL_EXT_nonuniform_qualifier : enable
#include "bindings.glsl"
#include "FBM.glsl"

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec4 inColor;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBitangent;

layout (location=10) out vec4 Position;
layout (location=11) out vec4 PrevPosition;
layout (location=12) out vec3 position_coord;


mat3 CreateTBNMatrix(mat3 normalMatrix)
{
     vec3 T = normalize(normalMatrix * inTangent);
     vec3 B = normalize(normalMatrix * inBitangent);
     vec3 N = normalize(normalMatrix * inNormal);

    return transpose(mat3(T, B, N));
}

mat3 CreateTBNMatrix(mat3 normalMatrix, vec3 normal, vec3 tangent, vec3 bitangent)
{
     vec3 T = normalize(normalMatrix * tangent);
     vec3 B = normalize(normalMatrix * bitangent);
     vec3 N = normalize(normalMatrix * normal);

    return transpose(mat3(T, B, N));
}

float hash13(vec3 p3)
{
	p3  = fract(p3 * .1031);
    p3 += dot(p3, p3.zyx + 31.32);
    return fract((p3.x + p3.y) * p3.z);
}
mat2 rot(float angle) {
  return mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
}

vec3 cubic_bezier(vec3 P0, vec3 P1, vec3 P2, vec3 P3, float t)
{
    return P0 * (pow(1.0 - t, 3.0)) +
           P1 * (3.0 * t * pow(1.0 - t, 2.0)) +
           P2 * (3.0 * pow(t, 2.0) * (1.0 - t)) +
           P3 * (pow(t, 3.0));
}


vec3 lerp(vec3 a, vec3 b, float t)
{
    return (1.0 - t) * a + t * b;
}
vec3 cuadratic_bezier(vec3 P0, vec3 P1, vec3 P2, float t)
{
   vec3 a_b = lerp(P0, P1, t);
    vec3 b_c = lerp(P1, P2, t);

    return lerp(a_b, b_c, t);
}



void main() {

  float iTime = RenderPassUBO.data.iTime;

    mat4 modelMatrix = TransformBuffer.entry[ObjectBuffer.entry[gl_InstanceIndex].transform_index];

    vec3 newPos = inPosition;
    position_coord = inPosition;
    vec3 instance_pos = vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
    
    vec3 clouds = fbm(instance_pos.xz*0.8 + iTime * 0.5, 2) * vec3(1.0) * 2.0 - 1.0;
    vec3 clouds2 = fbm(instance_pos.xz*0.1 - iTime*3.0, 2) * vec3(fract(gl_InstanceIndex/1000)) * 2.0 - 1.0;

    clouds2 = clamp(clouds2, -1.5, 1.5);

    vec4 pos = RenderPassUBO.data.proj * RenderPassUBO.data.view * modelMatrix * vec4(inPosition, 1.0);
    vec3 vPos = vec3(RenderPassUBO.data.view[3][0], RenderPassUBO.data.view[3][1], RenderPassUBO.data.view[3][2]);
    vec3 viewDir = normalize(pos.xyz - vPos);//vec3(RenderPassUBO.data.view[2][0], RenderPassUBO.data.view[2][1], RenderPassUBO.data.view[2][2]);

    vec3 normal = normalize(mat3(modelMatrix) * inNormal);

    float invCosTheta = 1.0-abs(dot(normal.xz, viewDir.xz));
    
    vec3 P0 = vec3(0.0, 0.0, 0.0);
    vec3 P1 = vec3(0.0, rot((sin(clouds.x)*0.5)) * vec2(0.5, 0.0));
    vec3 P2 = vec3(0.0, rot((sin(clouds.x+0.15)*1.5) + clouds2.x * 0.05) * vec2(1.0, 0.0));
    vec3 P3 = vec3(0.0, /*(cos(iTime)*0.5+0.5)*0.5*/1.0, 0.0);

    vec3 curve = cuadratic_bezier(P0, P1, P2, newPos.y);//bezier(P0, P1, P2, P3, 1.0-newPos.y);

    normal = inNormal;
    normal.yz = rot((sin(clouds.x+0.15)*1.5) + clouds2.x * 0.05)* normal.yz;//rot(clouds.x+hash13(instance_pos)* inPosition.y) * normal.yz;
    //normal.yz = curve.yz* normal.yz; //(rot(clouds2.x) ) * normal.yz;
    
    vec3 tangent = inTangent;
    tangent.yz = rot(clouds.x+hash13(instance_pos)* inPosition.y) * tangent.yz;
    tangent.yz = rot((sin(clouds.x+0.15)*1.5) + clouds2.x * 0.05)* tangent.yz;//curve.yz * tangent.yz;//(rot(clouds2.x) ) * tangent.yz;

    vec3 bitangent = normalize(cross(normal, tangent)); 


    newPos.yz = curve.yz;//(rot(/*clouds.x+hash13(instance_pos)* inPosition.y*/curve.y) ) * newPos.yz;
    //newPos.yz = (rot(clouds2.x) ) * newPos.yz;
    //newPos.xz += clouds2.xz;

    newPos.z += clamp((invCosTheta*newPos.x), 0.0, .5);

    vec4 worldPos = modelMatrix * vec4(newPos, 1.0);
    

    gl_Position = RenderPassUBO.data.proj * RenderPassUBO.data.view * worldPos;
    
  	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
    
    vs_out.TBN = CreateTBNMatrix(normalMatrix, normal, tangent, bitangent);
    vs_out.material_index = PushConstant.object.material_index;
    vs_out.color = inColor;
    vs_out.texCoords = inTexCoord;
    vs_out.position = vec3(modelMatrix * vec4(newPos, 1.0));
    vs_out.normal = normalize(mat3(modelMatrix) * normal);
    vs_out.object_id = gl_InstanceIndex;

    
    mat4 ViewMatrix = RenderPassUBO.data.view;
    mat4 Projection = RenderPassUBO.data.proj;

    vec4 viewPos = ViewMatrix * worldPos;

    Position = Projection * viewPos;
    PrevPosition = Projection * RenderPassUBO.data.prev_view * /*PrevModel*/ modelMatrix * vec4(newPos, 1.0);
}