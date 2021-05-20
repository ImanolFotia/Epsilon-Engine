///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
/*
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>*/
#include <memory>
#include <glm/glm.hpp>

namespace Epsilon
{
///64 BSP units equals 1 meter
///3.2 OpenGL units equals 1 meter
#define GOLDEN_RATIO 3.2f
#define GRAVITY -9.81f * GOLDEN_RATIO

#define DEFAULT_FRAMEBUFFER 0x00000000

/** Engine States*/
#define EPSILON_PAUSE 0x10000000
#define EPSILON_RUNNING 0x10000001
#define EPSILON_MENU 0x10000010
#define GL_TEXTURE_COMPRESSION_S3TC_DXT5_SRGB 0x8C4F
    /** Datatypes */

    typedef unsigned char UCHAR;
    typedef unsigned char BYTE;
    typedef int INT;
    typedef int INTEGER;
    typedef unsigned int UINT;
    typedef short SHORT;
    typedef unsigned short USHORT;

    constexpr double PI = 3.14159265359;
    constexpr double HPI = 3.14159265359 * 0.5;
    constexpr double TAU = PI * 0.5;

    struct MINMAX_POINTS
    {
        float MAX_X;
        float MAX_Y;
        float MAX_Z;

        float MIN_X;
        float MIN_Y;
        float MIN_Z;
    };

    struct MIN_MAX_POINTS
    {
        float MAX_X = 0.0f;
        float MAX_Y = 0.0f;
        float MAX_Z = 0.0f;

        float MIN_X = 0.0f;
        float MIN_Y = 0.0f;
        float MIN_Z = 0.0f;
    };


    enum PARTICLE_PROXY
    {
        RAIN = 0,
        MIST,
        SNOW,
        SMOKE
    };

    enum GUIType
    {
        /* Button = 0,
    Panel,
    Label,
    CheckBox*/
    };

    /**Enums*/

    enum MODEL_TYPE
    {
        DYNAMIC_MODEL = 0,
        STATIC_MODEL
    };

    enum CAMERA_MODE
    {
        PLAYER_CONTROLLED = 0,
        NO_CLIP,
        CAMERA_FIXED,
        CAMERA_OVERRIDE
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

    enum UNIFORM_TYPE
    {
        U_INT = 0,
        U_FLOAT,
        U_VEC2,
        U_VEC3,
        U_VEC4,
        U_MAT3,
        U_MAT4
    };

    enum AUDIO_TYPE
    {
        AMBIENT_SOUND = 0,
        STATIC_SOUND,
        DYNAMIC_SOUND,
        MUSIC,
        MENU_SOUND
    };

    enum AUDIO_STATE
    {
        PLAYING = 0,
        PAUSED,
        STOPPED
    };

    struct t_light
    {
        glm::vec4 position;  // 4/*
        glm::vec4 direction; // 8
        glm::vec4 color;     // 12
        float radius;
        float _padding[3]; // 13
        float watts;
        float __padding[3]; // 14
        int type;
        float ___padding[3]; //15*/
    };

    enum CUBEMAP_TYPE
    {
        STATIC = 0,
        DYNAMIC
    };

    namespace Component
    {
        enum COMPONENT_TYPE
        {
            RENDERCOMPONENT = 0,
            SPATIALCOMPONENT,
            PLAYERCOMPONENT,
            SOUNDCOMPONENT,
            PHYSICCOMPONENT,
            CLOTHCOMPONENT,
            MOVEMENTCOMPONENT,
            SCRIPTCOMPONENT,
            TRANSFORMCOMPONENT,
            ENTITY
        };

        enum MATERIAL
        {
            DIRT = 0,
            GRAVE,
            STONE,
            METAL,
            MUD,
            WATER,
            WOOD,
            CONCRETE,
            GRASS,
            OREO
        };
    } // namespace Component

    struct GUIEVENTS
    {
        double MousePosition[2];
        bool KeyPressed[256];
        bool RightClickWasPressed;
        bool LeftClickWasPressed;
    };

} // namespace Epsilon
