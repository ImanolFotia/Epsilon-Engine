#!/bin/bash

~/VulkanSDK/x86_64/bin/glslc -fshader-stage=vert ./assets/shaders/vertex.glsl -o ./assets/shaders/vertex.spv
~/VulkanSDK/x86_64/bin/glslc -fshader-stage=frag ./assets/shaders/fragment.glsl -o ./assets/shaders/fragment.spv