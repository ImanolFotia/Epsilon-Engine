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

		std::unordered_map<std::string, unsigned int> m_pRenderLayouts;

		Ref<RenderPass> m_pCurrentRenderPass;
		Ref<BindGroup> m_pDefaultBindGroup;

	public:
		Scene()
		{
			m_pRenderLayouts["DefaultLayout"] = 0;
			m_pRenderLayouts["SkyLayout"] = 1;
			m_pRenderLayouts["TerrainLayout"] = 2;

		}

		void Init() {

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

			m_pAssetManager.Init();

			engine::BindGroupInfo defaultBindGroup = {
					.bindingInfo = {
						{.size = sizeof(ShaderObjectData), .offset = 0, .binding = 0, .type = engine::UniformBindingType::UNIFORM_BUFFER},
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						//{.size = sizeof(ObjectData) * AssetManager::MAX_OBJECTS, .offset = 0, .binding = 2, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"},
						//{.size = sizeof(glm::mat4) * AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 3, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"}
			},
					.inputs = {},
					.renderPass = "DefaultRenderPass",
					.name = "DefaultBindGroup",
			};

			m_pDefaultBindGroup = resourceManager->createBindGroup(defaultBindGroup);

		}

		void setCurrentRenderPass(const std::string& renderpass) {
			m_pCurrentRenderPass = m_RenderPassesRefs[renderpass];
		}

		Ref<RenderPass> getRenderPass(const std::string& name) {
			return m_RenderPassesRefs.at(name);
		}

		AssetManager& getAssetManager()
		{
			return m_pAssetManager;
		}

		template <typename T>
		auto insertIntoScene(T object)
		{
			return m_pSceneManager.insert(m_pSceneManager.root, object);
		}

		template <typename T, class... Args>
		auto emplaceIntoScene(Args &&...args)
		{
			return m_pSceneManager.emplace<T>(m_pSceneManager.root, std::forward<Args>(args)...);
		}

		template <typename T>
		auto emplaceIntoScene()
		{
			return m_pSceneManager.emplace<T>(m_pSceneManager.root);
		}

		template <typename P, typename T>
		auto insertIntoNode(std::shared_ptr<Node<P>> parent, T object)
		{
			return m_pSceneManager.insert(parent, object);
		}

		template <typename T, typename P, class... Args>
		auto emplaceIntoNode(std::shared_ptr<Node<P>> parent, Args &&...args)
		{
			return m_pSceneManager.emplace<T>(parent, std::forward<Args>(args)...);
		}

		template <typename T, typename P>
		auto emplaceIntoNode(std::shared_ptr<Node<P>> parent)
		{
			return m_pSceneManager.emplace<T>(parent);
		}

		template <typename T>
		auto getNodes()
		{
			return m_pSceneManager.get<T>();
		}


		template <typename T, typename P>
		auto getChild(std::shared_ptr<Node<P>> parent)
		{
			auto single_child = m_pSceneManager.getChild<T>(parent);
			return m_pSceneManager.to<T>(single_child);
		}

		template <typename T>
		auto getChild()
		{
			auto single_child = m_pSceneManager.getChild<T>(m_pSceneManager.root);
			return m_pSceneManager.to<T>(single_child);
		}

		void BeginScene()
		{
			auto renderer = Context().Renderer();
			renderer->BeginFrame();
			renderer->Begin();
		}

		void Flush()
		{
			auto renderer = Context().Renderer();

			renderer->Flush(m_RenderPassesRefs["DefaultRenderPass"], engine::DrawType::INDEXED);
		}

		void Push(std::shared_ptr<Node<RenderModel>> renderModel, glm::mat4 transform, const std::string& layout)
		{
			auto renderer = Context().Renderer();

			Ref<PushConstant> push_constant;

			for (auto& mesh : renderModel->data.renderMeshes)
			{
				renderer->Push({ .mesh = mesh.mesh,
								.material = m_pDefaultBindGroup,
								.pushConstant = push_constant,
								.objectConstant = {
													.transform = transform, 
													.material_index = m_pAssetManager.m_pMaterials.at(mesh.material_key).index
												  },
								.layout_index = m_pRenderLayouts.at(layout),
								.uniformIndex = m_pAssetManager.m_pMaterials.at(mesh.material_key).index });
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