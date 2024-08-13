#pragma once

#include "wgpu_data.hpp"

namespace wgpu
{
    static WGPUShaderModule createShaderModule(const std::vector<char> &code, const WebGPUData &wgpu_data)
    {

        WGPUShaderModuleSPIRVDescriptor spirv = {};
        spirv.chain.sType = WGPUSType_ShaderModuleSPIRVDescriptor;
        spirv.codeSize = code.size();
        spirv.code = code.data();
        WGPUShaderModuleDescriptor desc = {};
        desc.nextInChain = reinterpret_cast<WGPUChainedStruct *>(&spirv);
        desc.label = label;
        return wgpuDeviceCreateShaderModule(device, &desc);
    }
}