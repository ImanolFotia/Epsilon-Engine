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

	class Scene
	{
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

		struct OctreeSceneItem {
			OctreeSceneItem() = default;
			int index;
			int instance_index;
			bool visible;
		};

		std::unordered_map<std::string, RenderLayout> m_pRenderLayouts;

		Ref<RenderPass> m_pCurrentRenderPass;

		std::shared_ptr<OctreeContainer<OctreeSceneItem>> m_pOctree;

		Frustum m_pFrustum;

		std::shared_ptr<Context> m_pContext;

		uint32_t m_pMeshCount = 0;

		std::size_t getHash(const std::string& s) {
			return std::hash<std::string>{}(s);
		}

	public:
		Scene() = default;

		Scene(std::shared_ptr<Context> context) : m_pContext(context)
		{
			m_pAssetManager.m_pContext = m_pContext;


			m_pCurrentRenderPass = m_RenderPassesRefs["DefaultRenderPass"];

			m_pOctree = std::make_shared<OctreeContainer<OctreeSceneItem>>(Box { glm::vec3(100, 25, 100), glm::vec3(0.0, 12.5, 0.0) }, 8);
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

			m_pAssetManager.Init();

			engine::BindGroupInfo defaultBindGroup = {
					.bindingInfo = {
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						{.size = 64, .offset = 0, .binding = 4, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "info_buffer"},
						{.size = sizeof(glm::mat4) * AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 5, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"},
						{.size = sizeof(ShaderObjectData) * AssetManager::MAX_OBJECTS, .offset = 0, .binding = 6, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"}
			},
					.inputs = {
					{.renderPass = "ShadowPass", .index = 0, .bindingPoint = 2},
					{.renderPass = "ShadowPass", .index = 1, .bindingPoint = 3},
				},
					.renderPass = "DefaultRenderPass",
					.name = "DefaultBindGroup",
			};

			engine::BindGroupInfo animatedBindGroup = {
					.bindingInfo = {
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						{.size = 64, .offset = 0, .binding = 4, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "info_buffer"},
						{.size = sizeof(glm::mat4) * AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 5, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"},
						{.size = sizeof(ShaderObjectData) * AssetManager::MAX_OBJECTS, .offset = 0, .binding = 6, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"},
						{.size = sizeof(GPUAnimationData), .offset = 0, .binding = 7, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "animation_transform_buffer"}

			},
					.inputs = {
					{.renderPass = "ShadowPass", .index = 0, .bindingPoint = 2},
					{.renderPass = "ShadowPass", .index = 1, .bindingPoint = 3},
				},
					.renderPass = "DefaultRenderPass",
					.name = "AnimatedBindGroup",
			};

			engine::BindGroupInfo decalBindGroup = {
					.bindingInfo = {
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},

			},
					.inputs = {
				},
					.renderPass = "DefaultRenderPass",
					.name = "DecalBindGroup",
			};

			engine::BindGroupInfo shadowBindGroup = {
					.bindingInfo = {
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},

			},
					.inputs = {},
					.renderPass = "ShadowPass",
					.name = "ShadowBindGroup",
			};


			engine::BindGroupInfo treeShadowBindGroup = {
					.bindingInfo = {
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},

						{.size = sizeof(glm::mat4) * AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 5, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"},
						{.size = sizeof(ShaderObjectData) * AssetManager::MAX_OBJECTS, .offset = 0, .binding = 6, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"}

			},
					.inputs = {},
					.renderPass = "ShadowPass",
					.name = "TreeShadowBindGroup",
			};

			engine::BindGroupInfo animateShadowBindGroup = {
					.bindingInfo = {
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						{.size = sizeof(GPUAnimationData), .offset = 0, .binding = 2, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "animation_transform_buffer"}

			},
					.inputs = {},
					.renderPass = "ShadowPass",
					.name = "AnimatedShadowBindGroup",
			};


			engine::BindGroupInfo prepassBindGroup = {
					.bindingInfo = {
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						{.size = sizeof(glm::mat4) * AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 5, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"},
						{.size = sizeof(ShaderObjectData) * AssetManager::MAX_OBJECTS, .offset = 0, .binding = 6, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"}

			},
					.inputs = {},
					.renderPass = "DepthPrePass",
					.name = "PrePassBindGroup",
			};


			m_pRenderLayouts["DefaultLayout"] = { 0, resourceManager->createBindGroup(defaultBindGroup) };
			m_pRenderLayouts["SkyLayout"] = { 1, m_pRenderLayouts["DefaultLayout"].bindGroup};
			m_pRenderLayouts["TerrainLayout"] = {2, m_pRenderLayouts["DefaultLayout"].bindGroup };
			m_pRenderLayouts["DecalLayout"] = {3, resourceManager->createBindGroup(decalBindGroup) };
			m_pRenderLayouts["TreeLayout"] = {4, m_pRenderLayouts["DefaultLayout"].bindGroup };
			m_pRenderLayouts["defaultAnimatedLayout"] = {5, resourceManager->createBindGroup(animatedBindGroup) };

			m_pRenderLayouts["ShadowLayout"] = {0, resourceManager->createBindGroup(shadowBindGroup) };
			m_pRenderLayouts["treeShadowLayout"] = { 1, resourceManager->createBindGroup(treeShadowBindGroup) };
			m_pRenderLayouts["animatedShadowLayout"] = {2,  resourceManager->createBindGroup(animateShadowBindGroup) };

			m_pRenderLayouts["prepassLayout"] = {0, resourceManager->createBindGroup(prepassBindGroup) };


			m_pContext->Renderer()->InitDebugRenderer();

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

		template<typename T>
		void removeFromScene(uint32_t index) {
			std::shared_ptr<Node<T>> node = std::static_pointer_cast<Node<T>>(m_pSceneManager.get(index));
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

			OctreeSceneItem octreeItem;
			octreeItem.index = scene_node->Index();
			octreeItem.instance_index= - 1;
			octreeItem.visible = true;

			m_pOctree->insert(boundingBox, octreeItem);
			
			return scene_node;
		}

		template <typename T, class... Args>
		auto emplaceIntoScene(Box boundingBox, Args &&...args)
		{
			auto scene_node = m_pSceneManager.emplace<T>(m_pSceneManager.root, std::forward<Args>(args)...);

			OctreeSceneItem octreeItem;
			octreeItem.index = scene_node->Index();
			octreeItem.instance_index = -1;
			octreeItem.visible = true;

			m_pOctree->insert(boundingBox, octreeItem);

			return scene_node;
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