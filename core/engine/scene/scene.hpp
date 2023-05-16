#pragma once

#include "asset_manager.hpp"
#include "nodes/node_manager.hpp"

#include "trees/octree.hpp"
#include "parsers/renderpass.hpp"

namespace engine
{

	class Scene
	{
		AssetManager m_pAssetManager;
		// OctreeContainer<std::shared_ptr<NodeBase>> m_pOctree;
		SceneManager m_pSceneManager;

		std::unordered_map<std::string, RenderPassInfo> m_RenderPassesInfo;
		std::unordered_map<std::string, Ref<RenderPass>> m_RenderPassesRefs;


		Ref<RenderPass> m_pCurrentRenderPass;
		Ref<BindGroup> m_pDefaultBindGroup;

	public:
		Scene()
		{
			auto resourceManager = Context().ResourceManager();

			m_RenderPassesInfo = engine::parsers::parse_renderpasses();

			for (auto& [name, renderpass] : m_RenderPassesInfo) {
				if (renderpass.isSwapChainAttachment) {
					m_RenderPassesRefs[name] = Context().ResourceManager()->createDefaultRenderPass(renderpass);
				}
				else {
					m_RenderPassesRefs[name] = Context().ResourceManager()->createRenderPass(renderpass);
				}
			}

			engine::BindGroupInfo defaultBindGroup = {
					.bindingInfo = {
						{.size = sizeof(ShaderObjectData), .offset = 0, .binding = 0, .type = engine::UniformBindingType::UNIFORM_BUFFER},
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						{.size = sizeof(ObjectData) * AssetManager::MAX_OBJECTS, .offset = 0, .binding = 2, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"} },
					.inputs = {},
					.renderPass = "DefaultRenderPass",
					.name = "DefaultBindGroup",
			};

			m_pDefaultBindGroup = resourceManager->createBindGroup(defaultBindGroup);
		}

		void setCurrentRenderPass(const std::string& renderpass) {
			m_pCurrentRenderPass = m_RenderPassesRefs[renderpass];
		}

		const AssetManager &getAssetManager()
		{
			return m_pAssetManager;
		}

		template <typename T>
		auto insertIntoScene(T object)
		{
			return m_pSceneManager.insert(m_pSceneManager.root, object);
		}

		template <typename P, typename T>
		auto insertIntoNode(std::shared_ptr<Node<P>> parent, T object)
		{
			return m_pSceneManager.insert(parent, object);
		}

		template <class... Args>
		auto emplaceIntoScene(Args &&...args)
		{
			return m_pSceneManager.insert(m_pSceneManager.root, std::forward<Args>(args)...);
		}

		template <typename P, class... Args>
		auto emplaceIntoNode(std::shared_ptr<Node<P>> parent, Args &&...args)
		{
			return m_pSceneManager.insert(parent, std::forward<Args>(args)...);
		}

		template <typename T>
		auto getNodes()
		{
			return m_pSceneManager.get<T>();
		}

		template <typename P, typename T>
		auto getChild(std::shared_ptr<Node<P>> parent)
		{
			return m_pSceneManager.get<T>(parent);
		}

		void BeginScene()
		{
			auto renderer = Context().Renderer();
			renderer->BeginFrame();
			renderer->Begin();
		}

		void Push(const RenderModel &renderModel, std::string layout)
		{
			auto renderer = Context().Renderer();

			Ref<PushConstant> push_constant;

			for (auto &mesh : renderModel.renderMeshes)
			{
				renderer->Push({.mesh = mesh.mesh,
								.material = mesh.bind_group,
								.pushConstant = push_constant,
								.layout_index = 0,
								.uniformIndex = 0});
			}
		}

		void EndScene()
		{

			auto renderer = Context().Renderer();
			glm::vec3 v;
			renderer->End(v);

			renderer->Submit();
			renderer->EndFrame();
		}
	};
}