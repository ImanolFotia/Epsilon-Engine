///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once

/** Datatypes */

typedef unsigned char   UCHAR;
typedef unsigned char   BYTE;
typedef int             INT;
typedef int             INTEGER;
typedef unsigned int    UINT;
typedef float           DECIMAL;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef void            EGUIAPI;


struct GUIEVENTS
{
    double MousePosition[2];
    bool RightClickWasPressed;
    bool LeftClickWasPressed;
};


