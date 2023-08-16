#include "resource_manager.hpp"
#include "helpers.hpp"

#include "core/framework/exception.hpp"

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

namespace engine
{
	WGPUResourceManager::WGPUResourceManager()
	{
	}

	WGPUResourceManager::~WGPUResourceManager()
	{
	}

	void WGPUResourceManager::Init()
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<Texture> WGPUResourceManager::createTexture(TextureCreationInfo texInfo)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<BindGroup> WGPUResourceManager::createBindGroup(BindGroupInfo material)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPUResourceManager::pRecreateSwapChain(framework::Window::windowType *window)
	{
	}

	Ref<RenderPass> WGPUResourceManager::createDefaultRenderPass(RenderPassInfo renderPassInfo)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<RenderPass> WGPUResourceManager::createRenderPass(RenderPassInfo renderPassInfo)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPUResourceManager::clean()
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	/**
	 * Create functions
	 */
	Ref<Buffer> WGPUResourceManager::destroyBuffer(BufferInfo)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<UniformBindings> WGPUResourceManager::createUniformData(UniformBindingInfo bindingInfo)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<Mesh> WGPUResourceManager::createMesh(MeshInfo meshInfo)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<Mesh> WGPUResourceManager::createMesh(AnimatedMeshInfo meshInfo)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Ref<Buffer> WGPUResourceManager::createGPUBuffer(const std::string &name, uint32_t size, BufferStorageType type)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	/**
	 * Get functions
	 *
	 */

	vk::VulkanRenderPass *WGPUResourceManager::getRenderPass(Ref<RenderPass> ref)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	/**
	 * Destroy functions
	 */
	void WGPUResourceManager::destroyTexture(Ref<Texture> textureRef)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPUResourceManager::destroyBuffer(Ref<Buffer>)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPUResourceManager::destroyUniformData(Ref<UniformBindings>)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPUResourceManager::destroyMaterial(Ref<BindGroup>)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPUResourceManager::destroyMesh(Ref<Mesh>)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPUResourceManager::destroyRenderPass(Ref<RenderPass> renderPassRef)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void *WGPUResourceManager::mapBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}
	void WGPUResourceManager::unmapBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void *WGPUResourceManager::getMappedBuffer(Ref<Buffer> bufferRef, uint32_t currentFrame)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}
}