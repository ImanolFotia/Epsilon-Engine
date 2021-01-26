#version 440 core

in vec4 FragPos;
uniform vec3 LightPosition;
uniform float Far;

void main() {
    vec3 LightPosition = LightPosition;
    gl_FragDepth = length(FragPos.xyz - LightPosition) / Far;
}