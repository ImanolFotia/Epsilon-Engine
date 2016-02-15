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
    bool FULLSCREEN;
    float ANISOTROPY;
    int MSAA_SAMPLES;
    int MONITOR;
    int VSYNC;
    bool SSAO;
};
#endif




