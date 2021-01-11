///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef PROGRAMDATA_H_INCLUDED
#define PROGRAMDATA_H_INCLUDED

class ProgramData
{

public:
    ProgramData();
    virtual ~ProgramData(){}

public:
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    int FULLSCREEN;
    float ANISOTROPY;
    int MSAA_SAMPLES;
    int MONITOR;
    int VSYNC;
    int SHADOWMAP_SIZE;
    bool BLOOM;
    bool SCREEN_SPACE_REFLECTIONS;
    int SSR_QUALITY;
    bool LIGHTSHAFTS;
    bool COMPRESSED_TEXTURES;
    bool MOTION_BLUR;
    float MOTION_BLUR_STRENGTH;
    bool PARALLAX_OCLUSSION_MAPPING;
    bool BOKEH_DOF;
    bool CHROMATIC_ABERRATION;
    bool FXAA;
    bool HBAO;
    int HBAO_QUALITY;
    int AMBIENT_PROBE_RES;
    int CUBEMAP_RES;

};
#endif




