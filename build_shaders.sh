#!/bin/bash

/home/solaire/Downloads/vulkansdk-linux-x86_64-1.3.211.0/1.3.211.0/x86_64/bin/glslc -fshader-stage=vert ./assets/shaders/vertex.glsl -o ./assets/shaders/vertex.spv
/home/solaire/Downloads/vulkansdk-linux-x86_64-1.3.211.0/1.3.211.0/x86_64/bin/glslc -fshader-stage=frag ./assets/shaders/fragment.glsl -o ./assets/shaders/fragment.spv