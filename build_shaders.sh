#!/bin/sh

glslc -fshader-stage=vert ./assets/shaders/vertex.vert.glsl -o ./assets/shaders/vertex.spv
glslc -fshader-stage=frag ./assets/shaders/fragment.frag.glsl -o ./assets/shaders/fragment.spv

glslc -fshader-stage=vert ./assets/shaders/board.vert.glsl -o ./assets/shaders/board-vertex.spv
glslc -fshader-stage=frag ./assets/shaders/board.frag.glsl -o ./assets/shaders/board-fragment.spv
