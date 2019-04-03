# Epsilon Engine
![Hex.pm](https://img.shields.io/hexpm/l/plug.svg?style=popout)
[![Build Status](https://travis-ci.com/ImanolFotia/Epsilon-Engine.svg?token=eqNdpsuxZdxtFxRycvU4&branch=master)](https://travis-ci.com/ImanolFotia/Epsilon-Engine)

Official repository for Epsilon Engine.
Epsilon Engine is a next-gen, multi-platform, general purpose game engine.

Written in modern C++ and GLSL 4.3+

Screenshots and more detailed information is available on [my Webpage](http://imanolfotia.com/epsilon_engine)

Installers and binaries will be available soon

## table of Contents

| Table of Contents | 
| :--- |
| [Dependencies](#dependencies) | 
| [Features](#features)| 
| [Software & Hardware Succesfully Tested](#software_and_hardware_succesfully_tested)| 
| [Notes](#notes)| 

<a name="dependencies"></a>
## Dependencies 

* Bullet Physics 2.82-r2704
* Zlib
* GLM 0.9.7.1 
* GLEW 2.0.0
* GLFW 3.2
* libinih
* Freetype 2
* Soft OpenAL 1.18.2

<a name="features"></a>
## Features 

* Bullet Physics integration
* Screen Space Ambient Oclussion
* Cook-Torrance specular lighting
* Oren-Nayar diffuse lighting
* Ambient and indirect lighting is encoded in 3rd order Spherical Harmonics
* Screen Space Realtime Reflections
* Octree space division for open areas
* Binary Space Partition Trees(BSP Trees) are used for indoor areas
* Bokeh Depth of Field(DoF) post-processing
* Nvidia's FXAA post-processing
* Point, Area and Ambient lights are supported
* Physical Based Rendering(PBR) workflow
* Skeletal Animation
* Volumetric Lighting
* Day & Night system
* GPU simulated particle systems
* Parallax Oclussion mapping
* Dynamic sky and weather using procedural generation and Fractal Brownian Motion
* Meta-surfaces rendering using signed distance fields
* Developer's tools
* Custom GUI
* Dynamic terrain generation

<a name="software_and_hardware_succesfully_tested"></a>
## Software & Hardware Succesfully Tested 
* OS: Windows 7 & 10
* CPU: Intel i7 4790K & Intel I7 6700HQ
* RAM: 12 & 24 GB
* GPU: Nvidia GTX 970 & 970M

<a name="notes"></a>
## Notes 

* At the moment the only IDE supported is Code::Blocks, the idea is to migrate to CMake and make it IDE agnostic
* It should compile almost unmodified under Linux
