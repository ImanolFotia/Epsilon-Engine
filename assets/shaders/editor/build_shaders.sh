#!/bin/sh

glslc -fshader-stage=vert forward.vert.glsl -DVERTEX_SHADER -o forward_vertex.spv
glslc -fshader-stage=frag forward.frag.glsl -DFRAGMENT_SHADER -o forward_frag.spv

glslc -fshader-stage=vert vertex.vert.glsl -DVERTEX_SHADER -o vertex.spv
glslc -fshader-stage=frag fragment.frag.glsl -DFRAGMENT_SHADER -o frag.spv

glslc -fshader-stage=vert sky.vert.glsl -DVERTEX_SHADER -o sky-vertex.spv
glslc -fshader-stage=frag sky.frag.glsl -DFRAGMENT_SHADER -o sky-fragment.spv

glslc -fshader-stage=vert grid.vert.glsl -DVERTEX_SHADER -o grid-vertex.spv
glslc -fshader-stage=frag grid.frag.glsl -DFRAGMENT_SHADER -o grid-fragment.spv

glslc -fshader-stage=vert grass.vert.glsl -DVERTEX_SHADER -o grass-vertex.spv
glslc -fshader-stage=frag grass.frag.glsl -DFRAGMENT_SHADER -o grass-fragment.spv

glslc -fshader-stage=comp dummyCompute.glsl -DCOMPUTE_SHADER -o dummyCompute.spv

glslc -fshader-stage=vert effect.vert.glsl -DVERTEX_SHADER -o effect_vert.spv
glslc -fshader-stage=frag TAA.glsl -DFRAGMENT_SHADER -o TAA.spv
glslc -fshader-stage=frag SSS.glsl -DFRAGMENT_SHADER -o SSS.spv

