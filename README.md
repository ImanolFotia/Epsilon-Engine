# Epsilon Engine
[![LICENSE.md](https://img.shields.io/badge/License-EULA-blue.svg)](LICENSE.md)
[![CMake](https://github.com/ImanolFotia/Epsilon-Engine/actions/workflows/cmake.yml/badge.svg)](https://github.com/ImanolFotia/Epsilon-Engine/actions/workflows/cmake.yml)
[![pipeline status](https://gitlab.com/Difima/Epsilon-Engine/badges/master/pipeline.svg)](https://gitlab.com/Difima/Epsilon-Engine/-/commits/master)
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


<a name="building"></a>
## Building

### Windows

* You'll need a working **C++ compiler**, either **MSVC** (Visual Studio) or **MSYS2 MinGW/Clang** will work.
* **LunarG's Vulkan SDK**, which you can download [here](https://vulkan.lunarg.com/sdk/home#windows) (be sure it's in the Path)
* **CMake GUI** (optional)

**Clone the repo**

```
git clone https://github.com/ImanolFotia/Epsilon-Engine.git
```
```
cd Epsilon-Engine
```
```
git submodule init
```
```
git submodule update --remote
```
#### Build
Now you can either configure your project using CMake or CMake-GUI if you are using MinGW, or open Visual Studio IDE and open the project as "CMake Project"  

`
Be careful, Visual Studio might decide that the primary project is 'EpsilonSharp.sln', this incorrect, as this is an Editor module, choose Epsilon.dll instead.
`

### Linux

**Clone the repo**

```
git clone --recursive https://github.com/ImanolFotia/Epsilon-Engine.git
```

```
cd Epsilon-Engine
```

#### Setting up the environment

Install system and third party dependencies

#### Debian/Ubuntu based:  

Pipewire ppa (optional):  

```
sudo add-apt-repository ppa:pipewire-debian/pipewire-upstream
```
```
sudo apt-get update
```
```
sudo apt install vulkan-tools libxkbcommon-dev libwayland-dev libvulkan-dev vulkan-validationlayers-dev spirv-tools libxcb-keysyms1-dev xorg-dev libpulse-dev libpulse-mainloop-glib0 libpipewire-0.3-dev xorg-x11-server-devel pipewire-devel
```

#### Fedora/RedHat (dnf)

```
sudo dnf install vulkan-tools libxcb-devel wayland-devel vulkan-validation-layers-devel spirv-tools-devel xcb-util-keysyms-devel xorg-x11-server-Xwayland-devel pulseaudio-libs-devel
```

### Build
```
cmake --no-warn-unused-cli -DCMAKE_BUILD_TYPE:STRING=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -DCMAKE_C_COMPILER:FILEPATH=/usr/bin/gcc -DCMAKE_CXX_COMPILER:FILEPATH=/usr/bin/g++ -S./ -B./build -G Ninja
```
```
cmake -B ./build -DCMAKE_BUILD_TYPE=Release -DIS_CI_ENV:BOOL=FALSE
```

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
