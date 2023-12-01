#!/bin/sh

glslc -fshader-stage=vert vertex.vert.glsl -o vertex.spv
glslc -fshader-stage=frag fragment.frag.glsl -o fragment.spv

glslc -fshader-stage=vert shadow-vertex.glsl -o shadow-vertex.spv
glslc -fshader-stage=frag shadow-fragment.glsl -o shadow-fragment.spv


glslc -fshader-stage=vert sky.vert.glsl -o sky-vertex.spv
glslc -fshader-stage=frag sky.frag.glsl -o sky-fragment.spv


glslc -fshader-stage=vert grass.vert.glsl -o grass-vertex.spv
glslc -fshader-stage=frag grass.frag.glsl -o grass-fragment.spv
