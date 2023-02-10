#undef VMA_DEBUG_LOG
#undef VMA_DEBUG_LOG_FORMAT

#include "resource_manager.hpp"

#include "core/framework/exception.hpp"

#define VMA_DEBUG_LOG_FORMAT(format, ...)

#define VMA_DEBUG_LOG_FORMAT(format, ...) do { \
    printf((format), __VA_ARGS__); \
    printf("\n"); \
} while(false)


#define VMA_DEBUG_LOG(str)   VMA_DEBUG_LOG_FORMAT("%s", (str))

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif


namespace engine {
    VulkanResourceManager::VulkanResourceManager() {
    }

    VulkanResourceManager::~VulkanResourceManager() {
    }

    void VulkanResourceManager::Init() {

        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = m_pVkDataPtr->physicalDevice;
        allocatorInfo.device = m_pVkDataPtr->logicalDevice;
        allocatorInfo.instance = m_pVkDataPtr->instance;

        vmaCreateAllocator(&allocatorInfo, &m_pAllocator);

        m_pCommandPools.emplace_back();

        vk::createCommandPool(*m_pVkDataPtr, m_pCommandPools.back());
    }

    Ref<Texture> VulkanResourceManager::createTexture(unsigned char *pixels, TextureInfo texInfo) {
        Ref<Texture> refTexture;

        auto stagingBuffer = pCreateStagingTextureBuffer(pixels, texInfo);

        auto texture = pCreateTextureBuffer({.width = texInfo.width,
                                                    .height = texInfo.height,
                                                    .num_channels = texInfo.numChannels,
                                                    .usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                             VK_IMAGE_USAGE_SAMPLED_BIT});

        auto size = texInfo.width * texInfo.height * texInfo.numChannels;

        transitionImageLayout(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), texture.image,
                              VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              texture.info);
        copyBufferToImage(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), stagingBuffer.buffer, texture.image,
                          static_cast<uint32_t>(texInfo.width), static_cast<uint32_t>(texInfo.height));
        transitionImageLayout(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools.back(), texture.image,
                              VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                              VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture.info);

        vmaDestroyBuffer(m_pAllocator, stagingBuffer.buffer, stagingBuffer.allocation);
        // vmaFreeMemory(m_pAllocator, m_pStagingTextureBuffer.allocation);

        texture.format = VK_FORMAT_R8G8B8A8_SRGB;
        vk::createImageView(*m_pVkDataPtr, texture, VK_IMAGE_ASPECT_COLOR_BIT);
        vk::createTextureSampler(*m_pVkDataPtr, texture);

        auto ref = texPool.insert(texture);
        return ref;
    }

    Ref<Material> VulkanResourceManager::createMaterial(MaterialInfo material, Ref<RenderPass> renderPassRef) {
        return createMaterial(material, renderPassRef, {});
    }

    Ref<Material> VulkanResourceManager::createMaterial(MaterialInfo material, Ref<RenderPass> renderPassRef,
                                                        std::initializer_list<RenderPassBinding> bindings) {
        try {
            vk::VulkanMaterial vkMaterial;
            auto renderPass = renderPassPool.get(renderPassRef);
            vkMaterial.descriptorSetLayout = renderPass->renderPipelines.back().descriptorSetLayout;
            //auto &materialdata = m_pMaterials.emplace_back();

            for (auto &binding: bindings) {
                auto pass = renderPassPool.get(binding.renderPass);
                vkMaterial.renderBufferBindings.push_back(pass->renderPassChain.Textures.at(binding.index));
            }

            for (auto &texture: material.textures) {
                auto tex = createTexture(texture.pixels, texture);
                vkMaterial.textures.push_back(*texPool.get(tex));
            }

            //auto uniformBufferRef = createUniformData(material.bindingInfo);

            auto buffers = renderPass->uniformBuffer.buffers;

            for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
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
        catch (std::exception &e) {

            throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
        }

    }

    /*
    Ref<Texture> VulkanResourceManager::getTexture(Ref<RenderPass> renderPassRef, uint32_t index) {

        auto renderPass = renderPassPool.get(renderPassRef);
        Ref<Texture> textureRef;

        renderPass->renderPassChain.Textures.at(index);
    }*/

    void VulkanResourceManager::pRecreateSwapChain(framework::Window::windowType *window) {
        vkDeviceWaitIdle(m_pVkDataPtr->logicalDevice);

        for (auto &pass: renderPassPool) {
            cleanupSwapChain(*m_pVkDataPtr, pass);
        }

        vk::createSwapChain(*m_pVkDataPtr, window);

        vk::createImageViews(*m_pVkDataPtr);

        // Recreate the default renderpass
        vk::createRenderPass(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, m_pDefaultRenderPassInfo, true);

        for (auto i = 0; i < m_pVkDataPtr->defaultRenderPass.renderPipelines.size(); i++)
            vk::createGraphicsPipeline(*m_pVkDataPtr,
                                       m_pVkDataPtr->defaultRenderPass,
                                       m_pVkDataPtr->defaultRenderPass.renderPipelines[i],
                    //m_pVkDataPtr->defaultRenderPass.vertexInfo[i],
                                       m_pDefaultRenderPassInfo);

        vk::createFramebuffers(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass,
                               m_pVkDataPtr->defaultRenderPass.renderPassChain);

        for (auto &pass: renderPassPool) {
            if (pass.id == std::numeric_limits<uint32_t>::max()) continue;
            vk::createRenderPass(*m_pVkDataPtr, pass, m_pRenderPassInfo[pass.id], false);

            for (auto i = 0; i < pass.renderPipelines.size(); i++)
                vk::createGraphicsPipeline(*m_pVkDataPtr, pass, pass.renderPipelines[i], m_pRenderPassInfo[pass.id]);

            vk::createFramebuffers(*m_pVkDataPtr, pass, pass.renderPassChain);
        }
    }

    Ref<RenderPass> VulkanResourceManager::createDefaultRenderPass(RenderPassInfo renderPassInfo) {


        m_pVkDataPtr->defaultRenderPass.id = std::numeric_limits<uint32_t>::max();
        m_pDefaultRenderPassInfo = renderPassInfo;
        vk::createRenderPass(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass, renderPassInfo, true);

        for (int i = 0; i < renderPassInfo.numLayouts; i++) {


            m_pVkDataPtr->defaultRenderPass.renderPipelines.emplace_back();
            vk::createDescriptorSetLayout(*m_pVkDataPtr,
                                          m_pVkDataPtr->defaultRenderPass.renderPipelines.back().descriptorSetLayout);
            vk::createGraphicsPipeline(*m_pVkDataPtr,
                                       m_pVkDataPtr->defaultRenderPass,
                                       m_pVkDataPtr->defaultRenderPass.renderPipelines.back(),
                                       renderPassInfo);
        }

        vk::createFramebuffers(*m_pVkDataPtr, m_pVkDataPtr->defaultRenderPass,
                               m_pVkDataPtr->defaultRenderPass.renderPassChain);

        auto uniformRef = createUniformData(renderPassInfo.bindingInfo);
        m_pVkDataPtr->defaultRenderPass.uniformBuffer = *uniformBufferPool.get(uniformRef);
        m_pDefaultRenderPassRef = renderPassPool.insert(m_pVkDataPtr->defaultRenderPass);

        return m_pDefaultRenderPassRef;
    }

    Ref<RenderPass> VulkanResourceManager::createRenderPass(RenderPassInfo renderPassInfo) {
        vk::VulkanRenderPass renderPass = {};

        renderPass.id = m_pRenderPassCount;
        m_pRenderPassInfo.push_back(renderPassInfo);

        renderPass.renderPassChain.ImageFormats.resize(renderPassInfo.attachments.size());
        for (int index = 0; index < renderPassInfo.attachments.size(); index++)
            renderPass.renderPassChain.ImageFormats[index] = resolveFormat(renderPassInfo.attachments[index].format);

        vk::createRenderPass(*m_pVkDataPtr, renderPass, renderPassInfo, false);

        for (size_t i = 0; i < renderPassInfo.attachments.size(); i++) {
            auto &attachment = renderPassInfo.attachments[i];
            vk::VulkanTextureInfo texInfo;
            texInfo.format = attachment.isDepthAttachment ? findDepthFormat(*m_pVkDataPtr) : resolveFormat(
                    renderPassInfo.attachments[i].format);
            texInfo.addressMode = resolveWrapping(attachment.wrapMode);
            texInfo.compareOp = resolveCompareOp(attachment.compareFunc);
            texInfo.filter = resolveFilter(attachment.filtering);
            texInfo.width = renderPassInfo.dimensions.width;
            texInfo.height = renderPassInfo.dimensions.height;
            texInfo.num_channels = attachment.isDepthAttachment ? 1 : resolveNumChannels(attachment.format);
            texInfo.usage = attachment.isDepthAttachment ? VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT :
                            VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            texInfo.compareEnable = attachment.depthCompare;
            if (attachment.isSampler)
                texInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

            vk::VulkanTexture texture = pCreateTextureBuffer(texInfo);

            createImageView(*m_pVkDataPtr, texture,
                            attachment.isDepthAttachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT);

            if (attachment.isSampler) {
                vk::createTextureSampler(*m_pVkDataPtr, texture);
            }

            if (attachment.isDepthAttachment) {
                renderPass.renderPassChain.DepthTexture = texture;
            } else {
                renderPass.renderPassChain.ImageViews.push_back(texture.imageView);
                renderPass.renderPassChain.Textures.push_back(texture);
            }

        }

        renderPass.renderPassChain.Extent.width = renderPassInfo.dimensions.width;
        renderPass.renderPassChain.Extent.height = renderPassInfo.dimensions.height;

        for (int i = 0; i < renderPassInfo.numLayouts; i++) {

            renderPass.renderPipelines.emplace_back();
            vk::createDescriptorSetLayout(*m_pVkDataPtr, renderPass.renderPipelines.back().descriptorSetLayout);

            vk::createGraphicsPipeline(*m_pVkDataPtr,
                                       renderPass,
                                       renderPass.renderPipelines.back(),
                                       renderPassInfo);
        }

        vk::createFramebuffers(*m_pVkDataPtr, renderPass, renderPass.renderPassChain);

        auto uniformRef = createUniformData(renderPassInfo.bindingInfo);
        renderPass.uniformBuffer = *uniformBufferPool.get(uniformRef);
        auto ref = renderPassPool.insert(renderPass);

        m_pRenderPassCount++;
        return ref;
    }

    void VulkanResourceManager::clean() {
        for (auto &buffer: vertexBufferPool) {
            vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
        }

        for (auto &buffer: indexBufferPool) {
            vmaDestroyBuffer(m_pAllocator, buffer.buffer, buffer.allocation);
        }

        for (auto &buffer: uniformBufferPool) {
            for (auto &b: buffer.buffers) {
                vmaDestroyBuffer(m_pAllocator, b.buffer, b.allocation);
            }
        }


        vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);

        vk::cleanupSyncObjects(*m_pVkDataPtr);
        vk::cleanCommandPool(*m_pVkDataPtr, m_pVkDataPtr->m_pCommandPools[0]);

        for (auto &texture: texPool) {
            vkDestroySampler(m_pVkDataPtr->logicalDevice, texture.sampler, nullptr);
            vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture.imageView, nullptr);
            vmaDestroyImage(m_pAllocator, texture.image, texture.allocation);
            //vk::destroyImage(*m_pVkDataPtr, texture);
        }


        for (auto &pass: renderPassPool) {
            vk::cleanupRenderPass(*m_pVkDataPtr, pass.renderPass);
            for (auto &pipeline: pass.renderPipelines)
                vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);

            if (pass.id == std::numeric_limits<uint32_t>::max()) continue;

            for (int i = 0; i < pass.renderPassChain.Textures.size(); i++) {
                vkDestroySampler(m_pVkDataPtr->logicalDevice, pass.renderPassChain.Textures[i].sampler, nullptr);
                vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass.renderPassChain.Textures[i].imageView, nullptr);
                vmaDestroyImage(m_pAllocator, pass.renderPassChain.Textures[i].image,
                                pass.renderPassChain.Textures[i].allocation);
            }

            if (pass.renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE)
                vkDestroySampler(m_pVkDataPtr->logicalDevice, pass.renderPassChain.DepthTexture.sampler, nullptr);

            if (pass.renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE)
                vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass.renderPassChain.DepthTexture.imageView, nullptr);

            vmaDestroyImage(m_pAllocator, pass.renderPassChain.DepthTexture.image,
                            pass.renderPassChain.DepthTexture.allocation);

        }


        vmaDestroyAllocator(m_pAllocator);
        vk::cleanup(*m_pVkDataPtr);
    }

    /**
     * Create functions
     */
    Ref<Buffer> VulkanResourceManager::destroyBuffer(BufferInfo) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    Ref<Shader> VulkanResourceManager::createShader(ShaderInfo) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    Ref<UniformBindings> VulkanResourceManager::createUniformData(UniformBindingInfo bindingInfo) {
        auto buffer = pCreateUniformBuffer(bindingInfo);
        return uniformBufferPool.insert(buffer);
        //throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    Ref<Mesh> VulkanResourceManager::createMesh(MeshInfo meshInfo) {
        std::vector<Vertex> *vertices = &meshInfo.vertices;
        std::vector<uint32_t> *indices = &meshInfo.indices;

        auto maxOffset = [](auto &indices) -> uint32_t {
            uint32_t out = 0;
            for (auto &i: indices) out = i > out ? i : out;
            return out;
        };

        //uint32_t iOffset = vertexBuffer->allocatedVertices;//maxOffset(meshInfo.indices);

        Ref<Buffer> vertexBufferRef = pFetchVertexBuffer(vertices->size());
        vk::VulkanBuffer *vertexBuffer = vertexBufferPool.get(vertexBufferRef);

        Ref<Buffer> indexBufferRef = pFetchIndexBuffer(indices->size(), vertexBuffer->allocatedVertices);
        vk::VulkanBuffer *indexBuffer = indexBufferPool.get(indexBufferRef);

        auto vertexStagingBuffer = pCreateStagingBuffer(*vertices);
        auto indexStagingBuffer = pCreateStagingIndexBuffer(*indices);

        vk::copyBuffer(*m_pVkDataPtr, m_pCommandPools.back(), vertexStagingBuffer.buffer, vertexBuffer->buffer,
                       vertices->size() * sizeof(Vertex), vertexBuffer->allocatedVertices * sizeof(Vertex));

        vmaDestroyBuffer(m_pAllocator, vertexStagingBuffer.buffer, vertexStagingBuffer.allocation);

        vk::copyBuffer(*m_pVkDataPtr, m_pCommandPools.back(), indexStagingBuffer.buffer, indexBuffer->buffer,
                       indices->size() * sizeof(IndexType), indexBuffer->allocatedVertices * sizeof(IndexType));

        vmaDestroyBuffer(m_pAllocator, indexStagingBuffer.buffer, indexStagingBuffer.allocation);

        if (0) {
            IO::Info("From function ", __PRETTY_FUNCTION__, "\n\tin (", __FILE__, ":", __LINE__, ") \n\t", "copied ",
                     vertices->size(), " vertices, of size ", vertices->size() * sizeof(Vertex), " bytes, at offset ",
                     vertexBuffer->allocatedVertices * sizeof(Vertex), " to local buffer");

            IO::Info("From function ", __PRETTY_FUNCTION__, "\n\tin (", __FILE__, ":", __LINE__, ") \n\t", "copied ",
                     indices->size(), " indices, of size ", indices->size() * sizeof(IndexType), " bytes, at offset ",
                     indexBuffer->allocatedVertices * sizeof(IndexType), " to local buffer");
        }

        MeshResource meshResource = {
                .vertexBuffer = vertexBufferRef,
                .indexBuffer = indexBufferRef,
                .vertexOffset = vertexBuffer->allocatedVertices,
                .indexOffset = indexBuffer->allocatedVertices,
                .numVertices = (uint32_t) vertices->size(),
                .numIndices = (uint32_t) indices->size()};
        auto ref = meshPool.insert(meshResource);


        int maxAllocatingSize = sizeof(IndexType) * (indexBuffer->allocatedVertices + indices->size());

        if (maxAllocatingSize > MAX_INDICES_PER_BUFFER * sizeof(IndexType)) {
            std::cout << "sdf" << std::endl;
        }
        vertexBuffer->allocatedVertices += vertices->size();
        indexBuffer->allocatedVertices += indices->size();

        if (0)
            IO::Warning("Function ready, testing required\n\t", __PRETTY_FUNCTION__,
                        "\n\tin ", __FILE__, ":", __LINE__);

        // throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
        return ref;
    }

    /**
     * Get functions
     *
     */

    vk::VulkanTexture *VulkanResourceManager::getTexture(Ref<Texture>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    vk::VulkanBuffer *VulkanResourceManager::getBuffer(Ref<Buffer>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    ShaderStageInfo *VulkanResourceManager::getShader(Ref<Shader>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    VkDescriptorSetLayoutBinding *VulkanResourceManager::getUniformData(Ref<UniformBindings>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    vk::VulkanMaterial *VulkanResourceManager::getMaterial(Ref<Material>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    vk::VulkanRenderPass *VulkanResourceManager::getRenderPass(Ref<RenderPass> ref) {

        return renderPassPool.get(ref);
    }

    /**
     * Destroy functions
     */
    void VulkanResourceManager::destroyTexture(Ref<Texture> textureRef) {
        auto texture = texPool.get(textureRef);
        vkDestroySampler(m_pVkDataPtr->logicalDevice, texture->sampler, nullptr);
        vkDestroyImageView(m_pVkDataPtr->logicalDevice, texture->imageView, nullptr);
        vk::destroyImage(*m_pVkDataPtr, *texture);
    }

    void VulkanResourceManager::destroyBuffer(Ref<Buffer>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    void VulkanResourceManager::destroyShader(Ref<Shader>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    void VulkanResourceManager::destroyUniformData(Ref<UniformBindings>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    void VulkanResourceManager::destroyMaterial(Ref<Material>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    void VulkanResourceManager::destroyMesh(Ref<Mesh>) {

        throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
    }

    void VulkanResourceManager::destroyRenderPass(Ref<RenderPass> renderPassRef) {
        auto pass = renderPassPool.get(renderPassRef);
        vk::cleanupRenderPass(*m_pVkDataPtr, pass->renderPass);
        for (auto &pipeline: pass->renderPipelines)
            vk::destroyGraphicsPipeline(*m_pVkDataPtr, pipeline);

        if (pass->id == std::numeric_limits<uint32_t>::max()) return;

        for (int i = 0; i < pass->renderPassChain.Textures.size(); i++) {
            vkDestroySampler(m_pVkDataPtr->logicalDevice, pass->renderPassChain.Textures[i].sampler, nullptr);
            vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass->renderPassChain.Textures[i].imageView, nullptr);
            vmaDestroyImage(m_pAllocator, pass->renderPassChain.Textures[i].image,
                            pass->renderPassChain.Textures[i].allocation);
        }

        if (pass->renderPassChain.DepthTexture.sampler != VK_NULL_HANDLE)
            vkDestroySampler(m_pVkDataPtr->logicalDevice, pass->renderPassChain.DepthTexture.sampler, nullptr);

        if (pass->renderPassChain.DepthTexture.imageView != VK_NULL_HANDLE)
            vkDestroyImageView(m_pVkDataPtr->logicalDevice, pass->renderPassChain.DepthTexture.imageView, nullptr);

        vmaDestroyImage(m_pAllocator, pass->renderPassChain.DepthTexture.image,
                        pass->renderPassChain.DepthTexture.allocation);
    }

    Ref<ID> VulkanResourceManager::getId(Ref<RenderPass> renderPass) {
        //renderPassPool.getId(renderPass);
        //resourceIdPool
    }

    VkFormat VulkanResourceManager::resolveFormat(engine::TextureFormat format) {
        //Color formats
        if (format == COLOR_R) return VK_FORMAT_R8_SRGB;
        if (format == COLOR_RG) return VK_FORMAT_R8G8_SRGB;
        if (format == COLOR_RGB) return VK_FORMAT_R8G8B8_SRGB;
        if (format == COLOR_RGBA) return VK_FORMAT_R8G8B8A8_SRGB;
        if (format == COLOR_R_16F) return VK_FORMAT_R16_SFLOAT;
        if (format == COLOR_R_32F) return VK_FORMAT_R32_SFLOAT;
        if (format == COLOR_RG_16F) return VK_FORMAT_R16G16_SFLOAT;
        if (format == COLOR_RG_32F) return VK_FORMAT_R32G32_SFLOAT;
        if (format == COLOR_RGB_16F) return VK_FORMAT_R16G16B16_SFLOAT;
        if (format == COLOR_RGB_32F) return VK_FORMAT_R32G32B32_SFLOAT;
        if (format == COLOR_RGBA_16F) return VK_FORMAT_R16G16B16A16_SFLOAT;
        if (format == COLOR_RGBA_32F) return VK_FORMAT_R32G32B32A32_SFLOAT;

        //Non color formats (i.e. Normal Maps)
        if (format == NON_COLOR_R) return VK_FORMAT_R8_UNORM;
        if (format == NON_COLOR_RG) return VK_FORMAT_R8G8_UNORM;
        if (format == NON_COLOR_RGB) return VK_FORMAT_R8G8B8_UNORM;
        if (format == NON_COLOR_RGBA) return VK_FORMAT_R8G8B8A8_UNORM;

        if (format == DEPTH_F16) return VK_FORMAT_D16_UNORM;
        if (format == DEPTH_F32) return VK_FORMAT_D32_SFLOAT;
        if (format == DEPTH_F32_STENCIL_8) return VK_FORMAT_D32_SFLOAT_S8_UINT;

    }

    VkFilter VulkanResourceManager::resolveFilter(engine::Filtering filter) {
        if (filter == POINT) return VK_FILTER_NEAREST;
        if (filter == LINEAR) return VK_FILTER_LINEAR;
    }

    VkSamplerAddressMode VulkanResourceManager::resolveWrapping(engine::WrapMode wrapping) {
        if (wrapping == REPEAT) return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        if (wrapping == CLAMP_TO_BORDER) return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        if (wrapping == CLAMP_TO_EDGE) return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    }

    VkCompareOp VulkanResourceManager::resolveCompareOp(engine::CompareFunction compareOp) {
        if (compareOp == ALWAYS) return VK_COMPARE_OP_ALWAYS;
        if (compareOp == LESS) return VK_COMPARE_OP_LESS;
        if (compareOp == LESS_OR_EQUAL) return VK_COMPARE_OP_LESS_OR_EQUAL;
        if (compareOp == EQUAL) return VK_COMPARE_OP_EQUAL;
        //TODO: Implement the rest
    }


    unsigned VulkanResourceManager::resolveNumChannels(engine::TextureFormat format) {
        if (format == COLOR_R ||
            format == NON_COLOR_R ||
            format == DEPTH_F16 ||
            format == DEPTH_F32 ||
            format == DEPTH_F32_STENCIL_8 ||
            format == COLOR_R_32F ||
            format == COLOR_R_16F) {
            return 1;
        }

        if (format == COLOR_RG ||
            format == COLOR_RG_16F ||
            format == COLOR_RG_32F ||
            format == NON_COLOR_RG) {
            return 2;
        }

        if (format == COLOR_RGB ||
            format == COLOR_RGB_16F ||
            format == COLOR_RGB_32F ||
            format == NON_COLOR_RGB) {
            return 3;
        }
        if (format == COLOR_RGBA ||
            format == COLOR_RGBA_16F ||
            format == COLOR_RGBA_32F ||
            format == NON_COLOR_RGBA) {
            return 4;
        }
    }
}