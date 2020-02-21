#version 330 core
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texcoords;
layout (location=3) in vec4 Weights;
layout (location=4) in ivec4 VertexIDs;
uniform float time;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 Bones[250];
out vec3 Normal;
out vec3 FragPos;
out vec2 texCoords;
void main() {
    mat4 BoneTransform = Bones[VertexIDs[0]] * Weights[0];
    BoneTransform += Bones[VertexIDs[1]] * Weights[1];
    BoneTransform += Bones[VertexIDs[2]] * Weights[2];
    BoneTransform += Bones[VertexIDs[3]] * Weights[3];
    vec4 vertexTransform = ( BoneTransform * vec4(position, 1.0));
    gl_Position = proj * view * model * vertexTransform;
    Normal = normalize(mat3(transpose(inverse(view * model))) * normal);
    FragPos = vec4(model * vec4(position, 1.0)).xyz;
    texCoords = texcoords;
}