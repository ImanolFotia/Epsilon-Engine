#version 440 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

mat4 matmul = mat4(1, 1, 1, 1,
                   1, 1, 1, 1,
                   1, 1, 1, 1,
                   1, 1, 1, 1);

void main() {

    vec4 posScreen = MVP * inverse(model) * vec4(1.0);


    mat4 newView = view;
    //newView[3][0] = 0;
    //newView[3][1] = 0;
    //newView[2][2] = 0;
    vec4 pos = proj * newView * model * vec4(position, 1.0);
    gl_Position = pos;// * inverse(view);
    //gl_Position /= gl_Position.w;
}