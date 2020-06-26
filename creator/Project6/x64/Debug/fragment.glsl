#version 330 core

out vec4 FragColor;

void main() {

    FragColor = vec4(vec3(dot(gl_FragCoord.xy, gl_FragCoord.yx)), 1.0)
}