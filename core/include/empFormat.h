#pragma once
// Format identifier must always be 0x53454D4C || SEMP
#define EMPHEADER (('P'<<24)+('M'<<16)+('E'<<8)+'S')

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

const int maxLumps = 3;
const float empVersion = 1.0f;

enum empLumps
{
    lBSPMap = 0,
    lLights,
    lEntities,
    lSounds,
    lPlayerStart,
    lParticles,
    lTerrains,
    lSky,
    lSun,
    lLightProbes,
    lWaters
};

struct t_Lumps
{
    int offset;
    int size;
};

struct t_emp_header
{
    int format;
    float version;
    int numLumps;
    int numLights;
    int numEntities;
    int numSounds;
    int numParticles;
    int numterrains;
    int numLightProbes;
    int numWaters;
};

struct t_vector
{
    float x;
    float y;
    float z;
};

struct t_BSPmap
{
    char path[50];
    float scale;
    t_vector position;
    t_vector rotation;
};

struct t_light
{
    t_vector position;
    t_vector direction;
    int type;
    float radius;
    float intensity;
};

struct t_entitie
{

};

struct t_sound
{

};

struct t_particle
{

};

struct t_terrain
{

};

struct t_lightprobe
{
    t_vector u_L00;
    t_vector u_L1m1;
    t_vector u_L10;
    t_vector u_L11;
    t_vector u_L2m2;
    t_vector u_L2m1;
    t_vector u_L20;
    t_vector u_L21;
    t_vector u_L22;
};

struct t_water
{

};
