#!/bin/bash

glslc -fshader-stage=vert ./assets/shaders/vertex.glsl -o ./assets/shaders/vertex.spv
glslc -fshader-stage=frag ./assets/shaders/fragment.glsl -o ./assets/shaders/fragment.spv
