#pragma once

#include "wgpu_data.hpp"

namespace wgpu
{

    static void createQueues(WebGPUData *wgpu_data)
    {

        wgpu_data->queue = wgpuDeviceGetQueue(wgpu_data->device);
    }
}