#!/bin/sh

glslc -fshader-stage=vert forward.vert.glsl -DVERTEX_SHADER -o forward_vertex.spv
glslc -fshader-stage=frag forward.frag.glsl -DFRAGMENT_SHADER -o forward_frag.spv

glslc -fshader-stage=vert vertex.vert.glsl -DVERTEX_SHADER -o vertex.spv
glslc -fshader-stage=frag fragment.frag.glsl -DFRAGMENT_SHADER -o frag.spv

glslc -fshader-stage=vert sky.vert.glsl -DVERTEX_SHADER -o sky-vertex.spv
glslc -fshader-stage=frag sky.frag.glsl -DFRAGMENT_SHADER -o sky-fragment.spv

