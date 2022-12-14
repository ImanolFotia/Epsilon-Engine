#include "resource_manager.hpp"

#include <framework/exception.hpp>


#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif


namespace engine
{
    VulkanResourceManager::VulkanResourceManager()
    {
    }

    VulkanResourceManager::~VulkanResourceManager()
    {
    }

    void VulkanResourceManager::Init()
    {

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_pVkDataPtr->physicalDevice;
        allocatorInfo.device = m_pVkDataPtr->logicalDevice;
        allocatorInfo.instance = m_pVkDataPtr->instance;

        vmaCreateAllocator(&allocatorInfo, &m_pAllocator);

        m_pCommandPools.emplace_back();

        vk::createCommandPool(*m_pVkDataPtr, m_pCommandPools.back());
    }

    Ref<Texture> VulkanResourceManager::createTexture(unsigned char *pixels, TextureInfo texInfo)
    {
        Ref<Texture> refTexture;

        auto stagingBuffer = pCreateStagingTextureBuffer(pixels, texInfo);

        auto texture = pCreateTextureBuffer({.width = texInfo.width,
                                             .height = texInfo.height,
                                             .num_channels = texInfo.numChannels});

        auto size = texInfo.width * texInfo.height * texInfo.numChannels;

        transitionImageLayout(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
        copyBufferToImage(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), stagingBuffer.buffer, texture.image, static_cast<uint32_t>(texInfo.width), static_cast<uint32_t>(texInfo.height));
        transitionImageLayout(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), texture.image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        vmaDestroyBuffer(m_pAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
        // vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);

        texture.format = VK_FORMAT_R8G8B8A8_SRGB;
        vk::createImageView(*m_pVkDataPtr, texture, VK_IMAGE_ASPECT_COLOR_BIT);
        vk::createTextureSampler(*m_pVkDataPtr, texture);

        auto ref = texPool.insert(texture);
        return ref;
    }

    Ref<Material> VulkanResourceManager::createMaterial(MaterialInfo material, Ref<RenderPass> renderPassRef)
    {

        try{
        vk::VulkanMaterial vkMaterial;
        auto renderPass = renderPassPool.get(renderPassRef);
        vkMaterial.descriptorSetLayout = renderPass->renderPipelines.back().descriptorSetLayout;
        //auto &materialdata = m_pMaterials.emplace_back();

        for (auto &texture : material.textures)
        {
            auto tex = createTexture(texture.pixels, texture);
            vkMaterial.textures.push_back(*texPool.get(tex)); 
        }

        //auto uniformBufferRef = createUniformData(material.bindingInfo);

        auto buffers = renderPass->uniformBuffer.buffers;

        for(int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
            vkMaterial.bufferInfo[i].offset = buffers[i].offset;
            vkMaterial.bufferInfo[i].buffer = buffers[i].buffer;
        }
        vkMaterial.bufferSize = renderPass->uniformBuffer.size;

        m_pNumCommandPools++;
        pCreateDescriptorPool();
        pRecreateDescriptorSets();
        pCreateDescriptorSets(vkMaterial);

        
        Ref<Material> materialRef = materialPool.insert(vkMaterial);
        return materialRef;
        }
        catch(std::exception& e) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
        }

    }

    void VulkanResourceManager::pRecreateSwapChain(GLFWwindow *window)
    {
        vkDeviceWaitIdle(m_pVkDataPtr->logicalDevice);

        for (auto &pass : renderPassPool)
        {
            cleanupSwapChain(*m_pVkDataPtr, pass);
        }

        vk::createSwapChain(*m_pVkDataPtr, window);

        vk::createImageViews(*m_pVkDataPtr);

        // Recreate the default renderpass
        vk::createRenderPass(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, m_pDefaultRenderPassInfo);

        for (auto i = 0; i < m_pVkDataPtr->defaultRenderPass.renderPipelines.size(); i++)
            vk::createGraphicsPipeline(*m_pVkDataPtr,
                                       m_pVkDataPtr->defaultRenderPass,
                                       m_pVkDataPtr->defaultRenderPass.renderPipelines[i],
                                       m_pVkDataPtr->defaultRenderPass.vertexInfo,
                                       m_pDefaultRenderPassInfo);

        vk::createFramebuffers(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, m_pVkDataPtr->defaultRenderPass.renderPassChain);

        for (auto &pass : renderPassPool)

        {
            if(pass.id == std::numeric_limits<uint32_t>::max()) continue;
            vk::createRenderPass(*m_pVkDataPtr, pass, m_pRenderPassInfo[pass.id]);

            for (auto i = 0; i < pass.renderPipelines.size(); i++)
                vk::createGraphicsPipeline(*m_pVkDataPtr, pass, pass.renderPipelines[i], pass.vertexInfo, m_pRenderPassInfo[pass.id]);

            vk::createFramebuffers(*m_pVkDataPtr, pass, pass.renderPassChain);
        }
    }

    Ref<RenderPass> VulkanResourceManager::createDefaultRenderPass(RenderPassInfo renderPassInfo) {


        m_pVkDataPtr->defaultRenderPass.renderPipelines.emplace_back();
        m_pVkDataPtr->defaultRenderPass.id = std::numeric_limits<uint32_t>::max();
        m_pDefaultRenderPassInfo = renderPassInfo;

        vk::createRenderPass(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, renderPassInfo);

        vk::VulkanVertexInfo vertexInfo;

        vertexInfo.attributeDescriptions =
                vk::getAttributeDescriptions(0, renderPassInfo.vertexLayout);

        vertexInfo.bindingDescription = vk::getBindingDescription(renderPassInfo.size);

        vk::createDescriptorSetLayout(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass.renderPipelines.back().descriptorSetLayout);

        vk::createGraphicsPipeline(*m_pVkDataPtr,
                                   m_pVkDataPtr->defaultRenderPass,
                                   m_pVkDataPtr->defaultRenderPass.renderPipelines.back(),
                                   vertexInfo,
                                   renderPassInfo);

        vk::createFramebuffers(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, m_pVkDataPtr->defaultRenderPass.renderPassChain);

        m_pVkDataPtr->defaultRenderPass.vertexInfo = vertexInfo;
        auto uniformRef = createUniformData(renderPassInfo.bindingInfo);
        m_pVkDataPtr->defaultRenderPass.uniformBuffer = *uniformBufferPool.get(uniformRef);
        m_pDefaultRenderPassRef = renderPassPool.insert(m_pVkDataPtr->defaultRenderPass);

        return m_pDefaultRenderPassRef;
    }

    Ref<RenderPass> VulkanResourceManager::createRenderPass(RenderPassInfo renderPassInfo)
    {
        vk::VulkanRenderPass renderPass = {};

        renderPass.renderPipelines.emplace_back();
        renderPass.id = m_pRenderPassCount;
        m_pRenderPassInfo.push_back(renderPassInfo);

        vk::createRenderPass(*m_pVkDataPtr, renderPass, renderPassInfo);

        vk::VulkanVertexInfo vertexInfo;

        vertexInfo.attributeDescriptions =
            vk::getAttributeDescriptions(0, renderPassInfo.vertexLayout);

        vertexInfo.bindingDescription = vk::getBindingDescription(renderPassInfo.size);

        vk::createDescriptorSetLayout(*m_pVkDataPtr, renderPass.renderPipelines.back().descriptorSetLayout);

        vk::createGraphicsPipeline(*m_pVkDataPtr,
                                                     renderPass,
                                                     renderPass.renderPipelines.back(),
                                                     vertexInfo,
                                                     renderPassInfo);

        vk::createFramebuffers(*m_pVkDataPtr, renderPass, renderPass.renderPassChain);

        renderPass.vertexInfo = vertexInfo;
        auto uniformRef = createUniformData(renderPassInfo.bindingInfo);
        renderPass.uniformBuffer = *uniformBufferPool.get(uniformRef);
        auto ref = renderPassPool.insert(renderPass);

        m_pRenderPassCount++;
        return ref;
    }

    void VulkanResourceManager::clean()
    {
        for (auto &buffer : vertexBufferPool)
        {
            vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
        }

        for (auto &buffer : indexBufferPool)
        {
            vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
        }

        vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);

        vk::cleanupSyncObjects(*m_pVkDataPtr);
        vk::cleanCommandPool(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools[0]);

        for (auto &texture : texPool)
        {

            vkDestroySampler(m_pVkDataPtr->logicalDevice, texture.sampler, nullptr);
            vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture.imageView, nullptr);
            vk::destroyImage(*m_pVkDataPtr, texture);
        }

        vmaDestroyAllocator(m_pAllocator);

        for (auto &pass : renderPassPool)
        {
            vk::cleanupRenderPass(*m_pVkDataPtr, pass.renderPass);
            for (auto &pipeline : pass.renderPipelines)
                vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);
        }

        vk::cleanup(*m_pVkDataPtr);
    }

    /**
     * Create functions
     */
    Ref<Buffer> VulkanResourceManager::destroyBuffer(BufferInfo)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    Ref<Shader> VulkanResourceManager::createShader(ShaderInfo)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    Ref<UniformBindings> VulkanResourceManager::createUniformData(UniformBindingInfo bindingInfo)
    {
        auto buffer = pCreateUniformBuffer(bindingInfo);
        return uniformBufferPool.insert(buffer);
        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    Ref<Mesh> VulkanResourceManager::createMesh(MeshInfo meshInfo)
    {
        std::vector<Vertex> *vertices = &meshInfo.vertices;
        std::vector<uint32_t> *indices = &meshInfo.indices;

        Ref<Buffer> vertexBufferRef = pFetchVertexBuffer(vertices->size());
        Ref<Buffer> indexBufferRef = pFetchIndexBuffer(indices->size());

        vk::VulkanBuffer *vertexBuffer = vertexBufferPool.get(vertexBufferRef);
        vk::VulkanBuffer *indexBuffer = indexBufferPool.get(indexBufferRef);

        auto vertexStagingBuffer = pCreateStagingBuffer(*vertices);
        auto indexStagingBuffer = pCreateStagingIndexBuffer(*indices);

        vk::copyBuffer(*m_pVkDataPtr, m_pCommandPools.back(), vertexStagingBuffer.buffer, vertexBuffer->buffer, vertices->size() * sizeof(Vertex), (vertexBuffer->allocatedVertices - vertices->size()) * sizeof(Vertex));

        IO::Info("From function ", __PRETTY_FUNCTION__, "\n\tin (", __FILE__, ":", __LINE__, ") \n\t", "copied ", vertices->size(), " vertices, of size ", vertices->size() * sizeof(Vertex), " bytes, at offset ", (vertexBuffer->allocatedVertices - vertices->size()) * sizeof(Vertex), " to local buffer");

        vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);

        vk::copyBuffer(*m_pVkDataPtr, m_pCommandPools.back(), indexStagingBuffer.buffer, indexBuffer->buffer, indices->size() * sizeof(IndexType), (indexBuffer->allocatedVertices - indices->size()) * sizeof(IndexType));

        vmaDestroyBuffer(m_pAllocator, indexStagingBuffer.buffer, indexStagingBuffer.allocation);

        IO::Info("From function ", __PRETTY_FUNCTION__, "\n\tin (", __FILE__, ":", __LINE__, ") \n\t", "copied ", indices->size(), " indices, of size ", indices->size() * sizeof(IndexType), " bytes, at offset ", (indexBuffer->allocatedVertices - indices->size()) * sizeof(IndexType), " to local buffer");

        MeshResource meshResource = {
            .vertexBuffer = vertexBufferRef,
            .indexBuffer = indexBufferRef,
            .vertexOffset = static_cast<uint32_t>((vertexBuffer->allocatedVertices - (uint32_t)vertices->size()) * sizeof(Vertex)),
            .indexOffset = static_cast<uint32_t>((indexBuffer->allocatedVertices - (uint32_t)indices->size()) * sizeof(IndexType)),
            .numVertices = (uint32_t)vertices->size(),
            .numIndices = (uint32_t)indices->size()};
        auto ref = meshPool.insert(meshResource);

        IO::Warning("Function ready, testing required\n\t", __PRETTY_FUNCTION__,
                    "\n\tin ", __FILE__, ":", __LINE__);

        // throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
        return ref;
    }

    /**
     * Get functions
     *
     */

    vk::VulkanTexture *VulkanResourceManager::getTexture(Ref<Texture>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    vk::VulkanBuffer *VulkanResourceManager::getBuffer(Ref<Buffer>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    ShaderStageInfo *VulkanResourceManager::getShader(Ref<Shader>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    VkDescriptorSetLayoutBinding *VulkanResourceManager::getUniformData(Ref<UniformBindings>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    vk::VulkanMaterial *VulkanResourceManager::getMaterial(Ref<Material>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    vk::VulkanRenderPass *VulkanResourceManager::getRenderPass(Ref<RenderPass> ref)
    {

        return renderPassPool.get(ref);
    }

    /**
     * Destroy functions
     */
    void VulkanResourceManager::destroyTexture(Ref<Texture>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    void VulkanResourceManager::destroyBuffer(Ref<Buffer>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    void VulkanResourceManager::destroyShader(Ref<Shader>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    void VulkanResourceManager::destroyUniformData(Ref<UniformBindings>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    void VulkanResourceManager::destroyMaterial(Ref<Material>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    void VulkanResourceManager::destroyMesh(Ref<Mesh>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
    void VulkanResourceManager::destroyRenderPass(Ref<RenderPass>)
    {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }
}