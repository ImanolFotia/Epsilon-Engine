#pragma once

#include "asset_manager.hpp"
#include "nodes/node_manager.hpp"

#include "trees/octree.hpp"
#include "parsers/renderpass.hpp"

#include "../audio/audio_manager.hpp"

#include "trees/octree.hpp"
#include "structs/frustum.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif


namespace engine
{

	struct EntityBehaviour {
		std::function<void(void)> callback;
		bool once = false;
	};

	struct OctreeSceneItem {
		OctreeSceneItem() = default;
		std::shared_ptr<Node<RenderModel>> renderModel;
		int index;
		int instance_index;
		bool visible;
	};
	class Scene
	{
		using OctreeRenderType = OctreeSceneItem;
		using OctreeRenderItem = typename std::list<OctreeItem<OctreeRenderType>>::iterator;
		using OctreeNodeType = std::shared_ptr<NodeBase>;
		AssetManager m_pAssetManager;
		std::shared_ptr<audio::AudioManager> m_pAudioManager;
		// OctreeContainer<std::shared_ptr<NodeBase>> m_pOctree;
		SceneManager m_pSceneManager;

		std::unordered_map<std::string, RenderPassInfo> m_RenderPassesInfo;
		std::unordered_map<std::string, Ref<RenderPass>> m_RenderPassesRefs;

		struct RenderLayout {
			unsigned int pipelineLayoutIndex = 0;
			Ref<BindGroup> bindGroup;
		};

		std::unordered_map<std::string, RenderLayout> m_pRenderLayouts;

		Ref<RenderPass> m_pCurrentRenderPass;

		std::shared_ptr<OctreeContainer<OctreeRenderType>> m_pRenderOctree;
		std::shared_ptr<OctreeContainer<OctreeNodeType>> m_pNodeOctree;

		Frustum m_pFrustum;

		std::shared_ptr<Context> m_pContext;

		uint32_t m_pMeshCount = 0;

		std::size_t getHash(const std::string& s) {
			return std::hash<std::string>{}(s);
		}


	public:

		struct SceneEntity {};

		Scene() = default;

		Scene(std::shared_ptr<Context> context) : m_pContext(context)
		{
			m_pAssetManager.m_pContext = m_pContext;


			m_pCurrentRenderPass = m_RenderPassesRefs["DefaultRenderPass"];

			m_pNodeOctree = std::make_shared<OctreeContainer<OctreeNodeType>>(Box { glm::vec3(100, 25, 100), glm::vec3(0.0, 12.5, 0.0) }, 8);
			m_pRenderOctree = std::make_shared<OctreeContainer<OctreeRenderType>>(Box{ glm::vec3(100, 25, 100), glm::vec3(0.0, 12.5, 0.0) }, 8);
		

			m_pAssetManager.Init();
		}

		std::shared_ptr<Context> getContext() { return m_pContext; }

		void Init() {

			auto resourceManager = m_pContext->ResourceManager();
			/*
			#ifdef _WIN32
						system("cd .\\assets\\shaders && .\\build_shaders.sh");
			#endif*/
			m_RenderPassesInfo = engine::parsers::parse_renderpasses();

			for (auto& [name, renderpass] : m_RenderPassesInfo) {
				if (renderpass.isSwapChainAttachment) {
					m_RenderPassesRefs[name] = m_pContext->ResourceManager()->createDefaultRenderPass(renderpass);
				}
				else {
					m_RenderPassesRefs[name] = m_pContext->ResourceManager()->createRenderPass(renderpass);
				}
			}


			m_pContext->Renderer()->InitDebugRenderer();

		}

		Ref<BindGroup> addBindGroup(const std::string& name, uint32_t layoutIndex, engine::BindGroupInfo info) {
			auto resourceManager = m_pContext->ResourceManager();
			m_pRenderLayouts[name] = { layoutIndex, resourceManager->createBindGroup(info) };

			return m_pRenderLayouts[name].bindGroup;
		}

		Ref<BindGroup> addBindGroup(const std::string& name, uint32_t layoutIndex, Ref<BindGroup> bindGroup) {
			auto resourceManager = m_pContext->ResourceManager();
			m_pRenderLayouts[name] = { layoutIndex, bindGroup };
			return bindGroup;
		}

		void addRenderPass(const std::string& name, Ref<RenderPass> renderPass) {
			m_RenderPassesRefs[name] = renderPass;
		}

		void setCurrentRenderPass(const std::string& renderpass) {
			m_pCurrentRenderPass = m_RenderPassesRefs[renderpass];

			auto renderer = m_pContext->Renderer();
			renderer->SetRenderPass(m_pCurrentRenderPass);
		}

		Ref<RenderPass> getRenderPass(const std::string& name) {
			return m_RenderPassesRefs.at(name);
		}

		AssetManager& getAssetManager()
		{
			return m_pAssetManager;
		}

		void SetViewport(const Viewport& viewport) {

			auto renderer = m_pContext->Renderer();
			renderer->SetViewport(viewport);
		}

		void SetScissor(const Scissor& scissor) {

			auto renderer = m_pContext->Renderer();
			renderer->SetScissor(scissor);
		}


		void UpdateFrustum(glm::mat4 proj, glm::mat4 view) {
			m_pFrustum.CalculateFrustum(proj * view, glm::mat4(1.0));
		}

		auto Cull() {
			return m_pRenderOctree->search(m_pFrustum);
		}

		auto Cull(Box box) {
			return m_pRenderOctree->search(box);
		}

		void RelocateObject(Box boundingBox, int index) {

			std::shared_ptr<Node<RenderModel>> node = std::static_pointer_cast<Node<RenderModel>>(m_pSceneManager.get(index));
			auto octree_render_node = getChild<typename std::list<OctreeItem<OctreeRenderType>>::iterator>(node->Parent());
			if (octree_render_node != nullptr) {
				auto new_item = m_pRenderOctree->relocate(octree_render_node->data, boundingBox);
			}
		}

		template<typename T>
		void removeFromScene(uint32_t index) {
			std::shared_ptr<Node<T>> node = std::static_pointer_cast<Node<T>>(m_pSceneManager.get(index));
				auto octree_render_node = getChild<typename std::list<OctreeItem<OctreeRenderType>>::iterator>(node);
				if(octree_render_node != nullptr)
					m_pRenderOctree->erase(octree_render_node->data);

			m_pSceneManager.erase<T>(node);
		}


		template<typename T>
		bool isOfType(std::shared_ptr<NodeBase> node) {
			return m_pSceneManager.isOfType<T>(node);
		}

		template <typename T>
		auto insertIntoScene(Box boundingBox, T object)
		{
			auto scene_node = m_pSceneManager.insert(m_pSceneManager.root, object);

			m_pNodeOctree->insert(boundingBox, { scene_node, scene_node->Index() });
			
			return scene_node;
		}

		template <typename T, class... Args>
		auto emplaceIntoScene(Box boundingBox, Args &&...args)
		{
			auto scene_node = m_pSceneManager.emplace<T>(m_pSceneManager.root, std::forward<Args>(args)...);

			m_pNodeOctree->insert(boundingBox, scene_node);

			return scene_node;
		}

		template <typename T>
		auto emplaceIntoScene(Box boundingBox)
		{
			auto scene_node = m_pSceneManager.emplace<T>(m_pSceneManager.root);
			m_pNodeOctree->insert(boundingBox, scene_node);

			return scene_node;
		}

		template <typename P, typename T>
		auto insertIntoNode(std::shared_ptr<Node<P>> parent, T object)
		{
			auto node = m_pSceneManager.insert(parent, object);

			return node;
		}

		template <typename P, typename T>
		auto insertIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent, T object)
		{
			auto node = m_pSceneManager.insert(parent, object);

			if (typeid(T) == typeid(RenderModel)) {
				OctreeSceneItem item;
				item.renderModel = node;
				item.index = node->Index();
				std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(boundingBox, item);
				insertIntoNode(parent, octree_node);
			}
			return node;
		}

		void insertIntoOctree(Box boundingBox, OctreeSceneItem item) {
			std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(boundingBox, item);
		}

		template <typename T, typename P, class... Args>
		auto emplaceIntoNode(std::shared_ptr<Node<P>> parent, Args &&...args)
		{
			auto node = m_pSceneManager.emplace<T>(parent, std::forward<Args>(args)...);
			return node;
		}


		template <typename T, typename P, class... Args>
		auto emplaceIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent, Args &&...args)
		{

			auto node = m_pSceneManager.emplace<T>(parent, std::forward<Args>(args)...);
			if (typeid(T) == typeid(RenderModel)) {

				OctreeSceneItem item;
				item.renderModel = node;
				item.index = node->Index();
				std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(boundingBox, item);
				insertIntoNode(parent, octree_node);
			}

			return node;
		}

		template <typename T, typename P>
		auto emplaceIntoNode(std::shared_ptr<Node<P>> parent)
		{
			auto node = m_pSceneManager.emplace<T>(parent);
			return node;

		}

		template <typename T, typename P>
		auto emplaceIntoNode(Box boundingBox, std::shared_ptr<Node<P>> parent)
		{
			auto node = m_pSceneManager.emplace<T>(parent);
			if (typeid(T) == typeid(RenderModel)) {
				OctreeSceneItem item;
				item.renderModel = node;
				item.index = node->Index();
				std::list<OctreeItem<OctreeRenderType>>::iterator octree_node = m_pRenderOctree->insert(boundingBox, item);
				insertIntoNode(parent, octree_node);
			}

			return node;

		}

		template <typename T>
		auto getNodes()
		{
			return m_pSceneManager.get<T>();
		}

		const auto& getNodes()
		{
			return m_pSceneManager.get<Node<Root>>();
		}

		auto getNode(uint32_t index)
		{
			return m_pSceneManager.get(index);
		}


		template <typename T, typename P>
		auto getChild(std::shared_ptr<Node<P>> parent)
		{
			return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(parent));
		}

		template <typename T>
		auto getChild(std::shared_ptr<NodeBase> parent)
		{
			return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(parent));
		}

		template <typename T>
		auto getChild()
		{
			return m_pSceneManager.to<T>(m_pSceneManager.getChild<T>(m_pSceneManager.root));
		}

		void BeginScene()
		{
			if (m_pContext->Window().getSize().first > 0) {
				auto renderer = m_pContext->Renderer();
				renderer->BeginFrame();
				renderer->Begin();

				m_pMeshCount = 0;
			}
		}

		void Flush(engine::DrawType drawType = engine::DrawType::INDEXED)
		{

			if (m_pContext->Window().getSize().first > 0) {
				auto renderer = m_pContext->Renderer();

				renderer->Flush(m_pCurrentRenderPass, drawType);
			}
		}

		void Push(std::shared_ptr<Node<RenderModel>> renderModel, glm::mat4 transform, const std::string& layout)
		{
			if (m_pContext->Window().getSize().first > 0) {
				auto renderer = m_pContext->Renderer();

				Ref<PushConstant> push_constant;

				Ref<BindGroup> selectedBindGroup = m_pRenderLayouts[layout].bindGroup;
;
				for (auto& mesh : renderModel->data.renderMeshes)
				{
					if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
						Flush();
					}

					uint32_t material_indices[4] = { 0 };

					for (int i = 0; i < mesh.numMaterials; i++) {
						uint32_t uniform_index = m_pAssetManager.m_pMaterials.at(mesh.material_keys[i]).index;
						material_indices[i] = uniform_index;
					}


					m_pAssetManager.getTransformBuffer()[m_pMeshCount] = transform;

					m_pAssetManager.getObjectBuffer()[m_pMeshCount] = {
						.object_id = (unsigned int)renderModel->Parent()->Index(),
						.transform_index = m_pMeshCount,
						.material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
						.numMaterials = (uint32_t)mesh.numMaterials
					};

					renderer->Push({ .mesh = mesh.mesh,
									.material = selectedBindGroup,
									.pushConstant = push_constant,
									.objectConstant = {
														.transform = transform,
														.material_index = material_indices[0]
													  },
									.layout_index = m_pRenderLayouts[layout].pipelineLayoutIndex,
									.uniformIndex = m_pMeshCount });
					m_pMeshCount++;
				}
			}
		}

		void Push(const std::vector<glm::mat4>& transforms, const std::string& layout, const std::string material, unsigned int count = 1) {

			if (m_pContext->Window().getSize().first > 0) {
				auto renderer = m_pContext->Renderer();

				Ref<PushConstant> push_constant;

				Ref<BindGroup> selectedBindGroup = m_pRenderLayouts[layout].bindGroup;
				
				uint32_t material_indices[4] = { 0 };
				uint32_t firstInstance = m_pMeshCount;
				for (int i = 0; i < count; i++) {
					if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
						Flush();
					}


					for (int j = 0; j < 1; j++) {
						uint32_t uniform_index = m_pAssetManager.m_pMaterials.at(std::hash<std::string>{}(material)).index;
						material_indices[j] = uniform_index;
					}


					m_pAssetManager.getTransformBuffer()[m_pMeshCount] = transforms[i];

					m_pAssetManager.getObjectBuffer()[m_pMeshCount] = {
						.object_id = (unsigned int)i,
						.transform_index = m_pMeshCount,
						.material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
						.numMaterials = (uint32_t)1
					};

					m_pMeshCount++;
				}

				renderer->Push({ .mesh = engine::Ref<Mesh>().makeEmpty(),
								.material = selectedBindGroup,
								.pushConstant = push_constant,
								.objectConstant = {
													.transform = transforms[0],
													.material_index = material_indices[0]
												  },
								.layout_index = m_pRenderLayouts.at(layout).pipelineLayoutIndex,
								.uniformIndex = firstInstance,
								.count = count });

			}
		}

		void Push(std::shared_ptr<Node<RenderModel>> renderModel, const std::vector<glm::mat4>& transforms, const std::string& layout, unsigned int count = 1)
		{
			if (m_pContext->Window().getSize().first > 0) {
				auto renderer = m_pContext->Renderer();

				Ref<PushConstant> push_constant;

				Ref<BindGroup> selectedBindGroup = m_pRenderLayouts[layout].bindGroup;
				

				if (count > 1) {

				}

				for (auto& mesh : renderModel->data.renderMeshes)
				{
					uint32_t material_indices[4] = { 0 };
					uint32_t firstInstance = m_pMeshCount;
					for (int i = 0; i < count; i++) {
						if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
							Flush();
						}


						for (int j = 0; j < mesh.numMaterials; j++) {
							uint32_t uniform_index = m_pAssetManager.m_pMaterials.at(mesh.material_keys[j]).index;
							material_indices[j] = uniform_index;
						}


						m_pAssetManager.getTransformBuffer()[m_pMeshCount] = transforms[i];

						m_pAssetManager.getObjectBuffer()[m_pMeshCount] = {
							.object_id = (unsigned int)renderModel->Index() + i,
							.transform_index = m_pMeshCount,
							.material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
							.numMaterials = (uint32_t)mesh.numMaterials
						};

						m_pMeshCount++;
					}

					renderer->Push({ .mesh = mesh.mesh,
									.material = selectedBindGroup,
									.pushConstant = push_constant,
									.objectConstant = {
														.transform = transforms[0],
														.material_index = material_indices[0]
													  },
									.layout_index = m_pRenderLayouts.at(layout).pipelineLayoutIndex,
									.uniformIndex = firstInstance,
									.count = count });
				}
			}

		}

		void EndScene()
		{
			if (m_pContext->Window().getSize().first > 0) {
				auto renderer = m_pContext->Renderer();
				glm::vec3 v;
				renderer->End(v);

				renderer->Submit();
				renderer->EndFrame();
			}
		}

		void Destroy() {
			m_pAssetManager.Destroy();
		}

	};
}