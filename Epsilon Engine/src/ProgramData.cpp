///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#include <iostream>

#include <INIReader.h>
#include <ProgramData.h>

ProgramData::ProgramData()
{
    INIReader reader( "cfg/EpsilonPrefs.ini" );

    if ( reader.ParseError() < 0 )
    {
        std::cout << "Can't load configuration file\n";
        return;
    }

    WINDOW_WIDTH    = (int)reader.GetReal   (     "WINDOW"    ,     "Width"        ,       640     );
    WINDOW_HEIGHT   = (int)reader.GetReal   (     "WINDOW"    ,     "Height"       ,       480     );
    FULLSCREEN      = (bool)reader.GetReal  (     "WINDOW"    ,     "FullScreen"   ,       0       );
    ANISOTROPY      = (float)reader.GetReal (     "VISUAL"    ,     "ANISOTROPY"   ,       0       );
    MSAA_SAMPLES    = (int)reader.GetReal   (     "VISUAL"    ,     "MSAA_SAMPLES" ,       0       );
    MONITOR         = (int)reader.GetReal   (     "VISUAL"    ,     "MONITOR"      ,       0       );
    VSYNC           = (int)reader.GetReal   (     "VISUAL"    ,     "VSYNC"        ,       0       );
    SSAO            = (bool)reader.GetReal  (     "VISUAL"    ,     "SSAO"         ,       0       );
}
