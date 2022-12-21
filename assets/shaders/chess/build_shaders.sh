#!/bin/sh

glslc -fshader-stage=vert piece.vert.glsl -o piece-vertex.spv
glslc -fshader-stage=frag piece.frag.glsl -o piece-fragment.spv

glslc -fshader-stage=vert board.vert.glsl -o board-vertex.spv
glslc -fshader-stage=frag board.frag.glsl -o board-fragment.spv
