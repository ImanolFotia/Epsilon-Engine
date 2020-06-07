#pragma once

#define DEBUG 1

#ifndef GL_SHADER_STORAGE_BUFFER
#define GL_SHADER_STORAGE_BUFFER 0x90D2 // According to https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_shader_storage_buffer_object.txt
#endif

#ifndef GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#endif
#ifndef GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
typedef unsigned long DWORD, *PDWORD, *LPDWORD; //Defining because windows.h has defines that interfere with some declarations

//just in case, we don't care about ancient code :)
//Windows x64 uses a flat memory model anyway
#undef near
#undef far
#define NOMINNMAX //exclude these terrible names
#endif

#ifdef __linux__
#endif

#ifdef DEBUG

#else

#endif
