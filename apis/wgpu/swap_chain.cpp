#pragma once

#include "wgpu_data.hpp"

namespace wgpu
{
    static void createswapChain(WebGPUData *wgpu_data)
    {

        WGPUSurfaceDescriptorFromCanvasHTMLSelector canvDesc = {};
        canvDesc.chain.sType = WGPUSType_SurfaceDescriptorFromCanvasHTMLSelector;
        canvDesc.selector = "canvas";

        WGPUSurfaceDescriptor surfDesc = {};
        surfDesc.nextInChain = reinterpret_cast<WGPUChainedStruct *>(&canvDesc);

        WGPUSurface surface = wgpuInstanceCreateSurface(nullptr, &surfDesc);

        WGPUSwapChainDescriptor swapDesc = {};
        swapDesc.usage = WGPUTextureUsage_RenderAttachment;
        swapDesc.format = WGPUTextureFormat_BGRA8Unorm;
        swapDesc.width = 800;
        swapDesc.height = 450;
        swapDesc.presentMode = WGPUPresentMode_Fifo;

        wgpu_data->swapchain = wgpuDeviceCreateSwapChain(device, surface, &swapDesc);
    }
}