///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <pch.hpp>

#include <INIReader.h>
#include <ProgramData.h>

ProgramData::ProgramData()
{
    INIReader reader("cfg/EpsilonPrefs.ini");

    if (reader.ParseError() < 0)
    {
        std::cout << "Can't load configuration file\n";
        return;
    }

    WINDOW_WIDTH = (int)reader.GetReal("WINDOW", "Width", 640);
    WINDOW_HEIGHT = (int)reader.GetReal("WINDOW", "Height", 480);
    FULLSCREEN = (int)reader.GetReal("WINDOW", "FullScreen", 0);

    ANISOTROPY = (float)reader.GetReal("BASIC_GRAPHICS", "ANISOTROPIC_FILTERING", 0);
    MSAA_SAMPLES = (int)reader.GetReal("BASIC_GRAPHICS", "MSAA_SAMPLES", 0);
    MONITOR = (int)reader.GetReal("BASIC_GRAPHICS", "MONITOR", 0);
    VSYNC = (int)reader.GetReal("BASIC_GRAPHICS", "VSYNC", 0);
    BLOOM = (int)reader.GetReal("BASIC_GRAPHICS", "BLOOM", 0);
    LIGHTSHAFTS = (bool)reader.GetReal("BASIC_GRAPHICS", "LIGHTSHAFTS", false);
    COMPRESSED_TEXTURES = (bool)reader.GetReal("BASIC_GRAPHICS", "COMPRESSED_TEXTURES", false);

    MOTION_BLUR = (bool)reader.GetReal("ADVANCED_GRAPHICS", "MOTION_BLUR", false);
    MOTION_BLUR_STRENGTH = (float)reader.GetReal("ADVANCED_GRAPHICS", "MOTION_BLUR_STRENGTH", 1.0);
    PARALLAX_OCLUSSION_MAPPING = (bool)reader.GetReal("ADVANCED_GRAPHICS", "PARALLAX_OCLUSSION_MAPPING", false);
    BOKEH_DOF = (bool)reader.GetReal("ADVANCED_GRAPHICS", "BOKEH_DOF", false);
    CHROMATIC_ABERRATION = (bool)reader.GetReal("ADVANCED_GRAPHICS", "CHROMATIC_ABERRATION", true);
    FXAA = (bool)reader.GetReal("ADVANCED_GRAPHICS", "FXAA", false);
    SHADOWMAP_SIZE = (int)reader.GetReal("ADVANCED_GRAPHICS", "SHADOWMAP_SIZE", 1024);
    SCREEN_SPACE_REFLECTIONS = (bool)reader.GetReal("ADVANCED_GRAPHICS", "SCREEN_SPACE_REFLECTIONS", false);
    SSR_QUALITY = (int)reader.GetReal("ADVANCED_GRAPHICS", "SSR_QUALITY", 1);
    HBAO = (bool)reader.GetReal("ADVANCED_GRAPHICS", "HBAO", 1);
    HBAO_QUALITY = (int)reader.GetReal("ADVANCED_GRAPHICS", "HBAO_QUALITY", 1);

    AMBIENT_PROBE_RES = (int)reader.GetReal("ENGINE", "AMBIENT_PROBE_RES", 1);;
    CUBEMAP_RES = (int)reader.GetReal("ENGINE", "CUBEMAP_RES", 1);;
}
