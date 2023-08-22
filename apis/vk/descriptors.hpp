#pragma once

#if !defined(__ANDROID__)
#include <vulkan/vulkan.hpp>
#endif
#include "vk_data.hpp"
#include <core/engine/renderer/types.hpp>

namespace vk
{

    VkShaderStageFlags getStageFlag(engine::ShaderStage stage) {
        VkShaderStageFlags outputStage{};
        if (static_cast<bool>(stage & engine::ShaderStage::VERTEX)) {
            outputStage = outputStage | VK_SHADER_STAGE_VERTEX_BIT;
        }
        if (static_cast<bool>(stage & engine::ShaderStage::FRAGMENT)) {
            outputStage = outputStage | VK_SHADER_STAGE_FRAGMENT_BIT;
        }
        if (static_cast<bool>(stage & engine::ShaderStage::COMPUTE)) {
            outputStage = outputStage | VK_SHADER_STAGE_COMPUTE_BIT;
        }
        if (static_cast<bool>(stage & engine::ShaderStage::TESSELLATION_EVALUATION)) {
            outputStage = outputStage | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }
        if (static_cast<bool>(stage & engine::ShaderStage::TESSELLATION_CONTROL)) {
            outputStage = outputStage | VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        }
        if (static_cast<bool>(stage & engine::ShaderStage::GEOMETRY)) {
            outputStage = outputStage | VK_SHADER_STAGE_GEOMETRY_BIT;
        }
        if (static_cast<bool>(stage & engine::ShaderStage::MESH)) {
            outputStage = outputStage | VK_SHADER_STAGE_MESH_BIT_EXT;
        }
        if (static_cast<bool>(stage & engine::ShaderStage::TASK)) {
            outputStage = outputStage | VK_SHADER_STAGE_TASK_BIT_EXT;
        }
        return outputStage;
    }

    static VkDescriptorSetLayoutBinding createSSBOBinding(int bind, VkShaderStageFlagBits stage = VkShaderStageFlagBits(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT))
    {
        VkDescriptorSetLayoutBinding ssboLayoutBinding{};
        ssboLayoutBinding.binding = bind;
        ssboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        ssboLayoutBinding.descriptorCount = 1;
        ssboLayoutBinding.stageFlags = stage;
        ssboLayoutBinding.pImmutableSamplers = nullptr;
        return ssboLayoutBinding;
    }
    static VkDescriptorSetLayoutBinding createUboBinding(int bind, VkShaderStageFlagBits stage = VkShaderStageFlagBits(VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT))
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = bind;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = stage;
        uboLayoutBinding.pImmutableSamplers = nullptr;
        return uboLayoutBinding;
    }
    static VkDescriptorSetLayoutBinding createTextureBinding(int bind, int descriptorCount = 1, VkShaderStageFlagBits stage = VK_SHADER_STAGE_FRAGMENT_BIT)
    {
        VkDescriptorSetLayoutBinding samplerLayoutBinding{};
        samplerLayoutBinding.binding = bind;
        samplerLayoutBinding.descriptorCount = descriptorCount;
        samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        samplerLayoutBinding.pImmutableSamplers = nullptr;
        samplerLayoutBinding.stageFlags = stage;
        return samplerLayoutBinding;
    }
    static void createDescriptorSetLayout(const VulkanData &vkData, VkDescriptorSetLayout &descriptorSetLayout, std::vector<engine::UniformBindingInfo> layoutBindings)
    {
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        bool any_bindless = false;
        for (auto &binding : layoutBindings)
        {
            any_bindless |= binding.bindless;
            if (binding.type == engine::UniformBindingType::UNIFORM_BUFFER)
            {
                bindings.push_back(createUboBinding(binding.binding));
            }
            else if (binding.type == engine::UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER)
            {
                bindings.push_back(createTextureBinding(binding.binding, binding.descriptorCount));
            }
            else if (binding.type == engine::UniformBindingType::SHADER_STORAGE) {
                bindings.push_back(createSSBOBinding(binding.binding));
                
            }
        }
        // std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, samplerLayoutBinding};
        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        layoutInfo.pBindings = bindings.data();

        VkDescriptorBindingFlags bindless_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT;
        VkDescriptorSetLayoutBindingFlagsCreateInfoEXT extended_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT, nullptr};
        extended_info.bindingCount = 1;
        if (any_bindless)
        {
            layoutInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

            extended_info.pBindingFlags = &bindless_flags;

            layoutInfo.pNext = &extended_info;
        }

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