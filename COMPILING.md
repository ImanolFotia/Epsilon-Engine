# Compiling

Epsilon uses CMake as it's build toolchain, it will download most of the required libraries, some however will need to be insatlled manually. 

## Linux

### Xorg

#### Ubuntu based (DEB)

Dev libraries are required by GLFW3

```
sudo apt install xorg-dev libxcb-keysyms1-dev
```


#### Fedora (RPM)
```
sudo dnf install libXcursor-devel libXi-devel libXinerama-devel libXrandr-devel xcb-util-keysyms-devel
```

### Wayland

Even if you are using Xorg, wayland developer packages are still needed to build Vulkan

#### Ubuntu (DEB)

```
sudo apt install libwayland-dev
```

#### Fedora

```
sudo dnf install wayland-devel wayland-protocols-devel
```

### Vulkan
Download and install the VulkanSDK from the [LunarG website](https://vulkan.lunarg.com/sdk/home)

You may also need these libraries:

```
sudo apt install vulkan-tools libvulkan-dev vulkan-validationlayers-dev spirv-tools libxcb-keysyms1-dev ninja-build
```


#### Fedora (RPM)
```
sudo dnf install vulkan-tools vulkan-loader-devel vulkan-validationlayers-dev spirv-tools xcb-util-keysyms-devel ninja-build
```


## Windows


### Vulkan
Download and install the VulkanSDK from the [LunarG website](https://vulkan.lunarg.com/sdk/home)

