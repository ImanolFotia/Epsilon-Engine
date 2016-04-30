///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define     DEFAULT_FRAMEBUFFER                     0x00000000

/** Light types */

#define     LIGHT_DIRECTIONAL                       0x00000001
#define     LIGHT_DIRECTIONAL_SHADOWS               0x00000010
#define     LIGHT_POINT                             0x00000011
#define     LIGHT_POINT_SHADOWS                     0x00000100

/** Entity types */

#define     ENTITY_GRASS                            0x00000101
#define     ENTITY_LIGHT                            0x00000110
#define     ENTITY_LADDER                           0x00000111
#define     ENTITY_FOG                              0x00001000
#define     ENTITY_SUN                              0x00001001
#define     ENTITY_RAIN                             0x00001010
#define     ENTITY_WEAPON                           0x00001011

/** Engine States*/
#define     EPSILON_PAUSE                           0x10000000
#define     EPSILON_RUNNING                         0x10000001
#define     EPSILON_MENU                            0x10000010

/** Datatypes */

typedef unsigned char   UCHAR;
typedef unsigned char   BYTE;
typedef int             INT;
typedef int             INTEGER;
typedef unsigned int    UINT;
typedef float           DECIMAL;
typedef short           SHORT;
typedef unsigned short  USHORT;

/**Enums*/

enum MODEL_TYPE
{
    DYNAMIC_MODEL = 0,
    STATIC_MODEL
};

enum SHADER_TYPE
{
    FORWARD_RENDERING_SHADER = 0,
    LIGHTNING_PASS_SHADER,
    SSAO_SHADER,
    GAUSSIAN_BLUR_SHADER,
    ANIMATION_SKINING_SHADER,
    SHADOW_MAPPING_SHADER,
    ANIMATION_SKINING_SHADOW_MAPPING_SHADER,
    SUN_SHADER,
    SKYBOX_SHADER,
    GEOMETRY_PASS_SHADER
};


#endif /// TYPES_H_INCLUDED
