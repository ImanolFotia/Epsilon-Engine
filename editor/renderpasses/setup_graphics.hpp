#pragma once

#include <core/engine/scene/scene.hpp>
#include <core/engine/scene/parsers/renderpass.hpp>

namespace Editor {

	class GraphicsHelper {

	public:
		std::shared_ptr<engine::AssetManager> m_pAssetManager;
		std::shared_ptr<engine::Context> m_pContext;

		std::unordered_map<std::string, engine::RenderPassInfo> m_RenderPassesInfo;
		std::unordered_map<std::string, engine::Ref<engine::RenderPass>> m_RenderPassesRefs;
		std::unordered_map<size_t, engine::parsers::RenderLayout> m_pRenderLayouts{};


		GraphicsHelper(std::shared_ptr<engine::Context> context, std::shared_ptr<engine::AssetManager> assetManager) : m_pContext{ context }, m_pAssetManager{ assetManager }
		{
		}

		engine::Ref<engine::RenderPass> getRenderPass(const std::string& name)
		{
			return m_RenderPassesRefs.at(name);
		}

		void UpdateBindGroup(const std::string& name)
		{
			auto resourceManager = m_pContext->ResourceManager();
			// resourceManager->updateBindGroup();
			m_pRenderLayouts[std::hash<std::string>{}(name)];
		}

		std::unordered_map<size_t, engine::parsers::RenderLayout> getBindGroups()
		{
			return m_pRenderLayouts;
		}

		engine::parsers::RenderLayout getBindGroup(const std::string& name) {
			return m_pRenderLayouts[std::hash <std::string>{}(name)];
		}

		engine::parsers::RenderLayout getBindGroup(size_t id) {
			return m_pRenderLayouts[id];
		}

		void addRenderPass(const std::string& name, engine::Ref<engine::RenderPass> renderPass)
		{
			m_RenderPassesRefs[name] = renderPass;
		}

		engine::Ref<engine::BindGroup> addBindGroup(const std::string& name, uint32_t layoutIndex, engine::BindGroupInfo info)
		{
			auto resourceManager = m_pContext->ResourceManager();
			m_pRenderLayouts[std::hash<std::string>{}(name)] = { layoutIndex, resourceManager->createBindGroup(info) };

			return m_pRenderLayouts[std::hash<std::string>{}(name)].bindGroup;
		}

		engine::Ref<engine::BindGroup> addBindGroup(const std::string& name, uint32_t layoutIndex, engine::Ref<engine::BindGroup> bindGroup)
		{
			m_pRenderLayouts[std::hash<std::string>{}(name)] = { layoutIndex, bindGroup };
			return bindGroup;
		}

	};
}