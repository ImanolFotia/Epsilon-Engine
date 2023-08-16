#include "core/framework/window.hpp"
#include "core/framework/exception.hpp"
#include <core/framework/clock.hpp>
#include "core/framework/common.hpp"
#include <core/engine/context.hpp>

#include "wgpu.hpp"
#include "imgui/imgui_setup.hpp"
#include "resource_manager.hpp"

#include "helpers.hpp"

// #define VMA_VULKAN_VERSION 1001000
#if defined(ANDROID) || defined(__ANDROID__)

#include <android/log.h>

#define LOG_TAG "Epsilon"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

namespace engine
{

	WGPURenderer::WGPURenderer()
	{
	}

	WGPURenderer::~WGPURenderer()
	{
	}

	void WGPURenderer::Init(const char *appName, framework::Window &window)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	engine::Renderer::ObjectDataId
	WGPURenderer::RegisterMesh(const std::vector<common::Vertex> &, std::vector<IndexType> &indices, bool)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	engine::Renderer::TexturesDataId WGPURenderer::RegisterTexture(TextureCreationInfo)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	Material WGPURenderer::CreateMaterial(Ref<Material>)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::Push(ObjectData object)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::BeginFrame()
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::InitDebugRenderer()
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::Begin()
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::Submit()
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::EndFrame()
	{
	}

	void WGPURenderer::End(glm::vec3 &v)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::Flush(engine::Ref<engine::RenderPass> renderPassRef, engine::DrawType type)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::FlushIndexed(wgpu::WGPURenderPass *renderPass)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::FlushNonIndexed(wgpu::WGPURenderPass *renderPass)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::FlushIndirect(wgpu::WGPURenderPass *renderPass)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::FlushIndexedIndirect(wgpu::WGPURenderPass *renderPass)
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	int32_t WGPURenderer::pPrepareSyncObjects()
	{

		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::pUpdateUniforms(const wgpu::WGPUBuffer &buffer)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::pUpdateUniformBuffer(const wgpu::WGPUBuffer &buffer, const void *newData)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::Cleanup()
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::SetVSync(bool state)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::UpdateRenderPassUniforms(Ref<RenderPass> renderPassRef, BindingIndex index, const void *data)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::pUpdateMaterial(wgpu::WGPUMaterial &)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::pRecreateSwapChain()
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::SetViewport(const Viewport &viewport)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::SetScissor(const Scissor &scissor)
	{
		throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
	}

	void WGPURenderer::SetRenderPass(Ref<RenderPass> renderpass)
	{
		// m_pActiveRenderPass = renderpass;
	}

	std::shared_ptr<ImGuiRenderer> WGPURenderer::getDebugRenderer()
	{
		return m_pImguiRenderer;
	}
}