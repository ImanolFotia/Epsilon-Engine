#pragma once

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif

#include <vector>
#include <fstream>
#include "vk_data.hpp"

namespace vk::shader
{
    static std::vector<char> readFile(const std::string &filename, const VulkanData& vk_data)
    {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("failed to open file! " + filename);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    static VkShaderModule createShaderModule(const std::vector<char> &code, const VulkanData& vk_data)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(vk_data.logicalDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create shader module!");
        }

        return shaderModule;
    }

    static VkPipelineShaderStageCreateInfo createShaderStage(VulkanData& vk_data, engine::ShaderStageInfo info)
    {

        VkPipelineShaderStageCreateInfo shaderStageInfo{};

        std::string entry_point = info.entryPoint;

        shaderStageInfo.pName = entry_point.c_str();
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;

        if (info.stage == engine::ShaderModuleStage::FRAGMENT)
        {
            shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        else if (info.stage == engine::ShaderModuleStage::VERTEX)
        {
            shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        }
        else if (info.stage == engine::ShaderModuleStage::COMPUTE)
        {
            shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        }
        else
        {
        }

        //appendShaderStageMacro(info);

        shaderStageInfo.module = createShaderModule(info.shaderCode, vk_data);

        return shaderStageInfo;
    }
}