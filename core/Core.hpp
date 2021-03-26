#pragma once

//#define DEBUG 1

//#define EPSILON_STATIC
#ifndef GLAD_INCLUDED
#define GLAD_INCLUDED
#include <glad/glad.h>
#endif

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
#define WGL_WGLEXT_PROTOTYPES
#include <windows.h>
//#include <GL/wgl.h>
#include <GL/wglext.h>
typedef unsigned long DWORD, *PDWORD, *LPDWORD; //Defining because windows.h has defines that interfere with some declarations

//just in case, we don't care about ancient code :)
//Windows x64 uses a flat memory model anyway
#undef near
#undef far
#undef NEAR
#undef FAR
#define NOMINNMAX //exclude these terrible names

#endif

#undef near
#undef far
#undef NEAR
#undef FAR
#define NOMINNMAX //exclude these terrible names(twice)

#ifdef __linux__
#endif

#if defined _WIN32 || defined __CYGWIN__ || __MINGW32__
  #ifdef BUILDING_DLL
    #if defined __GNUC__ || defined __MINGW32__
      #define EPSILON_API __attribute__ ((dllexport))
    #else
      #define EPSILON_API __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #if defined __GNUC__ || defined __MINGW32__
      #define EPSILON_API __attribute__ ((dllimport))
    #else
      #define EPSILON_API __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define EPSILON_LOCAL
#else
  #if __GNUC__ >= 4
    #define EPSILON_API __attribute__ ((visibility ("default")))
    #define EPSILON_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define EPSILON_API
    #define EPSILON_LOCAL
  #endif
#endif

#ifdef DEBUG

#else

#define COMPILE_FOR_GL

#endif
