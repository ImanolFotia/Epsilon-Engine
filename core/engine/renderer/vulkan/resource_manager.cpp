#include "apis/vk/vk_data.hpp"
#include "core/common/common.hpp"
#include "vulkan/vulkan_core.h"
#undef VMA_DEBUG_LOG
#undef VMA_DEBUG_LOG_FORMAT
#define GLM_ENABLE_EXPERIMENTAL
#include "helpers.hpp"
#include "resource_manager.hpp"
#include <apis/vk/barriers.hpp>

#include "core/framework/exception.hpp"

#ifdef _WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

namespace engine {
VulkanResourceManager::VulkanResourceManager() {}

VulkanResourceManager::~VulkanResourceManager() {}

void VulkanResourceManager::Init() {

  Log::Info("Initiating Resource Manager");
  VmaAllocatorCreateInfo allocatorInfo = {};
  allocatorInfo.physicalDevice = m_pVkDataPtr->physicalDevice;
  allocatorInfo.device = m_pVkDataPtr->logicalDevice;
  allocatorInfo.instance = m_pVkDataPtr->instance;
#if !defined(__ANDROID__)
  allocatorInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
#endif

  VkPhysicalDeviceProperties deviceProperties;
  vkGetPhysicalDeviceProperties(m_pVkDataPtr->physicalDevice, &deviceProperties);
  VkPhysicalDeviceFeatures deviceFeatures;
  vkGetPhysicalDeviceFeatures(m_pVkDataPtr->physicalDevice, &deviceFeatures);

  ResourcesMemory.GPUName = std::string(deviceProperties.deviceName);

  vmaCreateAllocator(&allocatorInfo, &m_pAllocator);

  gpuBufferPool.Initialize();
  computeShaderPool.Initialize();
  indexBufferPool.Initialize();
  materialPool.Initialize();
  meshPool.Initialize();
  renderPassPool.Initialize();
  resourceIdPool.Initialize();
  shaderPool.Initialize();
  texPool.Initialize();
  uniformBufferPool.Initialize();
  vertexBufferPool.Initialize();
  pushConstantPool.Initialize();

  m_pCommandPools.emplace_back();

  vk::createCommandPool(*m_pVkDataPtr, m_pCommandPools.back());
  vk::createTransferCommandPool(*m_pVkDataPtr, m_pTransferCommandPool);

  pCreateGlobalDescriptorPool();
  pCreateDescriptorPool();

  vk::createDescriptorSetLayout(*m_pVkDataPtr, m_pGlobalDescriptorSetLayout,
                                {{0,
                                  0,
                                  0,
                                  1,
                                  UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER,
                                  {},
                                  {},
                                  "",
                                  true,
                                  MAX_BINDLESS_RESOURCES,
                                  ShaderStage::FRAGMENT | ShaderStage::VERTEX,
                                  "GlobalBindlessTextures"}},
                                1);

  pCreateGlobalDescriptorSets(m_pGlobalDescriptorSetLayout, m_pGlobalDescriptorPool, m_pGlobalDescriptorSets, MAX_BINDLESS_RESOURCES);

  for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
    m_pIndirectBuffer.emplace_back();
    pCreateBuffer(m_pIndirectBuffer.back(), sizeof(VkDrawIndexedIndirectCommand) * 100000, VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT,
                  VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT |
                      VMA_ALLOCATION_CREATE_MAPPED_BIT,
                  VMA_MEMORY_USAGE_AUTO);
  }
}

Ref<Texture> VulkanResourceManager::createTexture(TextureCreationInfo texInfo) {
  Ref<Texture> refTexture;

  auto format = resolveFormat(texInfo.format, texInfo.isCompressed);
  uint32_t mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texInfo.width, texInfo.height)))) + 1;

  auto size = texInfo.width * texInfo.height * texInfo.numChannels;
  if (texInfo.mipLevels > 1) {
    size = texInfo.size;
    mipLevels = texInfo.mipLevels;
  } else {
    if (!texInfo.isCompressed)
      texInfo.mipLevels = mipLevels;
    else
      mipLevels = texInfo.mipLevels;

    if (texInfo.generateMipMaps == false) {
      texInfo.mipLevels = 1;
      mipLevels = 1;
    }
  }

  auto texture = pCreateTextureBuffer({
      .width = texInfo.width,
      .height = texInfo.height,
      .num_channels = texInfo.numChannels,
      .mipLevels = mipLevels,
      .arrayLayers = texInfo.numLayers,
      .format = format,
      .usage = (!texInfo.storage_image ? VK_IMAGE_USAGE_TRANSFER_DST_BIT : 0u) | (!texInfo.storage_image ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0u) |
               VK_IMAGE_USAGE_SAMPLED_BIT | (texInfo.storage_image ? VK_IMAGE_USAGE_STORAGE_BIT : 0u),
  });

  texture.format = format;
  texture.name = texInfo.name;

  if (texInfo.pixels != nullptr) {
    auto stagingBuffer = pCreateStagingTextureBuffer(texInfo.pixels, texInfo);

    transitionImageLayout(*m_pVkDataPtr, m_pTransferCommandPool, texture.image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          texture.info, 0, texInfo.mipLevels);
    copyBufferToImage(*m_pVkDataPtr, m_pTransferCommandPool, stagingBuffer.buffer, texture.image, static_cast<uint32_t>(texInfo.width),
                      static_cast<uint32_t>(texInfo.height), texInfo.offsets, texInfo.mipLevels);

    transitionImageLayout(*m_pVkDataPtr, m_pTransferCommandPool, texture.image, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture.info, 0, texInfo.mipLevels);

    vmaDestroyBuffer(m_pAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
    // vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);
  } else {

    transitionImageLayout(*m_pVkDataPtr, m_pTransferCommandPool, texture.image, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL, texture.info, 0,
                          texInfo.mipLevels);
  }
  texture.filter = resolveFilter(texInfo.filtering);

  texture.info.mipLevels = mipLevels;
  texture.info.width = texInfo.width;
  texture.info.height = texInfo.height;
  texture.info.addressMode = resolveWrapping(texInfo.wrapMode);
  texture.addressMode = resolveWrapping(texInfo.wrapMode);
  texture.name = texInfo.name;
  vk::createImageView(*m_pVkDataPtr, texture, VK_IMAGE_ASPECT_COLOR_BIT);

  ResourcesMemory.m_pTextureBufferAllocationSize += size;

  // Call gen mips
  pGenerateMipMaps(texInfo, texture);

  vk::createTextureSampler(*m_pVkDataPtr, texture);

  texture.bindingType = vk::MATERIAL_SAMPLER;

  auto ref = texPool.insert(texInfo.name, texture);
  texture.index = ref.m_pIndex;
  texPool.get(ref)->index = ref.m_pIndex;

  if (m_pVkDataPtr->bindless_supported) {
    int count = 1;
    VkWriteDescriptorSet bindless_descriptor_writes;
    VkDescriptorImageInfo bindless_image_info;
    bindless_descriptor_writes = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    bindless_descriptor_writes.descriptorCount = 1;
    bindless_descriptor_writes.dstArrayElement = texture.index;
    bindless_descriptor_writes.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    bindless_descriptor_writes.dstSet = m_pGlobalDescriptorSets;
    bindless_descriptor_writes.dstBinding = 0;

    bindless_image_info.sampler = texture.sampler;
    bindless_image_info.imageView = texture.imageView;
    bindless_image_info.imageLayout = texture.isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    bindless_image_info.imageLayout = texInfo.storage_image ? VK_IMAGE_LAYOUT_GENERAL : bindless_image_info.imageLayout;
    bindless_descriptor_writes.pImageInfo = &bindless_image_info;

    if (count > 0)
      vkUpdateDescriptorSets(m_pVkDataPtr->logicalDevice, count, &bindless_descriptor_writes, 0, nullptr);
  }

  return ref;
}

void VulkanResourceManager::updateBindGroup(Ref<BindGroup> bindGroupRef, std::initializer_list<UniformBindingInfo> updatedFields = {}) {
  vk::VulkanMaterial *vkMaterial = materialPool.get(bindGroupRef);

  //.size = buffer_size, .offset = 0, .binding = 9, .set = 0, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "light_cull_buffer"

  for (auto &binding : updatedFields) {
    for (auto &material_binding : vkMaterial->shaderBindings) {
      if (material_binding.bindingPoint == binding.binding) {

        if (binding.type == UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER) {
          auto tex = createTexture(binding.textureInfo);
          vk::VulkanShaderBinding shaderBinding = {.texture = *texPool.get(tex),
                                                   .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                   .bindingPoint = binding.binding,
                                                   .isRenderPassAttachment = false};
          material_binding = shaderBinding;
        } else if (binding.type == UniformBindingType::STORAGE_IMAGE) {
          auto tex = texPool.get(std::hash<std::string>{}(binding.texture)); // createTexture(binding.textureInfo);
          vk::VulkanShaderBinding shaderBinding = {.texture = *tex,          //*texPool.get(tex),
                                                   .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                                   .bindingPoint = binding.binding,
                                                   .isRenderPassAttachment = false};
          material_binding = shaderBinding;
        } else if (binding.type == UniformBindingType::SHADER_STORAGE) {
          auto &buff = gpuBufferPool.get(std::hash<std::string>{}(binding.buffer))->buffers;
          for (auto &b : buff)
            b.size = binding.size;

          vk::VulkanShaderBinding shaderBinding = {.buffers = buff,
                                                   .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                   .bindingPoint = binding.binding,
                                                   .isRenderPassAttachment = false};
          material_binding = shaderBinding;
        } else if (binding.type == UniformBindingType::UNIFORM_BUFFER) {
          auto &buff = uniformBufferPool.get(std::hash<std::string>{}(binding.buffer))->buffers;
          for (auto &b : buff) {
            b.size = binding.size;
          }

          vk::VulkanShaderBinding shaderBinding = {.buffers = buff,
                                                   .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                   .bindingPoint = binding.binding,
                                                   .isRenderPassAttachment = false};

          material_binding = shaderBinding;

          if (binding.binding == 0) {
            for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
              vkMaterial->bufferInfo[i].offset = buff[i].offset;
              vkMaterial->bufferInfo[i].buffer = buff[i].buffer;
              vkMaterial->bufferSize = buff[i].size;
            }
          }
        }
      }
    }
  }

  pUpdateMaterial(*vkMaterial);
}

void VulkanResourceManager::UpdateUniform(const std::string &name, void *newData) {
  auto &buffers = uniformBufferPool.get(std::hash<std::string>{}(name))->buffers;

  for (auto &buffer : buffers) {
    void *data;
    vmaMapMemory(m_pAllocator, buffer.allocation, &data);
    memcpy(data, newData, buffer.size);
    vmaUnmapMemory(m_pAllocator, buffer.allocation);
  }
}

Ref<BindGroup> VulkanResourceManager::createBindGroup(BindGroupInfo material) {
  // try
  {
    vk::VulkanMaterial vkMaterial;
    // auto renderPass = renderPassPool.get(renderPassRef);

    size_t id = std::hash<std::string>{}(material.renderPass);
    auto renderPass = renderPassPool.get(id);
    vk::VulkanComputeShader *computeShader;

    if (renderPass) {
      vkMaterial.descriptorSetLayout = renderPass->renderPipelines.back().descriptorSetLayouts.at(RENDERPASS_LAYOUT);
    } else {
      size_t compute_id = std::hash<std::string>{}(material.computeShader);
      computeShader = computeShaderPool.get(compute_id);
      vkMaterial.descriptorSetLayout = computeShader->pipeline.descriptorSetLayout;
    }
    // auto &materialdata = m_pMaterials.emplace_back();

    if (renderPass) {
      for (auto &binding : material.inputs) {

        auto pass = renderPassPool.get(std::hash<std::string>{}(binding.renderPass));
        vkMaterial.slots++;

        auto texture =
            binding.index == pass->renderPassChain.Textures.size() ? pass->renderPassChain.DepthTexture : pass->renderPassChain.Textures.at(binding.index);

        vk::VulkanShaderBinding shaderBinding = {.texture = texture,
                                                 .descriptorBinding = binding.type == UniformBindingType::STORAGE_IMAGE
                                                                          ? VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
                                                                          : VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                 .bindingPoint = binding.bindingPoint,
                                                 .isRenderPassAttachment = true,
                                                 .renderpass = binding.renderPass,
                                                 .attachment_index = (int32_t)binding.index};

        shaderBinding.texture.isDepthAttachment = texture.isDepthAttachment;

        vkMaterial.shaderBindings.push_back(shaderBinding);
      }
    } else {
      for (auto &binding : material.inputs) {
        auto pass = renderPassPool.get(std::hash<std::string>{}(binding.renderPass));
        vkMaterial.slots++;

        auto texture =
            binding.index == pass->renderPassChain.Textures.size() ? pass->renderPassChain.DepthTexture : pass->renderPassChain.Textures.at(binding.index);

        vk::VulkanShaderBinding shaderBinding = {.texture = texture,
                                                 .descriptorBinding = binding.type == UniformBindingType::STORAGE_IMAGE
                                                                          ? VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
                                                                          : VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                 .bindingPoint = binding.bindingPoint,
                                                 .isRenderPassAttachment = true,
                                                 .renderpass = binding.renderPass,
                                                 .attachment_index = (int32_t)binding.index};

        shaderBinding.texture.isDepthAttachment = texture.isDepthAttachment;

        vkMaterial.shaderBindings.push_back(shaderBinding);
      }
    }

    for (auto &binding : material.bindingInfo) {

      if (binding.type == UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER) {
        vkMaterial.slots++;
        vk::VulkanTexture texture;

        if (binding.textureInfo.pixels != nullptr) {
          texture = *texPool.get(createTexture(binding.textureInfo));
        } else {
          texture = *texPool.get(std::hash<std::string>{}(binding.texture));
        }

        vk::VulkanShaderBinding shaderBinding = {.texture = texture,
                                                 .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                                 .bindingPoint = binding.binding,
                                                 .isRenderPassAttachment = false};

        vkMaterial.shaderBindings.push_back(shaderBinding);
      } else if (binding.type == UniformBindingType::STORAGE_IMAGE) {
        vkMaterial.slots++;
        auto tex = texPool.get(std::hash<std::string>{}(binding.texture)); // createTexture(binding.textureInfo);
        if (tex != nullptr) {
          vk::VulkanShaderBinding shaderBinding = {.texture = *tex, //*texPool.get(tex),
                                                   .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                                   .bindingPoint = binding.binding,
                                                   .isRenderPassAttachment = false};

          vkMaterial.shaderBindings.push_back(shaderBinding);
        } else {
          auto pass = renderPassPool.get(std::hash<std::string>{}(binding.texture));

          vk::VulkanShaderBinding shaderBinding = {.texture = pass->renderPassChain.Textures.at(0), //*texPool.get(tex),
                                                   .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
                                                   .bindingPoint = binding.binding,
                                                   .isRenderPassAttachment = false};

          vkMaterial.shaderBindings.push_back(shaderBinding);
        }
      } else if (binding.type == UniformBindingType::SHADER_STORAGE) {
        vkMaterial.slots++;
        auto &buff = gpuBufferPool.get(std::hash<std::string>{}(binding.buffer))->buffers;
        for (auto &b : buff)
          b.size = binding.size;

        vk::VulkanShaderBinding shaderBinding = {.buffers = buff,
                                                 .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                                                 .bindingPoint = binding.binding,
                                                 .isRenderPassAttachment = false};

        vkMaterial.shaderBindings.push_back(shaderBinding);
      } else if (binding.type == UniformBindingType::UNIFORM_BUFFER) {
        vkMaterial.slots++;
        auto &buff = uniformBufferPool.get(std::hash<std::string>{}(binding.buffer))->buffers;
        for (auto &b : buff) {
          b.size = binding.size;
        }

        vk::VulkanShaderBinding shaderBinding = {.buffers = buff,
                                                 .descriptorBinding = VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                 .bindingPoint = binding.binding,
                                                 .isRenderPassAttachment = false};

        vkMaterial.shaderBindings.push_back(shaderBinding);

        if (binding.binding == 0) {
          vkMaterial.bufferInfo.resize(vk::MAX_FRAMES_IN_FLIGHT);
          for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
            vkMaterial.bufferInfo[i].offset = buff[i].offset;
            vkMaterial.bufferInfo[i].buffer = buff[i].buffer;
            vkMaterial.bufferSize = buff[i].size;
          }
        }
      }
    }

    if (renderPass) {
      if (renderPass->uniformBuffer.size() > 0) {

        auto &buffers = renderPass->uniformBuffer.front().buffers;
        //vkMaterial.bufferInfo.resize(vk::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
          vkMaterial.bufferInfo[i].offset = buffers[i].offset;
          vkMaterial.bufferInfo[i].buffer = buffers[i].buffer;
        }

        vkMaterial.bufferSize = renderPass->uniformBuffer.front().size;
      }
    }

    m_pNumCommandPools++;

    pCreateDescriptorSets(vkMaterial.descriptorSetLayout, m_pDescriptorPool, vkMaterial.descriptorSets, vk::MAX_FRAMES_IN_FLIGHT);

    pUpdateMaterial(vkMaterial);

    Ref<BindGroup> materialRef = materialPool.insert(material.name, vkMaterial);
    return materialRef;
  }
  // catch (std::exception& e)
  {

    //	throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
  }
}

void VulkanResourceManager::pRecreateSwapChain(framework::Window::windowType *window) {}

Ref<RenderPass> VulkanResourceManager::createDefaultRenderPass(RenderPassInfo renderPassInfo) {
  m_pVkDataPtr->defaultRenderPass.id = std::numeric_limits<uint32_t>::max();
  m_pDefaultRenderPassInfo = renderPassInfo;

  m_pVkDataPtr->defaultRenderPass.clearValues.resize(renderPassInfo.numAttachments + (renderPassInfo.depthAttachment ? 1 : 0));
  for (int i = 0; i < renderPassInfo.numAttachments; i++) {
    m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[0] = renderPassInfo.attachments[i].clearColor[0];
    m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[1] = renderPassInfo.attachments[i].clearColor[1];
    m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[2] = renderPassInfo.attachments[i].clearColor[2];
    m_pVkDataPtr->defaultRenderPass.clearValues[i].color.float32[3] = renderPassInfo.attachments[i].clearColor[3];
  }
  if (renderPassInfo.depthAttachment) {
    ResourcesMemory.m_pTextureBufferAllocationSize += renderPassInfo.dimensions.width * renderPassInfo.dimensions.height * 4;
    m_pVkDataPtr->defaultRenderPass.clearValues.back().depthStencil = {(float)renderPassInfo.attachments.back().depthStencilValue[0],
                                                                       renderPassInfo.attachments.back().depthStencilValue[1]};
  }

  vk::createRenderPass(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, renderPassInfo, true);

  m_pVkDataPtr->defaultRenderPass.renderPipelines.resize(renderPassInfo.numLayouts);
  m_pVkDataPtr->defaultRenderPass.renderPipelines.back().descriptorSetLayouts.resize(2);
  vk::createDescriptorSetLayout(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass.renderPipelines.back().descriptorSetLayouts.at(RENDERPASS_LAYOUT),
                                renderPassInfo.bindingInfo, 0);

  m_pVkDataPtr->defaultRenderPass.numAttachments = renderPassInfo.depthAttachment ? renderPassInfo.numAttachments - 1 : renderPassInfo.numAttachments;

  for (auto &pipeline : m_pVkDataPtr->defaultRenderPass.renderPipelines) {
    pipeline.descriptorSetLayouts.resize(1);
    if (m_pVkDataPtr->bindless_supported) {
      pipeline.descriptorSetLayouts.resize(2);
      pipeline.descriptorSetLayouts.at(GLOBAL_LAYOUT) = m_pGlobalDescriptorSetLayout;
    }
  }
  vk::createGraphicsPipeline(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, renderPassInfo);

  vk::createSwapChainFramebuffers(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, m_pVkDataPtr->defaultRenderPass.renderPassChain);

  int index = 0;
  for (auto &texture : m_pVkDataPtr->defaultRenderPass.renderPassChain.Textures) {
    texture.name = "SwapChainRenderTarget" + std::to_string(index);
    //texPool.insert(texture.name, texture);
    index++;
  }
  m_pVkDataPtr->defaultRenderPass.renderPassChain.DepthTexture.name = "SwapChainDepthRenderTarget";
  //texPool.insert("SwapChainDepthRenderTarget", m_pVkDataPtr->defaultRenderPass.renderPassChain.DepthTexture);

  m_pVkDataPtr->defaultRenderPass.name = renderPassInfo.name;

  VkViewport viewport = {.x = 0,
                         .y = 0,
                         .width = (float)m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.width,
                         .height = (float)m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.height,
                         .minDepth = 0.0f,
                         .maxDepth = 1.0f};

  VkRect2D rect;
  rect.extent.width = m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.width;
  rect.extent.height = m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent.height;

  rect.offset.x = 0;
  rect.offset.y = 0;

  m_pVkDataPtr->defaultRenderPass.renderPassChain.setViewport(viewport);
  m_pVkDataPtr->defaultRenderPass.renderPassChain.setScissor(rect);

  for (auto &binding : renderPassInfo.bindingInfo) {
    if (binding.type == UniformBindingType::UNIFORM_BUFFER) {
      auto uniformRef = createUniformData(binding);

      m_pVkDataPtr->defaultRenderPass.uniformBuffer.push_back(*uniformBufferPool.get(uniformRef));
      
    }
  }

  m_pDefaultRenderPassRef = renderPassPool.insert(renderPassInfo.name, m_pVkDataPtr->defaultRenderPass);

  return m_pDefaultRenderPassRef;
}

Ref<RenderPass> VulkanResourceManager::createRenderPass(RenderPassInfo renderPassInfo) {
  vk::VulkanRenderPass renderPass = {};

  renderPass.clearValues.resize(renderPassInfo.numAttachments /*+ (renderPassInfo.depthAttachment ? 1 : 0))*/);
  for (int i = 0; i < renderPassInfo.numAttachments; i++) {
    renderPass.clearValues[i].color = {renderPassInfo.attachments[i].clearColor[0], renderPassInfo.attachments[i].clearColor[1],
                                       renderPassInfo.attachments[i].clearColor[2], renderPassInfo.attachments[i].clearColor[3]};
  }
  if (renderPassInfo.depthAttachment)
    renderPass.clearValues.back().depthStencil = {(float)renderPassInfo.attachments.back().depthStencilValue[0],
                                                  renderPassInfo.attachments.back().depthStencilValue[1]};

  renderPass.id = m_pRenderPassCount;

  renderPass.renderPassChain.ImageFormats.resize(renderPassInfo.attachments.size());

  for (int index = 0; index < renderPassInfo.attachments.size(); index++)
    renderPass.renderPassChain.ImageFormats[index] = resolveFormat(renderPassInfo.attachments[index].format);

  vk::createRenderPass(*m_pVkDataPtr, renderPass, renderPassInfo, false);

  for (size_t i = 0; i < renderPassInfo.attachments.size(); i++) {
    auto &attachment = renderPassInfo.attachments[i];

    if (attachment.isExtern) {
      bool externValid = false;
      auto externRenderPass = renderPassPool.get(std::hash<std::string>{}(attachment.externRenderPass));

      for (auto &externAttachment : m_pRenderPassInfo[externRenderPass->id].attachments) {
        if (externAttachment.name == attachment.name) {
          externValid = true;
          renderPass.renderPassChain.Textures.push_back(externRenderPass->renderPassChain.Textures.at(externAttachment.texture_index));
          renderPass.renderPassChain.ImageViews.push_back(externRenderPass->renderPassChain.ImageViews.at(externAttachment.imageview_index));

          if (attachment.isDepthAttachment)
            renderPass.renderPassChain.DepthTexture = externRenderPass->renderPassChain.Textures.at(externAttachment.texture_index);
        }
      }

      if (externValid)
        continue;
    }

    vk::VulkanTextureInfo texInfo;
    texInfo.format = attachment.isDepthAttachment ? findDepthFormat(*m_pVkDataPtr) : resolveFormat(renderPassInfo.attachments[i].format);

    texInfo.addressMode = resolveWrapping(attachment.wrapMode);
    texInfo.compareOp = resolveCompareOp(attachment.compareFunc);
    texInfo.filter = resolveFilter(attachment.filtering);
    texInfo.width = renderPassInfo.dimensions.width;
    texInfo.height = renderPassInfo.dimensions.height;
    texInfo.num_channels = attachment.isDepthAttachment ? 1 : resolveNumChannels(attachment.format);
    texInfo.usage =
        ((attachment.isDepthAttachment || texInfo.format == VK_FORMAT_D32_SFLOAT) ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                                                                                  : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT) |
        (attachment.storageImage ? VK_IMAGE_USAGE_STORAGE_BIT : 0u);

    texInfo.compareEnable = attachment.depthCompare;

    vk::VulkanTexture texture = pCreateTextureBuffer(texInfo);
    texture.name = attachment.name;
    texture.imageLayout = attachment.isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    texture.imageLayout = attachment.storageImage ? VK_IMAGE_LAYOUT_GENERAL : texture.imageLayout;
    texture.bindingType = vk::RENDER_BUFFER_SAMPLER;
    texture.info.mipLevels = 1;
    createImageView(*m_pVkDataPtr, texture,
                    (attachment.isDepthAttachment || texInfo.format == VK_FORMAT_D32_SFLOAT) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

    if (attachment.isSampler) {
      texture.info.isSampler = true;
      vk::createTextureSampler(*m_pVkDataPtr, texture);

    }

    if (attachment.isDepthAttachment) {
      texture.isDepthAttachment = true;

      renderPass.renderPassChain.DepthTexture = texture;

      if (attachment.isSampler) {
        renderPass.renderPassChain.hasDepthSampler = true;
        renderPass.renderPassChain.ImageViews.push_back(texture.imageView);
        renderPass.renderPassChain.Textures.push_back(texture);
        attachment.imageview_index = renderPass.renderPassChain.ImageViews.size() - 1;
        attachment.texture_index = renderPass.renderPassChain.Textures.size() - 1;
        renderPass.renderPassChain.DepthTexture.index = i;
      }
    } else {
      renderPass.renderPassChain.ImageViews.push_back(texture.imageView);
      renderPass.renderPassChain.Textures.push_back(texture);
      attachment.imageview_index = renderPass.renderPassChain.ImageViews.size() - 1;
      attachment.texture_index = renderPass.renderPassChain.Textures.size() - 1;
    }

    //texPool.insert(attachment.name, texture);
  }

  m_pRenderPassInfo[renderPass.id] = renderPassInfo;

  renderPass.renderPassChain.Extent.width = renderPassInfo.dimensions.width;
  renderPass.renderPassChain.Extent.height = renderPassInfo.dimensions.height;
  renderPass.name = renderPassInfo.name;

  renderPass.renderPipelines.resize(renderPassInfo.numLayouts); //.emplace_back();

  for (auto &pipeline : renderPass.renderPipelines) {
    pipeline.descriptorSetLayouts.resize(1);
    if (m_pVkDataPtr->bindless_supported) {
      pipeline.descriptorSetLayouts.resize(2);
      pipeline.descriptorSetLayouts.at(GLOBAL_LAYOUT) = m_pGlobalDescriptorSetLayout;
    }
  }

  vk::createGraphicsPipeline(*m_pVkDataPtr, renderPass, renderPassInfo);

  vk::createFramebuffers(*m_pVkDataPtr, renderPass, renderPass.renderPassChain);

  VkViewport viewport = {.x = 0,
                         .y = 0,
                         .width = (float)renderPass.renderPassChain.Extent.width,
                         .height = (float)renderPass.renderPassChain.Extent.height,
                         .minDepth = 0.0f,
                         .maxDepth = 1.0f};

  VkRect2D rect;
  rect.extent.width = renderPass.renderPassChain.Extent.width;
  rect.extent.height = renderPass.renderPassChain.Extent.height;

  rect.offset.x = 0;
  rect.offset.y = 0;

  renderPass.renderPassChain.setViewport(viewport);
  renderPass.renderPassChain.setScissor(rect);

  for (auto &binding : renderPassInfo.bindingInfo) {
    if (binding.type == UniformBindingType::UNIFORM_BUFFER) {
      auto uniformRef = createUniformData(binding);
      renderPass.uniformBuffer.push_back(*uniformBufferPool.get(uniformRef));
      
    } else if (binding.type == UniformBindingType::SHADER_STORAGE) {
    }
  }
  auto ref = renderPassPool.insert(renderPassInfo.name, renderPass);
  for (auto &pipeline : renderPassInfo.pipelineLayout) {
    shaderPool.insert(pipeline.shaderInfo.name, pipeline.shaderInfo);
  }

  if (renderPassInfo.resizeWithSwapChain) {
    resizableRenderPasses.push_back(ref);
  }

  m_pRenderPassCount++;
  return ref;
}

void VulkanResourceManager::ReloadShaders(const std::string &shaderName) {
  uint32_t id = std::hash<std::string>{}(shaderName);
  auto shaderInfo = shaderPool.get(id);

  for (auto &stage : shaderInfo->stages) {
    for (auto &renderPassId : stage.renderPassIds) {
      auto renderPass = renderPassPool.get(renderPassId);
      for (auto &pipelineId : stage.pipelines[renderPassId])
        (void)renderPass->renderPipelines[pipelineId];
    }
  }
}

void VulkanResourceManager::clean() {

  vkDeviceWaitIdle(m_pVkDataPtr->logicalDevice);
  for (auto &buffer : vertexBufferPool) {
    vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
  }

  for (auto &buffer : indexBufferPool) {
    vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
  }

  for (auto &buffer : gpuBufferPool) {
    for (auto &b : buffer.buffers) {
      if (b.mapped) {
        vmaUnmapMemory(m_pAllocator, b.allocation);
        b.mapped = false;
      }
    }
  }

  for (auto &buffer : gpuBufferPool) {
    for (auto &b : buffer.buffers) {
      vmaDestroyBuffer(m_pAllocator, b.buffer, b.allocation);
    }
  }

  vk::cleanupSyncObjects(*m_pVkDataPtr);

  for (auto &commandPool : m_pVkDataPtr->m_pCommandPools)
    vk::cleanCommandPool(*m_pVkDataPtr, commandPool);

  for (auto &commandPool : m_pCommandPools)
    vk::cleanCommandPool(*m_pVkDataPtr, commandPool);

  for (auto &texture : texPool) {
    vkDestroySampler(m_pVkDataPtr->logicalDevice, texture.sampler, nullptr);
    texture.sampler = VK_NULL_HANDLE;
    vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture.imageView, nullptr);
    texture.imageView = VK_NULL_HANDLE;
    vmaDestroyImage(m_pAllocator, texture.image, texture.allocation);
    texture.image = VK_NULL_HANDLE;
  }

  for (auto &pass : renderPassPool) {
    vk::cleanupRenderPass(*m_pVkDataPtr, pass.renderPass);

    for (auto &pipeline : pass.renderPipelines)
      vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);

    for (auto &uniformBuffer : pass.uniformBuffer)
      for (auto &b : uniformBuffer.buffers) {
        if (b.buffer != VK_NULL_HANDLE) {
          vmaDestroyBuffer(m_pAllocator, b.buffer, b.allocation);
        }
      }

    if (pass.id == std::numeric_limits<uint32_t>::max())
      continue;

    for (auto &framebuffer : pass.renderPassChain.Framebuffers)
      vkDestroyFramebuffer(m_pVkDataPtr->logicalDevice, framebuffer, nullptr);

    for (int i = 0; i < pass.renderPassChain.Textures.size(); i++) {
      if (pass.renderPassChain.Textures[i].sampler != VK_NULL_HANDLE) {
        vkDestroySampler(m_pVkDataPtr->logicalDevice, pass.renderPassChain.Textures[i].sampler, nullptr);
        pass.renderPassChain.Textures[i].sampler = VK_NULL_HANDLE;
      }

      if (pass.renderPassChain.Textures[i].imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass.renderPassChain.Textures[i].imageView, nullptr);
        pass.renderPassChain.Textures[i].imageView = VK_NULL_HANDLE;
      }

      if (pass.renderPassChain.Textures[i].image != VK_NULL_HANDLE) {
        vmaDestroyImage(m_pAllocator, pass.renderPassChain.Textures[i].image, pass.renderPassChain.Textures[i].allocation);
        pass.renderPassChain.Textures[i].image = VK_NULL_HANDLE;
      }
    }

    if (pass.renderPassChain.DepthTexture.isDepthAttachment) {

      if (pass.renderPassChain.DepthTexture.isDepthAttachment && !pass.renderPassChain.hasDepthSampler) {
        if (pass.renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE && pass.renderPassChain.DepthTexture.index < 0)
          vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass.renderPassChain.DepthTexture.imageView, nullptr);
        if (pass.renderPassChain.DepthTexture.image != VK_NULL_HANDLE && pass.renderPassChain.DepthTexture.index < 0)
          vmaDestroyImage(m_pAllocator, pass.renderPassChain.DepthTexture.image, pass.renderPassChain.DepthTexture.allocation);

        continue;
      }
      if (pass.renderPassChain.hasDepthSampler && pass.renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE &&
          pass.renderPassChain.Textures[pass.renderPassChain.DepthTexture.index].sampler != VK_NULL_HANDLE)
        vkDestroySampler(m_pVkDataPtr->logicalDevice, pass.renderPassChain.DepthTexture.sampler, nullptr);

      if (pass.renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE &&
          pass.renderPassChain.Textures[pass.renderPassChain.DepthTexture.index].imageView != VK_NULL_HANDLE)
        vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass.renderPassChain.DepthTexture.imageView, nullptr);

      if (pass.renderPassChain.DepthTexture.image != VK_NULL_HANDLE &&
          pass.renderPassChain.Textures[pass.renderPassChain.DepthTexture.index].imageView != VK_NULL_HANDLE) {
        vmaDestroyImage(m_pAllocator, pass.renderPassChain.DepthTexture.image, pass.renderPassChain.DepthTexture.allocation);
        pass.renderPassChain.DepthTexture.image = VK_NULL_HANDLE;
      }
    }
  }

  if (m_pVkDataPtr->defaultRenderPass.renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE) {
    vkDestroySampler(m_pVkDataPtr->logicalDevice, m_pVkDataPtr->defaultRenderPass.renderPassChain.DepthTexture.sampler, nullptr);
  }

  vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);
  vkDestroyCommandPool(m_pVkDataPtr->logicalDevice, m_pTransferCommandPool, nullptr);

  vkDestroyDescriptorSetLayout(m_pVkDataPtr->logicalDevice, m_pGlobalDescriptorSetLayout, nullptr);
  vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pGlobalDescriptorPool, nullptr);

  for (auto &buffer : m_pIndirectBuffer)
    vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);

  vmaDestroyAllocator(m_pAllocator);
  vk::cleanup(*m_pVkDataPtr);

  materialPool.Destroy();
  meshPool.Destroy();
  shaderPool.Destroy();
  computeShaderPool.Destroy();
  texPool.Destroy();
  gpuBufferPool.Destroy();
  indexBufferPool.Destroy();
  uniformBufferPool.Destroy();
  vertexBufferPool.Destroy();
  pushConstantPool.Destroy();
  renderPassPool.Destroy();
}

/**
 * Create functions
 */
Ref<Buffer> VulkanResourceManager::destroyBuffer(const BufferInfo &) { throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__); }

Ref<UniformBindings> VulkanResourceManager::createUniformData(UniformBindingInfo bindingInfo) {
  auto buffer = pCreateUniformBuffer(bindingInfo);
  return uniformBufferPool.insert(bindingInfo.name, buffer);
}

Ref<Mesh> VulkanResourceManager::insertMesh(const std::string &name, MeshResource meshResource) {

  auto ref = meshPool.insert(name, meshResource);

  return ref;
}

Ref<Mesh> VulkanResourceManager::createMesh(MeshInfo meshInfo) {
  std::vector<common::Vertex> *vertices = &meshInfo.vertices;
  std::vector<uint32_t> *indices = &meshInfo.indices;

  auto maxOffset = [](auto &indices) -> uint32_t {
    uint32_t out = 0;
    for (auto &i : indices)
      out = i > out ? i : out;
    return out;
  };

  Ref<Buffer> vertexBufferRef;
  Ref<Buffer> indexBufferRef;

  vk::VulkanBuffer *vertexBuffer = nullptr;
  vk::VulkanBuffer *indexBuffer = nullptr;

  size_t vOffset = 0;
  size_t iOffset = 0;
  size_t prevOffset = 0;
  size_t preiOffset = 0;

  size_t vsize = vertices->size() * sizeof(common::Vertex);
  size_t isize = indices->size() * sizeof(IndexType);

  ResourceCache::Block cached_vertex_block = m_pVertexCache.GetBlock(vsize);
  ResourceCache::Block cached_index_block = m_pIndexCache.GetBlock(isize);
  bool usedVertexCache = false;
  bool usedIndexCache = false;
  if (!cached_vertex_block.empty) {
    vertexBuffer = vertexBufferPool.get(cached_vertex_block.buffer);
    vertexBufferRef = cached_vertex_block.buffer;
    vOffset = cached_vertex_block.offset * sizeof(common::Vertex);
    prevOffset = cached_vertex_block.offset;
    usedVertexCache = true;
  } else {
    vertexBufferRef = pFetchVertexBuffer(vertices->size(), sizeof(common::Vertex));
    vertexBuffer = vertexBufferPool.get(vertexBufferRef);
    vOffset = vertexBuffer->allocatedVertices * sizeof(common::Vertex);
    prevOffset = vertexBuffer->allocatedVertices;
  }

  if (!cached_index_block.empty) {
    indexBuffer = indexBufferPool.get(cached_index_block.buffer);
    iOffset = cached_index_block.offset * sizeof(IndexType);
    indexBufferRef = cached_index_block.buffer;
    preiOffset = cached_index_block.offset;
    usedIndexCache = true;
  } else {
    indexBufferRef = pFetchIndexBuffer(indices->size(), vertexBuffer->allocatedVertices);
    indexBuffer = indexBufferPool.get(indexBufferRef);
    iOffset = indexBuffer->allocatedVertices * sizeof(IndexType);
    preiOffset = indexBuffer->allocatedVertices;
  }

  auto vertexStagingBuffer = pCreateStagingBuffer(*vertices);
  auto indexStagingBuffer = pCreateStagingIndexBuffer(*indices);

  vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, vertexStagingBuffer.buffer, vertexBuffer->buffer, vertices->size() * sizeof(common::Vertex), vOffset);

  vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);

  vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, indexStagingBuffer.buffer, indexBuffer->buffer, indices->size() * sizeof(IndexType), iOffset);

  vmaDestroyBuffer(m_pAllocator, indexStagingBuffer.buffer, indexStagingBuffer.allocation);

  MeshResource meshResource = {.vertexBuffer = vertexBufferRef,
                               .indexBuffer = indexBufferRef,
                               .vertexOffset = static_cast<int32_t>(prevOffset),
                               .indexOffset = static_cast<int32_t>(preiOffset),
                               .numVertices = (int32_t)vertices->size(),
                               .numIndices = (int32_t)indices->size(),
                               .element_size = sizeof(common::Vertex)};

  auto ref = meshPool.insert(meshInfo.name, meshResource);

  if (!usedIndexCache)
    int maxAllocatingSize = sizeof(IndexType) * (indexBuffer->allocatedVertices + indices->size());

  if (!usedVertexCache) {
    vertexBuffer->allocatedVertices += vertices->size();
  }

  if (!usedIndexCache)
    indexBuffer->allocatedVertices += indices->size();

  return ref;
}

Ref<Mesh> VulkanResourceManager::createMesh(AnimatedMeshInfo meshInfo) {
  std::vector<common::AnimatedVertex> *vertices = &meshInfo.vertices;
  std::vector<uint32_t> *indices = &meshInfo.indices;

  auto maxOffset = [](auto &indices) -> uint32_t {
    uint32_t out = 0;
    for (auto &i : indices)
      out = i > out ? i : out;
    return out;
  };

  Ref<Buffer> vertexBufferRef;
  Ref<Buffer> indexBufferRef;

  vk::VulkanBuffer *vertexBuffer = nullptr;
  vk::VulkanBuffer *indexBuffer = nullptr;

  size_t vOffset = 0;
  size_t iOffset = 0;

  size_t vkey = vertices->size() * sizeof(common::AnimatedVertex);
  size_t ikey = indices->size();

  ResourceCache::Block cached_vertex_block = m_pVertexCache.GetBlock(vkey);
  ResourceCache::Block cached_index_block = m_pIndexCache.GetBlock(ikey);

  if (!cached_vertex_block.empty) {
    vertexBuffer = vertexBufferPool.get(cached_vertex_block.buffer);
    vOffset = cached_vertex_block.offset;
  } else {
    vertexBufferRef = pFetchVertexBuffer(vertices->size(), sizeof(common::AnimatedVertex));
    vertexBuffer = vertexBufferPool.get(vertexBufferRef);
    vOffset = vertexBuffer->allocatedVertices * sizeof(common::AnimatedVertex);
  }

  if (!cached_index_block.empty) {
    indexBuffer = indexBufferPool.get(cached_index_block.buffer);
    iOffset = cached_index_block.offset;
  } else {
    indexBufferRef = pFetchIndexBuffer(indices->size(), vertexBuffer->allocatedVertices);
    indexBuffer = indexBufferPool.get(indexBufferRef);
    iOffset = indexBuffer->allocatedVertices * sizeof(IndexType);
  }

  auto vertexStagingBuffer = pCreateStagingBuffer(*vertices);
  auto indexStagingBuffer = pCreateStagingIndexBuffer(*indices);

  vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, vertexStagingBuffer.buffer, vertexBuffer->buffer, vertices->size() * sizeof(common::AnimatedVertex),
                 vOffset);

  vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, indexStagingBuffer.buffer, indexBuffer->buffer, indices->size() * sizeof(IndexType), iOffset);

  vmaDestroyBuffer(m_pAllocator, indexStagingBuffer.buffer, indexStagingBuffer.allocation);
  vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);

  MeshResource meshResource = {.vertexBuffer = vertexBufferRef,
                               .indexBuffer = indexBufferRef,
                               .vertexOffset = vertexBuffer->allocatedVertices,
                               .indexOffset = indexBuffer->allocatedVertices,
                               .numVertices = (int32_t)vertices->size(),
                               .numIndices = (int32_t)indices->size(),
                               .element_size = sizeof(common::AnimatedVertex)};

  auto ref = meshPool.insert(meshInfo.name, meshResource);

  int maxAllocatingSize = sizeof(IndexType) * (indexBuffer->allocatedVertices + indices->size());

  vertexBuffer->allocatedVertices += vertices->size();
  indexBuffer->allocatedVertices += indices->size();

  return ref;
}

Ref<Buffer> VulkanResourceManager::createGPUBuffer(const std::string &name, uint32_t size, BufferStorageType type, int count = -1,
                                                   GPUBufferUsage usage = GPUBufferUsage::SHARED) {

  int numBuffers = count < 0 ? vk::MAX_FRAMES_IN_FLIGHT : count;
  vk::VulkanGPUMappedBuffer buffer;
  buffer.buffers.resize(numBuffers);

  for (unsigned i = 0; i < numBuffers; i++) {
    if (type == BufferStorageType::UNIFORM_BUFFER) {
      pCreateBuffer(buffer.buffers[i], size, UNIFORM_BUFFER_USAGE, UNIFORM_BUFFER_PROP, UNIFORM_BUFFER_MEM_USAGE);
    } else {
      pCreateBuffer(buffer.buffers[i], size, STORAGE_BUFFER_USAGE, STORAGE_BUFFER_PROP, STORAGE_BUFFER_MEM_USAGE);
    }
  }

  buffer.size = size;
  return gpuBufferPool.insert(name, buffer);
}

/**
 * Get functions
 *
 */

vk::VulkanRenderPass *VulkanResourceManager::getRenderPass(Ref<RenderPass> ref) { return renderPassPool.get(ref); }

/**
 * Destroy functions
 */
void VulkanResourceManager::destroyTexture(Ref<Texture> textureRef) {
  auto texture = texPool.get(textureRef);
  vkDestroySampler(m_pVkDataPtr->logicalDevice, texture->sampler, nullptr);
  vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture->imageView, nullptr);
  vmaDestroyImage(m_pAllocator, texture->image, texture->allocation);
}

void VulkanResourceManager::destroyBuffer(Ref<Buffer> buffer_ref) {
  auto buffer = gpuBufferPool.get(buffer_ref);

  for (int i = 0; i < buffer->buffers.size(); i++) {
    if (buffer->mapped)
      vmaUnmapMemory(m_pAllocator, buffer->buffers[i].allocation);
    vmaDestroyBuffer(m_pAllocator, buffer->buffers[i].buffer, buffer->buffers[i].allocation);
  }

  gpuBufferPool.destroy(buffer_ref);
}

void VulkanResourceManager::destroyUniformData(Ref<UniformBindings>) { throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__); }

void VulkanResourceManager::destroyMaterial(Ref<BindGroup>) { throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__); }

void VulkanResourceManager::destroyMesh(Ref<Mesh> meshRef) {
  auto mesh = meshPool.get(meshRef);

  m_pVertexCache.Free((ResourceCache::Block){
      .empty = false, .size = mesh->numVertices * (uint32_t)mesh->element_size, .offset = (size_t)mesh->vertexOffset, .buffer = mesh->vertexBuffer});

  m_pIndexCache.Free(
      (ResourceCache::Block){.empty = false, .size = mesh->numIndices * sizeof(uint32_t), .offset = (size_t)mesh->indexOffset, .buffer = mesh->indexBuffer});

  meshPool.destroy(meshRef);
}

void VulkanResourceManager::destroyRenderPass(Ref<RenderPass> renderPassRef) {
  auto pass = renderPassPool.get(renderPassRef);
  vk::cleanupRenderPass(*m_pVkDataPtr, pass->renderPass);
  for (auto &pipeline : pass->renderPipelines)
    vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);

  if (pass->id == std::numeric_limits<uint32_t>::max())
    return;

  for (int i = 0; i < pass->renderPassChain.Textures.size(); i++) {
    vkDestroySampler(m_pVkDataPtr->logicalDevice, pass->renderPassChain.Textures[i].sampler, nullptr);
    vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass->renderPassChain.Textures[i].imageView, nullptr);
    vmaDestroyImage(m_pAllocator, pass->renderPassChain.Textures[i].image, pass->renderPassChain.Textures[i].allocation);
  }

  vmaDestroyImage(m_pAllocator, pass->renderPassChain.DepthTexture.image, pass->renderPassChain.DepthTexture.allocation);
}

void *VulkanResourceManager::mapBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame) {
  auto buffer = gpuBufferPool.get(bufferRef);
  void *data;
  if (buffer->buffers[currentFrame].mapped) {

    vmaUnmapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation);
  }
  vmaMapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation, &data);
  buffer->buffers[currentFrame].mapped = true;
  return data;
}
void VulkanResourceManager::unmapBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame) {
  auto buffer = gpuBufferPool.get(bufferRef);

  if (!buffer->buffers[currentFrame].mapped)
    return;
  buffer->buffers[currentFrame].mapped = false;
  vmaUnmapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation);
}

void *VulkanResourceManager::getMappedBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame) {
  auto buffer = gpuBufferPool.get(bufferRef);

  buffer->buffers[currentFrame].mapped = true;

  void *data;
  if (buffer->mapped) {

    vmaUnmapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation);
  }
  vmaMapMemory(m_pAllocator, buffer->buffers[currentFrame].allocation, &data);

  return data;
}

Ref<ComputeShader> VulkanResourceManager::createComputeShader(ComputeShaderInfo computeInfo) {
  vk::VulkanComputeShader computeShader;

  VkImageSubresourceRange imageSubRange = {};
  imageSubRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  imageSubRange.baseMipLevel = 0;
  imageSubRange.levelCount = 1;
  imageSubRange.layerCount = 1;

  computeShader.groupCountX = computeInfo.groupCountX;
  computeShader.groupCountY = computeInfo.groupCountY;
  computeShader.groupCountZ = computeInfo.groupCountZ;
  vk::createComputePipeline(*m_pVkDataPtr, computeInfo, computeShader);

  for (auto &imageBarrier : computeInfo.memoryBarrier) {
    if (imageBarrier.image.size() > 0) {
      vk::VulkanTexture texture;

      auto pass = renderPassPool.get(std::hash<std::string>{}(imageBarrier.image));
      if (pass)
        texture = pass->renderPassChain.Textures.at(0);
      else
        texture = *texPool.get(std::hash<std::string>{}(imageBarrier.image));

      engine::MemoryBarrierHint hint;

      VkAccessFlags srcAccessMask;
      VkAccessFlags dstAccessMask;
      VkPipelineStageFlags2 srcStageMask;
      VkPipelineStageFlags2 dstStageMask;

      vk::ResolveMasks(imageBarrier.memoryBarrierHint, srcAccessMask, dstAccessMask, srcStageMask, dstStageMask);

      VkImageMemoryBarrier2 imageMemoryBarrier = {};
      imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
      imageMemoryBarrier.newLayout = texture.imageLayout;//VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      imageMemoryBarrier.image = texture.image;
      
      imageMemoryBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT, 0, 1, 0, 1};
      imageMemoryBarrier.srcAccessMask = srcAccessMask;
      imageMemoryBarrier.dstAccessMask = dstAccessMask;
      imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      imageMemoryBarrier.srcStageMask = srcStageMask;
      imageMemoryBarrier.dstStageMask = dstStageMask;

      computeShader.hasImageBarrier = true;
      computeShader.imageMemoryBarrier.push_back(imageMemoryBarrier);
    }
  }

  auto ref = computeShaderPool.insert(computeInfo.name, computeShader);
  return ref;
}

Ref<ComputeShader> VulkanResourceManager::UpdateComputeShader(Ref<ComputeShader> computeShaderRef, ComputeShaderInfo computeInfo) {

  vk::VulkanComputeShader *computeShader = computeShaderPool.get(computeShaderRef);
  computeShader->groupCountX = computeInfo.groupCountX;
  computeShader->groupCountY = computeInfo.groupCountY;
  computeShader->groupCountZ = computeInfo.groupCountZ;

  return computeShaderRef;
}

Ref<Buffer> VulkanResourceManager::createMappedVertexBuffer(const std::string &name, const BufferInfo &buffer_info) {

  vk::VulkanBuffer buffer;

  pCreateBuffer(buffer, buffer_info.size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT,
                VMA_MEMORY_USAGE_AUTO);

  return vertexBufferPool.insert(name, buffer);
}

void VulkanResourceManager::UpdateMappedBuffer(Ref<Buffer> buff, const UpdateVertexBufferInfo &info) {
  vk::VulkanBuffer *buffer = vertexBufferPool.get(buff);
  if (info.size + info.offset <= buffer->dataSize) {

    void *data;
    vmaMapMemory(m_pAllocator, buffer->allocation, &data);

    std::memcpy(data, info.vertices, info.size);

    vmaUnmapMemory(m_pAllocator, buffer->allocation);
  }
}

void VulkanResourceManager::destroyComputeShader(Ref<ComputeShader>) {}

void VulkanResourceManager::ResizeFramebuffer(Ref<RenderPass> renderpass_ref, glm::ivec2 size) {
  vkDeviceWaitIdle(m_pVkDataPtr->logicalDevice);
  auto renderPass = renderPassPool.get(renderpass_ref);
  VkExtent2D extent;
  extent.width = size.x;
  extent.height = size.y;

  for (auto &framebuffer : renderPass->renderPassChain.Framebuffers)
    vkDestroyFramebuffer(m_pVkDataPtr->logicalDevice, framebuffer, nullptr);

  for (int i = 0; i < renderPass->renderPassChain.Textures.size(); i++) {
    vkDestroySampler(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.Textures[i].sampler, nullptr);
    vkDestroyImageView(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.Textures[i].imageView, nullptr);
    vmaDestroyImage(m_pAllocator, renderPass->renderPassChain.Textures[i].image, renderPass->renderPassChain.Textures[i].allocation);
  }
  {

    if (renderPass->renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE && renderPass->renderPassChain.DepthTexture.index < 0)
      vkDestroySampler(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.DepthTexture.sampler, nullptr);

    if (renderPass->renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE && renderPass->renderPassChain.DepthTexture.index < 0)
      vkDestroyImageView(m_pVkDataPtr->logicalDevice, renderPass->renderPassChain.DepthTexture.imageView, nullptr);
    if (renderPass->renderPassChain.DepthTexture.image != VK_NULL_HANDLE && renderPass->renderPassChain.DepthTexture.index < 0)
      vmaDestroyImage(m_pAllocator, renderPass->renderPassChain.DepthTexture.image, renderPass->renderPassChain.DepthTexture.allocation);
  }

  auto &renderPassInfo = m_pRenderPassInfo[renderPass->id];
  renderPassInfo.dimensions.width = extent.width;
  renderPassInfo.dimensions.height = extent.height;

  renderPass->renderPassChain.Extent = extent;

  for (size_t i = 0; i < renderPassInfo.attachments.size(); i++) {
    auto &attachment = renderPassInfo.attachments[i];

    vk::VulkanTextureInfo texInfo;
    texInfo.format = attachment.isDepthAttachment ? findDepthFormat(*m_pVkDataPtr) : resolveFormat(renderPassInfo.attachments[i].format);

    texInfo.addressMode = resolveWrapping(attachment.wrapMode);
    texInfo.compareOp = resolveCompareOp(attachment.compareFunc);
    texInfo.filter = resolveFilter(attachment.filtering);
    texInfo.width = renderPassInfo.dimensions.width;
    texInfo.height = renderPassInfo.dimensions.height;
    texInfo.num_channels = attachment.isDepthAttachment ? 1 : resolveNumChannels(attachment.format);
    texInfo.usage = (attachment.isDepthAttachment || texInfo.format == VK_FORMAT_D32_SFLOAT)
                        ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
                        : VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

    texInfo.compareEnable = attachment.depthCompare;

    vk::VulkanTexture texture = pCreateTextureBuffer(texInfo);

    texture.bindingType = vk::RENDER_BUFFER_SAMPLER;
    texture.info.mipLevels = 1;
    createImageView(*m_pVkDataPtr, texture,
                    (attachment.isDepthAttachment || texInfo.format == VK_FORMAT_D32_SFLOAT) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

    texture.imageLayout = attachment.isDepthAttachment ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    texture.imageLayout = attachment.storageImage ? VK_IMAGE_LAYOUT_GENERAL : texture.imageLayout;
    if (attachment.isSampler) {
      vk::createTextureSampler(*m_pVkDataPtr, texture);
    }

    texture.name = attachment.name;

    if (attachment.isDepthAttachment) {
      texture.isDepthAttachment = true;
      renderPass->renderPassChain.DepthTexture = texture;
      if (attachment.isSampler) {
        renderPass->renderPassChain.DepthTexture.index = i;
      }

      if (attachment.isSampler) {
        renderPass->renderPassChain.hasDepthSampler = false;
        renderPass->renderPassChain.ImageViews.at(i) = texture.imageView;
        renderPass->renderPassChain.Textures.at(i) = texture;
      }
    } else {
      renderPass->renderPassChain.ImageViews.at(i) = texture.imageView;
      renderPass->renderPassChain.Textures.at(i) = texture;
    }
  }

  vk::createFramebuffers(*m_pVkDataPtr, *renderPass, renderPass->renderPassChain);

  VkViewport viewport = {.x = 0,
                         .y = 0,
                         .width = (float)renderPass->renderPassChain.Extent.width,
                         .height = (float)renderPass->renderPassChain.Extent.height,
                         .minDepth = 0.0f,
                         .maxDepth = 1.0f};

  VkRect2D rect;
  rect.extent.width = renderPass->renderPassChain.Extent.width;
  rect.extent.height = renderPass->renderPassChain.Extent.height;

  rect.offset.x = 0;
  rect.offset.y = 0;

  renderPass->renderPassChain.setViewport(viewport);
  renderPass->renderPassChain.setScissor(rect);
}

void VulkanResourceManager::PatchMesh(Ref<Mesh> meshRef, UpdateMeshInfo updateInfo) {

  auto mesh = meshPool.get(meshRef);
  auto vertexBuffer = vertexBufferPool.get(mesh->vertexBuffer);
  auto vertexStagingBuffer = pCreateStagingBuffer(updateInfo.vertices);

  vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, vertexStagingBuffer.buffer, vertexBuffer->buffer, updateInfo.vertices.size() * sizeof(common::Vertex),
                 (mesh->vertexOffset + updateInfo.vertex_offset) * sizeof(common::Vertex));

  vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);
}

void VulkanResourceManager::UpdateMesh(Ref<Mesh> meshRef, UpdateMeshInfo updateInfo) {
  auto mesh = meshPool.get(meshRef);

  if (updateInfo.vertex_size > 0)
    mesh->numVertices = updateInfo.vertex_size;

  if (updateInfo.vertex_size > 0)
    mesh->vertexOffset = updateInfo.vertex_offset;

  if (!updateInfo.vertexBuffer.empty())
    mesh->vertexBuffer = updateInfo.vertexBuffer;

  /*
                  if (updateInfo.index_offset >= mesh->numIndices)
                  {
                          Log::Error("Mesh index offset is bigger than buffer's index capacity\n\t", "buffer size: ", mesh->numIndices);
                          // throw std::out_of_range(std::string(__PRETTY_FUNCTION__ ": index offset out of range"));
                  }

                  if (updateInfo.vertex_offset >= mesh->numVertices)
                  {
                          Log::Error("Mesh vertex offset is bigger than buffer's vertex capacity\n\t", "buffer size: ", mesh->numVertices);
                          // throw std::out_of_range(std::string(__PRETTY_FUNCTION__ ": vertex offset out of range"));
                  }

                  if (updateInfo.vertex_offset + updateInfo.vertex_size >= mesh->numVertices)
                  {
                          Log::Error("Trying to allocate beyond vertex buffer's capacity\n\t", "buffer size: ", mesh->numVertices);
                          // throw std::out_of_range(std::string(__PRETTY_FUNCTION__ ": vertex allocation out of range"));
                  }

                  if (updateInfo.index_offset + updateInfo.index_size >= mesh->numIndices)
                  {
                          Log::Error("Trying to allocate beyond index buffer's capacity\n\t", "buffer size: ", mesh->numVertices);
                          // throw std::out_of_range(std::string(__PRETTY_FUNCTION__ ": index allocation out of range"));
                  }

                  auto &vertices = updateInfo.vertices;
                  auto &indices = updateInfo.indices;

                  auto vertexBuffer = vertexBufferPool.get(mesh->vertexBuffer);
                  auto indexBuffer = indexBufferPool.get(mesh->indexBuffer);

                  size_t vertexOffset = vertexBuffer->offset + updateInfo.vertex_offset;
                  size_t indexOffset = indexBuffer->offset + updateInfo.index_offset;

                  auto vertexStagingBuffer = pCreateStagingBuffer(vertices);
                  auto indexStagingBuffer = pCreateStagingIndexBuffer(indices);

                  vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, vertexStagingBuffer.buffer, vertexBuffer->buffer,
                                             updateInfo.vertex_offset * sizeof(common::Vertex), vertexOffset);

                  vk::copyBuffer(*m_pVkDataPtr, m_pTransferCommandPool, indexStagingBuffer.buffer, indexBuffer->buffer,
                                             updateInfo.index_size * sizeof(IndexType), indexOffset);

                  vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);
                  vmaDestroyBuffer(m_pAllocator, indexStagingBuffer.buffer, indexStagingBuffer.allocation);
                  */
}

Ref<Texture> VulkanResourceManager::GetRenderTarget(Ref<RenderPass> renderPassRef, uint32_t index) {
  vk::VulkanRenderPass *renderPass = renderPassPool.get(renderPassRef);

  if (index >= renderPass->numAttachments) {
    return Ref<Texture>::makeEmpty();
  }
return Ref<Texture>::makeEmpty();
 // return renderPass->renderPassChain.Textures.at(index).;
}

void VulkanResourceManager::CopyTexture(Ref<Texture> srcRef, Ref<Texture> dstRef) {}
} // namespace engine