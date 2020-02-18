#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 UV;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
out vec3 FragPos;
out vec3 Normal;

void main()
{   
    mat4 cView = mat4(mat3(view));
    vec4 pos = projection * cView * model * vec4(position, 1.0);
    FragPos = vec3(model * vec4(position, 1.0));
    gl_Position = projection * cView * model * vec4(position, 1.0);
    Normal = normalize(normal);
}
