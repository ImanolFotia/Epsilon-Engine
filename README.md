# Epsilon Engine
[![LICENSE.md](https://img.shields.io/badge/License-EULA-blue.svg)](LICENSE.md)
[![CMake](https://github.com/ImanolFotia/Epsilon-Engine/actions/workflows/cmake.yml/badge.svg)](https://github.com/ImanolFotia/Epsilon-Engine/actions/workflows/cmake.yml)
[![GitLab](https://gitlab.example.com/<namespace>/<project>/badges/<branch>/pipeline.svg)](https://gitlab.com/Difima/Epsilon-Engine/-/jobs)
![](https://img.shields.io/badge/platforms-Windows%20%7C%20Linux-orange)

Official repository for Epsilon Engine.
Epsilon Engine is a next-gen, multi-platform, general purpose game engine.

Epsilon Engine© is a multi-platform, next-gen, on-development engine especifically designed to build high quality games and simulations. It uses Vulkan (previously OpenGL© 4.5) capabilities to deliver stunning graphics with great real time performance. It has been under development since 2015, and has constantly been catching up with the latest realtime, 3D techniques.

Screenshots and more detailed information is available on [my Webpage](http://imanolfotia.com/epsilon_engine)

Installers and binaries will be available soon

![](https://raw.githubusercontent.com/ImanolFotia/Epsilon-Engine/master/media/Epsilon%20LOGO3.png)

## Table of Contents

| Table of Contents | 
| :--- |
| [Dependencies](#dependencies) | 
| [Features](#features)| 
| [Software & Hardware Succesfully Tested](#software_and_hardware_succesfully_tested)| 
| [Contributors](#contributors)|  
| [Notes](#notes)| 
| [Documentation](#documentation)| 
| [License](#license)| 

<a name="dependencies"> </a>
## Dependencies 

* GLM 
* GLFW 
* Soft OpenAL
* VMA
* Beacon
* nlohmann/json


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
* TAA
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
* OS: 
  * Windows 7 
  * Windows 10 
  * Ubuntu 18.0 LTS
* CPU: 
  * Intel i7 4790K 
  * Intel I7 6700HQ
* GPU: 
  * Nvidia GTX 970 
  * Nvidia GTX 970M
  * Nvidia RTX 2080 Ti

<a name="contributors"></a>
## Contributors 

**[Imanol Fotia](https://github.com/ImanolFotia)** - http://imanolfotia.com

<a name="documentation"></a>
## Documentation
A Doxygen auto generated documentation is available [here](http://imanolfotia.com/epsilon-documentation/html)

<a name="license"></a>
## License

Check out [LICENSE](LICENSE.md)
