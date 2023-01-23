#!/bin/sh

glslc -fshader-stage=vert vertex.vert.glsl -o vertex.spv
glslc -fshader-stage=frag fragment.frag.glsl -o fragment.spv
