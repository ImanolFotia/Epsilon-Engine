#pragma once

#if (BUILD_ANDROID == 0)
#include <vulkan/vulkan.hpp>
#endif
#include "vk_data.hpp"
#include <core/engine/renderer/types.hpp>

namespace vk
{
    static VkDescriptorSetLayoutBinding createUboBinding(int bind)
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = bind;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        return uboLayoutBinding;
    }
    static VkDescriptorSetLayoutBinding createTextureBinding(int bind)
    {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = bind;
        samplerLayoutBinding.descriptorCount = 1;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        return samplerLayoutBinding;
    }
    static void createDescriptorSetLayout(const VulkanData &vkData, VkDescriptorSetLayout &descriptorSetLayout, std::vector<engine::UniformBindingInfo> layoutBindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        for (auto &binding : layoutBindings)
        {
            if (binding.type == engine::UniformBindingType::UNIFORM_BUFFER)
            {
                bindings.push_back(createUboBinding(binding.binding));
            }
            else if (binding.type == engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER)
            {
                bindings.push_back(createTextureBinding(binding.binding));
            }
        }
        // std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        if (vkCreateDescriptorSetLayout(vkData.logicalDevice, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    static void updateDescriptorSet(const VulkanData &vkData, VulkanMaterial &material)
    {
        // vkUpdateDescriptorSets(vkData.logicalDevice, 2, material.descriptorWrites.data(), 0, nullptr);
    }
}