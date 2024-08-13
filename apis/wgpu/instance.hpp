#pragma once

#include "wgpu_data.hpp"

namespace wgpu
{
    static void createInstance(WebGPUData *wgpu_data)
    {
        // 1. We create a descriptor
        WGPUInstanceDescriptor desc = {};
        desc.nextInChain = nullptr;

        // 2. We create the instance using this descriptor
        wgpu_data->instance = wgpuCreateInstance(&desc);
        // 3. We can check whether there is actually an instance created
        if (!wgpu_data->instance)
        {
            std::cerr << "Could not initialize WebGPU!" << std::endl;
            return 1;
        }

        // 4. Display the object (WGPUInstance is a simple pointer, it may be
        // copied around without worrying about its size).
        std::cout << "WGPU instance: " << wgpu_data->instance << std::endl;
    }
}