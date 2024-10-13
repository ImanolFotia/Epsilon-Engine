#include "core/framework/window.hpp"
#include <core/engine/context.hpp>
#include <core/framework/clock.hpp>
#include <vulkan/vulkan_core.h>

#include "imgui/imgui_setup.hpp"
#include "resource_manager.hpp"
#include "vulkan.hpp"

#include "helpers.hpp"

/**
 * @brief Implementation of the Vulkan renderer public API
 *
 */

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 1

#if 0

#define VMA_DEBUG_LOG_FORMAT(format, ...)

#define VMA_DEBUG_LOG_FORMAT(format, ...)                                                                                                                      \
  do {                                                                                                                                                         \
    printf((format), __VA_ARGS__);                                                                                                                             \
    printf("\n");                                                                                                                                              \
  } while (false)

#define VMA_DEBUG_LOG(str) VMA_DEBUG_LOG_FORMAT("%s", (str))

#endif

// #define VMA_VULKAN_VERSION 1001000
#if defined(ANDROID) || defined(__ANDROID__)

#include <android/log.h>

#define LOG_TAG "Epsilon"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

#include "vk_mem_alloc.h"

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

namespace engine {

VulkanRenderer::VulkanRenderer() {}

VulkanRenderer::~VulkanRenderer() {}

void VulkanRenderer::Init(const char *appName, framework::Window &window) {
  m_pWindow = &window;
  m_pResourceManagerRef->m_pVkDataPtr = &m_pVkData;

  m_pFrame.SetSize(m_pWindow->getSize().width, m_pWindow->getSize().height);

  Log::Info("Initiating Vulkan Context");

#if defined(ANDROID) || defined(__ANDROID__)
  LOGI("Initiating Vulkan Instance\n");
#endif
  vk::createInstance(appName, m_pVkData);

#if defined(ANDROID) || defined(__ANDROID__)
  LOGI("Initiating Vulkan Debug Messenger\n");
#endif
  vk::setupDebugMessenger(m_pVkData.instance);

#if defined(ANDROID) || defined(__ANDROID__)
  LOGI("Initiating Vulkan Surface\n");
#endif
  vk::createSurface(m_pVkData, window.getWindow());
  vk::pickPhysicalDevice(m_pVkData);
  vk::createLogicalDevice(m_pVkData);

#if defined(ANDROID) || defined(__ANDROID__)
  LOGI("Initiating Vulkan swapchain\n");
#endif
  vk::createSwapChain(m_pVkData, window.getWindow());
  vk::createImageViews(m_pVkData);

  m_pVkData.m_pCommandPools.emplace_back();
  vk::createCommandPool(m_pVkData, m_pVkData.m_pCommandPools.back());

  m_pVkData.m_pCommandPools.emplace_back();
  vk::createComputeCommandPool(m_pVkData, m_pVkData.m_pCommandPools.back());

#if defined(ANDROID) || defined(__ANDROID__)
  LOGI("Initiating Vulkan Descriptor Pool\n");
#endif
  m_pResourceManagerRef->pCreateDescriptorPool();

  vk::createCommandBuffers(m_pVkData, m_pVkData.m_pCommandPools.at(0), m_pVkData.m_pCommandBuffers);
  vk::createCommandBuffers(m_pVkData, m_pVkData.m_pCommandPools.at(1), m_pVkData.m_pComputeCommandBuffers);

  vk::createSyncObjects(m_pVkData);

  m_pCurrentCommandQueue.resize(MAX_COMMAND_QUEUE_SIZE);

  m_pImguiRenderer = std::make_shared<ImGuiRenderer>();
  m_pImguiRenderer->setResourceManager(m_pResourceManagerRef.get());
}

engine::Renderer::ObjectDataId VulkanRenderer::RegisterMesh(const std::vector<common::Vertex> &, std::vector<IndexType> &indices, bool) {
  throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
}

engine::Renderer::TexturesDataId VulkanRenderer::RegisterTexture(TextureCreationInfo) { throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__); }

Material VulkanRenderer::CreateMaterial(Ref<Material>) { throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__); }

void VulkanRenderer::Push(ObjectData object) {
  DrawCommand drawCommand;
  if (!object.mesh.empty()) {
    auto mesh = m_pResourceManagerRef->meshPool.get(object.mesh);
    drawCommand.meshResource.indexBuffer = mesh->indexBuffer;
    drawCommand.meshResource.indexOffset = mesh->indexOffset;
    drawCommand.meshResource.numIndices = mesh->numIndices;
    drawCommand.meshResource.numVertices = mesh->numVertices;
    drawCommand.meshResource.vertexBuffer = mesh->vertexBuffer;
    drawCommand.meshResource.vertexOffset = mesh->vertexOffset;
  } else {
    drawCommand.meshResource.numIndices = 6;
    drawCommand.meshResource.numVertices = 4;
  }
  drawCommand.uniformIndex = object.uniformIndex;
  drawCommand.layoutIndex = object.layout_index;
  drawCommand.pushConstantData = object.objectConstant;
  drawCommand.count = object.count;

  drawCommand.material = object.material;
  // auto pushConstant = m_pResourceManagerRef->pushConstantPool.get(object.pushConstant);
  /*if (pushConstant != nullptr)
  {
          drawCommand.objectData = pushConstant->data;
          drawCommand.object_data_size = pushConstant->size;
  }*/
  // if (m_pCurrentCommandQueue.size() <= currentCommandsInQueue)
  //	m_pCurrentCommandQueue.push_back(drawCommand);
  // else
  m_pCurrentCommandQueue[currentCommandsInQueue] = drawCommand;
  currentCommandsInQueue++;
}

void VulkanRenderer::BeginFrame() {
  m_pImageIndex = pPrepareSyncObjects();
  vmaSetCurrentFrameIndex(m_pResourceManagerRef->m_pAllocator, m_pCurrentFrame);
  m_pVkData.frame_index = m_pCurrentFrame;

  if (m_pImageIndex == -1)
    return;

  m_pFrame.FrameIndex(m_pCurrentFrame);

  m_pFrame.CommandBuffer(&m_pVkData.m_pCommandBuffers.at(m_pCurrentFrame));
  m_pFrame.ComputeCommandBuffer(&m_pVkData.m_pComputeCommandBuffers.at(m_pCurrentFrame));

  m_pFrame.SyncObjects(&m_pVkData.syncObjects.at(m_pCurrentFrame));

  vkResetCommandBuffer(m_pFrame.CommandBuffer(), 0);
  vkResetCommandBuffer(m_pFrame.ComputeCommandBuffer(), 0);

#if !defined(__ANDROID__)

  m_pImguiRenderer->newFrame(m_pCurrentFrame, m_pVkData.m_pCommandBuffers.at(m_pCurrentFrame));
#endif
}

void VulkanRenderer::InitDebugRenderer() {
  if (!imguiInit) {
    m_pImguiRenderer->Init(m_pVkData, m_pWindow->getWindow(), m_pResourceManagerRef->m_pDescriptorPool, m_pVkData.defaultRenderPass.renderPass,
                           m_pResourceManagerRef->m_pCommandPools.at(0), m_pVkData.m_pCommandBuffers.at(m_pCurrentFrame));
    for (auto &renderPass : m_pResourceManagerRef->renderPassPool) {
      int image_index = 0;
      for (auto &texture : renderPass.renderPassChain.Textures) {
        if (texture.info.isSampler) {
          m_pImguiRenderer->addTexture(renderPass.name, image_index);
        }
        image_index++;
      }
    }
    imguiInit = true;
  }
}

void VulkanRenderer::ComputeDispatchAsync(Ref<ComputeShader> computeShaderRef, Ref<BindGroup> bindGroup) {
  m_pFrame.AddComputeDispatch();
  vk::VulkanComputeShader *computeShader = m_pResourceManagerRef->computeShaderPool.get(computeShaderRef);

  if (computeShader->hasImageBarrier || computeShader->hasBufferMemoryBarrier) {

    VkDependencyInfo dependencyInfo = {.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                                       .pNext = nullptr,
                                       .memoryBarrierCount = 0,
                                       .pMemoryBarriers = nullptr,
                                       .bufferMemoryBarrierCount = (uint32_t)computeShader->bufferMemoryBarrier.size(),
                                       .pBufferMemoryBarriers = computeShader->bufferMemoryBarrier.data(),
                                       .imageMemoryBarrierCount = (uint32_t)computeShader->imageMemoryBarrier.size(),
                                       .pImageMemoryBarriers = computeShader->imageMemoryBarrier.data()};

    vkCmdPipelineBarrier2(m_pFrame.ComputeCommandBuffer(), &dependencyInfo);
  }

  vkCmdBindPipeline(m_pFrame.ComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computeShader->pipeline.computePipeline);

  auto bg = m_pResourceManagerRef->materialPool.get(bindGroup);

  vkCmdBindDescriptorSets(m_pFrame.ComputeCommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computeShader->pipeline.pipelineLayout, 0, 1,
                          &bg->descriptorSets[m_pCurrentFrame], 0, 0);

  vkCmdDispatch(m_pFrame.ComputeCommandBuffer(), computeShader->groupCountX, computeShader->groupCountY, computeShader->groupCountZ);
}

void VulkanRenderer::ComputeDispatch(Ref<ComputeShader> computeShaderRef, Ref<BindGroup> bindGroup) {

  if (m_pRenderPassActive) {
    vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
    m_pRenderPassActive = false;
  }
  //m_pFrame.AddComputeDispatch();
  vk::VulkanComputeShader *computeShader = m_pResourceManagerRef->computeShaderPool.get(computeShaderRef);

  if (computeShader->hasImageBarrier || computeShader->hasBufferMemoryBarrier) {

    VkDependencyInfo dependencyInfo = {.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2,
                                       .pNext = nullptr,
                                       .memoryBarrierCount = 0,
                                       .pMemoryBarriers = nullptr,
                                       .bufferMemoryBarrierCount = (uint32_t)computeShader->bufferMemoryBarrier.size(),
                                       .pBufferMemoryBarriers = computeShader->bufferMemoryBarrier.data(),
                                       .imageMemoryBarrierCount = (uint32_t)computeShader->imageMemoryBarrier.size(),
                                       .pImageMemoryBarriers = computeShader->imageMemoryBarrier.data()};

    vkCmdPipelineBarrier2(m_pFrame.CommandBuffer(), &dependencyInfo);
  }

  vkCmdBindPipeline(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computeShader->pipeline.computePipeline);

  auto bg = m_pResourceManagerRef->materialPool.get(bindGroup);

  vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_COMPUTE, computeShader->pipeline.pipelineLayout, 0, 1,
                          &bg->descriptorSets[m_pCurrentFrame], 0, 0);

  vkCmdDispatch(m_pFrame.CommandBuffer(), computeShader->groupCountX, computeShader->groupCountY, computeShader->groupCountZ);
}

void VulkanRenderer::BeginCompute() {

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  if (vkBeginCommandBuffer(m_pFrame.ComputeCommandBuffer(), &beginInfo) != VK_SUCCESS) {
    throw std::runtime_error("failed to begin recording command buffer!");
  }
}

void VulkanRenderer::EndCompute() {
  if (vkEndCommandBuffer(m_pFrame.ComputeCommandBuffer()) != VK_SUCCESS) {
    throw std::runtime_error("failed to record command buffer!");
  }
}

void VulkanRenderer::Begin() { vk::recordCommandBuffer(m_pFrame.CommandBuffer(), m_pImageIndex); }

void VulkanRenderer::Submit() {
  VkSemaphore signalSemaphores[] = {m_pFrame.SyncObjects().renderFinishedSemaphores};

  if (m_pFrame.ComputeDispatches() > 0)
    vk::SyncCompute(m_pVkData, m_pFrame.ComputeCommandBuffer(), m_pCurrentFrame);

  vk::Sync(m_pVkData, m_pFrame.CommandBuffer(), m_pCurrentFrame, m_pFrame.ComputeDispatches() > 0);

  m_pShouldRecreateSwapchain |= vk::Present(m_pVkData, signalSemaphores, m_pImageIndex);

  m_pShouldRecreateSwapchain |= m_pFrame.SetSize(m_pWindow->getSize().width, m_pWindow->getSize().height);

  if (m_pShouldRecreateSwapchain) {

    if (m_pWindow->getSize().width > 0) {
      pRecreateSwapChain();
      m_pShouldRecreateSwapchain = false;
      Log::Info("swap chain recreated");
    }
  }

  m_pCurrentFrame = m_pImageIndex;
  m_pNumDrawCalls = 0;
  m_pNumVertices = 0;

  m_pFrame.ResetComputeDispatches();

  m_pCurrentFrame = (m_pCurrentFrame + 1) % vk::MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderer::EndFrame() {}

void VulkanRenderer::End(glm::vec3 &v) {
#if !defined(__ANDROID__)
  std::vector<VmaBudget> budgets;
  budgets.resize(m_pVkData.max_memory_heaps);
  vmaGetHeapBudgets(m_pResourceManagerRef->m_pAllocator, budgets.data());
  m_pResourceManagerRef->ResourcesMemory.reset();
  m_pResourceManagerRef->ResourcesMemory.heaps.resize(m_pVkData.max_memory_heaps);
  for (int i = 0; i < m_pVkData.max_memory_heaps; i++) {
    m_pResourceManagerRef->ResourcesMemory.heaps.at(i).used_memory = budgets.at(i).usage;
    m_pResourceManagerRef->ResourcesMemory.heaps.at(i).total_memory = budgets.at(i).budget;
    m_pResourceManagerRef->ResourcesMemory.heaps.at(i).free_memory = budgets.at(i).budget - budgets.at(i).usage;
  }
  m_pResourceManagerRef->ResourcesMemory.numDrawCalls = m_pNumDrawCalls;
  m_pResourceManagerRef->ResourcesMemory.numVertices = m_pNumVertices;

  auto renderPass = m_pResourceManagerRef->getRenderPass(m_pActiveRenderPass);

  if (renderPass->id != std::numeric_limits<uint32_t>::max() || !m_pRenderPassActive) {

    if (m_pRenderPassActive) {
      vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
      m_pRenderPassActive = false;
    }

    vk::createRenderPassInfo(m_pImageIndex, m_pVkData, m_pVkData.defaultRenderPass);

    auto renderPass = m_pVkData.defaultRenderPass;
    m_pVkData.defaultRenderPass.renderPassInfo.renderArea.offset = {0, 0};
    m_pVkData.defaultRenderPass.renderPassInfo.renderArea.extent = m_pVkData.defaultRenderPass.renderPassChain.Extent;
    m_pVkData.defaultRenderPass.renderPassChain.Extent = m_pVkData.defaultRenderPass.renderPassChain.Extent;

    vk::beginRenderPass(m_pFrame.CommandBuffer(), m_pVkData.defaultRenderPass);
    m_pRenderPassActive = true;
  }
  m_pImguiRenderer->DrawUI(std::forward<glm::vec3 &>(v), m_pResourceManagerRef->ResourcesMemory);
#endif
  if (m_pRenderPassActive) {
    vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
    m_pRenderPassActive = false;
  }
  if (m_pImageIndex == -1)
    return;

  vk::endRecording(m_pFrame.CommandBuffer());
}

void VulkanRenderer::Flush(FlushCommand command) {
  if (m_pImageIndex == -1)
    return;

  auto renderPass = m_pResourceManagerRef->getRenderPass(command.renderPassRef);

  if (!Ref<RenderPass>::isSame(m_pActiveRenderPass, command.renderPassRef) || !m_pRenderPassActive) {

    if (m_pRenderPassActive) {
      vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
      m_pRenderPassActive = false;
    }

    if (renderPass->id == std::numeric_limits<uint32_t>::max()) {
      vk::createRenderPassInfo(m_pImageIndex, m_pVkData, m_pVkData.defaultRenderPass);

      m_pVkData.defaultRenderPass.renderPassInfo.renderArea.offset = {0, 0};
      m_pVkData.defaultRenderPass.renderPassInfo.renderArea.extent = m_pVkData.defaultRenderPass.renderPassChain.Extent;
      renderPass->renderPassChain.Extent = m_pVkData.defaultRenderPass.renderPassChain.Extent;

      vk::beginRenderPass(m_pFrame.CommandBuffer(), m_pVkData.defaultRenderPass);
      m_pRenderPassActive = true;
      m_pActiveRenderPass = command.renderPassRef;
    } else {
      vk::createRenderPassInfo(m_pImageIndex, m_pVkData, *renderPass);
      renderPass->renderPassInfo.renderArea.offset = {0, 0};
      renderPass->renderPassInfo.renderArea.extent = renderPass->renderPassChain.Extent;
      vk::beginRenderPass(m_pFrame.CommandBuffer(), *renderPass);
      m_pRenderPassActive = true;
      m_pActiveRenderPass = command.renderPassRef;
    }

    if(command.clean_attachments) {
      VkExtent2D extent = renderPass->renderPassChain.Extent;

      std::vector<VkClearAttachment> clearAttachments;
      std::vector<VkClearRect> rects;
      for(int i = 0; i < renderPass->renderPassData.colorAttachments.size(); i++) {
        if(renderPass->renderPassData.colorAttachments[i].format == VK_FORMAT_D32_SFLOAT || 
        renderPass->renderPassData.colorAttachments[i].format == VK_FORMAT_D32_SFLOAT_S8_UINT || 
        renderPass->renderPassData.colorAttachments[i].format == VK_FORMAT_D16_UNORM_S8_UINT || 
        renderPass->renderPassData.colorAttachments[i].format == VK_FORMAT_D16_UNORM) break;

        rects.emplace_back();
        auto &attachment = clearAttachments.emplace_back();
        attachment.clearValue.color = renderPass->clearValues[i].color;
        attachment.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        attachment.colorAttachment = i;
      }

      if(renderPass->renderPassData.hasDepthAttachment) {
        auto &attachment = clearAttachments.emplace_back();
        rects.emplace_back();
        attachment.clearValue.depthStencil = renderPass->depthStencilClearColor;
        attachment.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
      }

      for(auto &rect: rects) {
        rect.rect = {0, 0, extent.width, extent.height};
        rect.baseArrayLayer = 0;
        rect.layerCount = 1;
      }

      vkCmdClearAttachments(
        m_pFrame.CommandBuffer(), 
        clearAttachments.size(), 
        clearAttachments.data(), 
        rects.size(), 
        rects.data());
    }
  }

  switch (command.type) {
  case DrawType::NON_INDEXED:
    FlushNonIndexed(renderPass);
    break;

  case DrawType::INDEXED:
    FlushIndexed(renderPass);
    break;

  case DrawType::INDIRECT:
    FlushIndirect(renderPass);
    break;

  case DrawType::INDEXED_INDIRECT:
    FlushIndexedIndirect(renderPass);
    break;
  default:
    Log::Error("Error: Draw command type not supported!");
    exit(99);
    break;
  }

  currentCommandsInQueue = 0;
}

void VulkanRenderer::Flush(engine::Ref<engine::RenderPass> renderPassRef, engine::DrawType type) {
  if (m_pImageIndex == -1)
    return;

  auto renderPass = m_pResourceManagerRef->getRenderPass(renderPassRef);

  if (!Ref<RenderPass>::isSame(m_pActiveRenderPass, renderPassRef) || !m_pRenderPassActive) {

    if (m_pRenderPassActive) {
      vk::endRenderPass(m_pFrame.CommandBuffer(), m_pVkData);
      m_pRenderPassActive = false;
    }

    if (renderPass->id == std::numeric_limits<uint32_t>::max()) {
      vk::createRenderPassInfo(m_pImageIndex, m_pVkData, m_pVkData.defaultRenderPass);

      m_pVkData.defaultRenderPass.renderPassInfo.renderArea.offset = {0, 0};
      m_pVkData.defaultRenderPass.renderPassInfo.renderArea.extent = m_pVkData.defaultRenderPass.renderPassChain.Extent;
      renderPass->renderPassChain.Extent = m_pVkData.defaultRenderPass.renderPassChain.Extent;

      vk::beginRenderPass(m_pFrame.CommandBuffer(), m_pVkData.defaultRenderPass);
      m_pRenderPassActive = true;
      m_pActiveRenderPass = renderPassRef;
    } else {
      vk::createRenderPassInfo(m_pImageIndex, m_pVkData, *renderPass);
      renderPass->renderPassInfo.renderArea.offset = {0, 0};
      renderPass->renderPassInfo.renderArea.extent = renderPass->renderPassChain.Extent;
      vk::beginRenderPass(m_pFrame.CommandBuffer(), *renderPass);
      m_pRenderPassActive = true;
      m_pActiveRenderPass = renderPassRef;
    }

  }

  switch (type) {
  case DrawType::NON_INDEXED:
    FlushNonIndexed(renderPass);
    break;

  case DrawType::INDEXED:
    FlushIndexed(renderPass);
    break;

  case DrawType::INDIRECT:
    FlushIndirect(renderPass);
    break;

  case DrawType::INDEXED_INDIRECT:
    FlushIndexedIndirect(renderPass);
    break;
  default:
    Log::Error("Error: Draw command type not supported!");
    exit(99);
    break;
  }

  currentCommandsInQueue = 0;
}

void VulkanRenderer::FlushIndexed(vk::VulkanRenderPass *renderPass) {

  int32_t prev_layout = -1;
  vk::VulkanBuffer *prev_vertex_buffer = nullptr;
  vk::VulkanBuffer *prev_index_buffer = nullptr;
  int32_t prev_vertex_buffer_id = -1;
  int32_t prev_index_buffer_id = -1;

  vk::VulkanMaterial *prev_material = nullptr;
  int32_t prev_material_id = -1;

  int changed = 0;

  VkExtent2D extent = renderPass->renderPassChain.Extent;
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(m_pFrame.CommandBuffer(), 0, 1, &renderPass->renderPassChain.Viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  vkCmdSetScissor(m_pFrame.CommandBuffer(), 0, 1, &renderPass->renderPassChain.Scissor);

  for (int i = 0; i < currentCommandsInQueue; i++) {

    auto &command = m_pCurrentCommandQueue[i];
    if (prev_layout != command.layoutIndex) {
      vk::bindPipeline(renderPass->renderPipelines[command.layoutIndex].graphicsPipeline, m_pFrame.CommandBuffer());
      prev_layout = command.layoutIndex;

      if (m_pVkData.bindless_supported) {
        vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                renderPass->renderPipelines[command.layoutIndex].pipelineLayout.back(), 1, 1, &m_pResourceManagerRef->m_pGlobalDescriptorSets,
                                0, nullptr);
      }
    }

    if (prev_vertex_buffer_id != command.meshResource.vertexBuffer.Id() && command.meshResource.vertexBuffer.Id() != -1) {
      auto vertexBuffer = m_pResourceManagerRef->vertexBufferPool.get(command.meshResource.vertexBuffer);
      prev_vertex_buffer_id = command.meshResource.vertexBuffer.Id();
      prev_vertex_buffer = vertexBuffer;
      vk::bindVertexBuffer(m_pVkData, m_pFrame.CommandBuffer(), vertexBuffer->buffer);
    }

    if (prev_index_buffer_id != command.meshResource.indexBuffer.Id() && command.meshResource.indexBuffer.Id() != -1) {
      auto indexBuffer = m_pResourceManagerRef->indexBufferPool.get(command.meshResource.indexBuffer);
      vkCmdBindIndexBuffer(m_pFrame.CommandBuffer(), indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
      prev_index_buffer_id = command.meshResource.indexBuffer.Id();
      prev_index_buffer = indexBuffer;
    }

    if (prev_material_id != command.material.Id()) {
      auto material = m_pResourceManagerRef->materialPool.get(command.material);
      prev_material_id = command.material.Id();
      prev_material = material;
      vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->renderPipelines[command.layoutIndex].pipelineLayout.back(),
                              0, 1, &material->descriptorSets[m_pCurrentFrame], 0, nullptr);
    }

    /** TODO:
     *  Find a way to cleanly implement push constants*/
    vkCmdPushConstants(m_pFrame.CommandBuffer(), renderPass->renderPipelines[command.layoutIndex].pipelineLayout.back(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(ObjectDataConstant), &command.pushConstantData);

    vkCmdDrawIndexed(m_pFrame.CommandBuffer(), command.meshResource.numIndices, command.count, command.meshResource.indexOffset,
                     command.meshResource.vertexOffset, command.uniformIndex);
    m_pNumDrawCalls++;
    m_pNumVertices += command.meshResource.numVertices * command.count;
  }
}

void VulkanRenderer::FlushNonIndexed(vk::VulkanRenderPass *renderPass) {

  int32_t prev_layout = -1;
  vk::VulkanBuffer *prev_vertex_buffer = nullptr;
  vk::VulkanBuffer *prev_index_buffer = nullptr;
  int32_t prev_vertex_buffer_id = -1;
  int32_t prev_index_buffer_id = -1;

  vk::VulkanMaterial *prev_material = nullptr;
  int32_t prev_material_id = -1;

  int changed = 0;

  VkExtent2D extent = renderPass->renderPassChain.Extent;
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(m_pFrame.CommandBuffer(), 0, 1, &renderPass->renderPassChain.Viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  vkCmdSetScissor(m_pFrame.CommandBuffer(), 0, 1, &renderPass->renderPassChain.Scissor);

  for (int i = 0; i < currentCommandsInQueue; i++) {

    auto &command = m_pCurrentCommandQueue[i];
    if (prev_layout != command.layoutIndex) {
      vk::bindPipeline(renderPass->renderPipelines[command.layoutIndex].graphicsPipeline, m_pFrame.CommandBuffer());
      prev_layout = command.layoutIndex;

      if (m_pVkData.bindless_supported) {
        vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                renderPass->renderPipelines[command.layoutIndex].pipelineLayout.back(), 1, 1, &m_pResourceManagerRef->m_pGlobalDescriptorSets,
                                0, nullptr);
      }
    }

    if (prev_vertex_buffer_id != command.meshResource.vertexBuffer.Id() && command.meshResource.vertexBuffer.Id() != -1) {
      auto vertexBuffer = m_pResourceManagerRef->vertexBufferPool.get(command.meshResource.vertexBuffer);
      prev_vertex_buffer_id = command.meshResource.vertexBuffer.Id();
      prev_vertex_buffer = vertexBuffer;
      vk::bindVertexBuffer(m_pVkData, m_pFrame.CommandBuffer(), vertexBuffer->buffer);
    }

    if (prev_material_id != command.material.Id()) {
      auto material = m_pResourceManagerRef->materialPool.get(command.material);
      prev_material_id = command.material.Id();
      prev_material = material;
      vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->renderPipelines[command.layoutIndex].pipelineLayout.back(),
                              0, 1, &material->descriptorSets[m_pCurrentFrame], 0, nullptr);
    }

    /** TODO:
     *  Find a way to cleanly implement push constants*/
    vkCmdPushConstants(m_pFrame.CommandBuffer(), renderPass->renderPipelines[command.layoutIndex].pipelineLayout.back(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(ObjectDataConstant), &command.pushConstantData);

    vkCmdDraw(m_pFrame.CommandBuffer(), command.meshResource.numVertices, command.count, command.meshResource.vertexOffset, command.uniformIndex);
    m_pNumDrawCalls++;
    m_pNumVertices += command.meshResource.numVertices * command.count;
  }
}

void VulkanRenderer::FlushIndirect(vk::VulkanRenderPass *renderPass) { throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__); }

void VulkanRenderer::FlushIndexedIndirect(vk::VulkanRenderPass *renderPass) {

  auto predicate = [](DrawCommand &a, DrawCommand &b) -> bool {
    auto a_mat = a.material.Index();
    auto b_mat = b.material.Index();
    auto a_vtx = a.meshResource.vertexBuffer.Index();
    auto b_vtx = b.meshResource.vertexBuffer.Index();
    auto a_i = a.meshResource.indexBuffer.Index();
    auto b_i = b.meshResource.indexBuffer.Index();
    return std::tie(a_mat, a.layoutIndex, a_vtx, a_i) < std::tie(b_mat, b.layoutIndex, b_vtx, b_i);
  };

  // std::sort(m_pCurrentCommandQueue.begin(), m_pCurrentCommandQueue.begin() + currentCommandsInQueue, predicate);

  batches = generateIndirectBatch(m_pCurrentCommandQueue, currentCommandsInQueue);

  void *data;
  vmaMapMemory(m_pResourceManagerRef->m_pAllocator, m_pResourceManagerRef->m_pIndirectBuffer[m_pCurrentFrame].allocation, &data);
  VkDrawIndexedIndirectCommand *indirect_commands = reinterpret_cast<VkDrawIndexedIndirectCommand *>(data);
  int i = 0;
  for (auto &command : m_pCurrentCommandQueue) {
    if (i >= currentCommandsInQueue)
      break;

    indirect_commands[i].firstIndex = command.meshResource.indexOffset;
    indirect_commands[i].firstInstance = command.uniformIndex;
    indirect_commands[i].indexCount = command.meshResource.numIndices;
    indirect_commands[i].vertexOffset = command.meshResource.vertexOffset;
    indirect_commands[i].instanceCount = command.count;
    i++;
  }
  uint32_t prev_layout = -1;
  uint32_t prev_vertexBuffer = -1;
  uint32_t prev_indexBuffer = -1;

  VkExtent2D extent = renderPass->renderPassChain.Extent;
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float)extent.width;
  viewport.height = (float)extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(m_pFrame.CommandBuffer(), 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = extent;
  vkCmdSetScissor(m_pFrame.CommandBuffer(), 0, 1, &scissor);

  for (auto &batch : batches) {
    auto material = m_pResourceManagerRef->materialPool.get(batch.material);
    auto vertexBuffer = m_pResourceManagerRef->vertexBufferPool.get(batch.meshResource.vertexBuffer);
    auto indexBuffer = m_pResourceManagerRef->indexBufferPool.get(batch.meshResource.indexBuffer);

    if (prev_layout != batch.layoutIndex) {
      vk::bindPipeline(renderPass->renderPipelines[batch.layoutIndex].graphicsPipeline, m_pFrame.CommandBuffer());
      prev_layout = batch.layoutIndex;
    }

    if (prev_vertexBuffer != vertexBuffer->id) {
      vk::bindVertexBuffer(m_pVkData, m_pFrame.CommandBuffer(), vertexBuffer->buffer);
      prev_vertexBuffer = vertexBuffer->id;
    }
    if (prev_indexBuffer != indexBuffer->id) {
      vkCmdBindIndexBuffer(m_pFrame.CommandBuffer(), indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);
      prev_indexBuffer = indexBuffer->id;
    }

    if (-1 != batch.material.Id()) {
      vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->renderPipelines[batch.layoutIndex].pipelineLayout.back(),
                              0, 1, &material->descriptorSets[m_pCurrentFrame], 0, nullptr);
    }

    if (m_pVkData.bindless_supported) {
      vkCmdBindDescriptorSets(m_pFrame.CommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderPass->renderPipelines[batch.layoutIndex].pipelineLayout.back(),
                              1, 1, &m_pResourceManagerRef->m_pGlobalDescriptorSets, 0, nullptr);
    }
    constexpr uint32_t draw_stride = sizeof(VkDrawIndexedIndirectCommand);
    VkDeviceSize indirect_offset = batch.first * draw_stride;

    char push_constant[72] = {};

    vkCmdPushConstants(m_pFrame.CommandBuffer(), renderPass->renderPipelines[batch.layoutIndex].pipelineLayout.back(), VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(ObjectDataConstant), &push_constant);

    vkCmdDrawIndexedIndirect(m_pFrame.CommandBuffer(), m_pResourceManagerRef->m_pIndirectBuffer[m_pCurrentFrame].buffer, indirect_offset, batch.count,
                             draw_stride);

    m_pNumDrawCalls++;
    m_pNumVertices += batch.meshResource.numVertices * batch.count;
  }

  vmaUnmapMemory(m_pResourceManagerRef->m_pAllocator, m_pResourceManagerRef->m_pIndirectBuffer[m_pCurrentFrame].allocation);
}

int32_t VulkanRenderer::pPrepareSyncObjects() {

  // graphics sync

  vkWaitForFences(m_pVkData.logicalDevice, 1, &m_pVkData.syncObjects[m_pCurrentFrame].inFlightFences, VK_TRUE, UINT64_MAX);

  uint32_t imageIndex = 0;
  VkResult result = vkAcquireNextImageKHR(m_pVkData.logicalDevice, m_pVkData.swapChain, UINT64_MAX,
                                          m_pVkData.syncObjects[m_pCurrentFrame].imageAvailableSemaphores, VK_NULL_HANDLE, &imageIndex);

  if (result == VK_ERROR_OUT_OF_DATE_KHR) {
    if (m_pWindow->getSize().width > 0)
      pRecreateSwapChain();
    return -1;
  } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    throw std::runtime_error("failed to acquire swap chain image!");
  }

  // Only reset the fence if we are submitting work
  vkResetFences(m_pVkData.logicalDevice, 1, &m_pVkData.syncObjects[m_pCurrentFrame].inFlightFences);

  return imageIndex;
}

void VulkanRenderer::pUpdateUniforms(const vk::VulkanBuffer &buffer) {
  void *data;
  vmaMapMemory(m_pResourceManagerRef->m_pAllocator, buffer.allocation, &data);
  memcpy(data, perPassData, buffer.size);
  vmaUnmapMemory(m_pResourceManagerRef->m_pAllocator, buffer.allocation);
}

void VulkanRenderer::pUpdateUniformBuffer(const vk::VulkanBuffer &buffer, const void *newData) {
  void *data;
  vmaMapMemory(m_pResourceManagerRef->m_pAllocator, buffer.allocation, &data);
  memcpy(data, newData, buffer.size);
  vmaUnmapMemory(m_pResourceManagerRef->m_pAllocator, buffer.allocation);
}

void VulkanRenderer::Cleanup() {
  vkDeviceWaitIdle(m_pVkData.logicalDevice);
  m_pImguiRenderer->Destroy();
  m_pResourceManagerRef->clean();
}

void VulkanRenderer::SetVSync(bool state) {
  if (state != m_pVkData.vsync)
    m_pShouldRecreateSwapchain = true;
  m_pVkData.vsync = state;
}

void VulkanRenderer::UpdateRenderPassUniforms(Ref<RenderPass> renderPassRef, BindingIndex index, const void *data) {
  auto renderPass = m_pResourceManagerRef->renderPassPool.get(renderPassRef);
  auto front = renderPass->uniformBuffer.begin();

  std::advance(front, (uint32_t)index);
  auto &buffer = (*front).buffers[m_pCurrentFrame];
  pUpdateUniformBuffer(buffer, data);
}

void VulkanRenderer::pUpdateMaterial(vk::VulkanMaterial &) { throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__); }

void VulkanRenderer::pRecreateSwapChain() {
  vkDeviceWaitIdle(m_pVkData.logicalDevice);

  { cleanupSwapChain(m_pVkData); }

  vk::createSwapChain(m_pVkData, m_pWindow->getWindow());

  vk::createImageViews(m_pVkData);

  vk::createSwapChainFramebuffers(m_pVkData, m_pVkData.defaultRenderPass, m_pVkData.defaultRenderPass.renderPassChain);


  size_t id = std::hash<std::string>{}(m_pVkData.defaultRenderPass.name);
  auto pass = m_pResourceManagerRef->renderPassPool.get(id);

  VkViewport viewport = {.x = 0,
                         .y = 0,
                         .width = (float)m_pVkData.defaultRenderPass.renderPassChain.Extent.width,
                         .height = (float)m_pVkData.defaultRenderPass.renderPassChain.Extent.height,
                         .minDepth = 0.0f,
                         .maxDepth = 1.0f};

  VkRect2D rect;
  rect.extent.width = m_pVkData.defaultRenderPass.renderPassChain.Extent.width;
  rect.extent.height = m_pVkData.defaultRenderPass.renderPassChain.Extent.height;

  rect.offset.x = 0;
  rect.offset.y = 0;

  m_pVkData.defaultRenderPass.renderPassChain.setViewport(viewport);
  m_pVkData.defaultRenderPass.renderPassChain.setScissor(rect);

  pass->renderPassChain.setViewport(viewport);
  pass->renderPassChain.setScissor(rect);

  m_pResourceManagerRef->pRecreateFrameBuffers(rect.extent);

  m_pResourceManagerRef->pCreateDescriptorPool();
  m_pResourceManagerRef->pRecreateDescriptorSets();
  m_pImguiRenderer->recreateDescriptorSets();
}

void VulkanRenderer::SetViewport(const Viewport &viewport) {
  auto renderpass = m_pResourceManagerRef->renderPassPool.get(m_pActiveRenderPass);
  if (renderpass == nullptr)
    return;

  renderpass->renderPassChain.setViewport({.x = viewport.offset_x,
                                           .y = viewport.offset_y,
                                           .width = viewport.width,
                                           .height = viewport.height,
                                           .minDepth = viewport.min_depth,
                                           .maxDepth = viewport.max_depth});
}

void VulkanRenderer::SetScissor(const Scissor &scissor) {
  auto renderpass = m_pResourceManagerRef->renderPassPool.get(m_pActiveRenderPass);
  if (renderpass == nullptr)
    return;

  VkRect2D rect;
  rect.extent.width = scissor.width;
  rect.extent.height = scissor.height;

  rect.offset.x = scissor.offset_x;
  rect.offset.y = scissor.offset_y;

  renderpass->renderPassChain.setScissor(rect);
}

void VulkanRenderer::SetRenderPass(Ref<RenderPass> renderpass) {
  // m_pActiveRenderPass = renderpass;
}

std::shared_ptr<ImGuiRenderer> VulkanRenderer::getDebugRenderer() { return m_pImguiRenderer; }
} // namespace engine
