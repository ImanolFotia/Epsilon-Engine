#pragma once

#include "asset_manager.hpp"
#include "nodes/node_manager.hpp"

#include "trees/octree.hpp"
#include "parsers/renderpass.hpp"

#include "../audio/audio_manager.hpp"

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

		std::unordered_map<std::string, unsigned int> m_pRenderLayouts;

		Ref<RenderPass> m_pCurrentRenderPass;
		Ref<BindGroup> m_pDefaultBindGroup;
		Ref<BindGroup> m_pShadowBindGroup;
		Ref<BindGroup> m_pDecalBindGroup;
		Ref<BindGroup> m_pPrePassBindGroup;

		std::shared_ptr<Context> m_pContext;

		std::size_t getHash(const std::string &s) {
			return std::hash<std::string>{}(s);
		}

	public:
		Scene() = default;

		Scene(std::shared_ptr<Context> context) : m_pContext(context)
		{
			m_pAssetManager.m_pContext = m_pContext;

			m_pRenderLayouts["DefaultLayout"] = 0;
			m_pRenderLayouts["SkyLayout"] = 1;
			m_pRenderLayouts["TerrainLayout"] = 2;
			m_pRenderLayouts["DecalLayout"] = 3;
			m_pRenderLayouts["ShadowLayout"] = 0;
			m_pRenderLayouts["prepassLayout"] = 0;

			m_pCurrentRenderPass = m_RenderPassesRefs["DefaultRenderPass"];

		}

		std::shared_ptr<Context> getContext() { return m_pContext; }

		void Init() {

			auto resourceManager = m_pContext->ResourceManager();

#ifdef _WIN32
			system("cd .\\assets\\shaders && .\\build_shaders.sh");
#endif
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
						{.size = sizeof(ShaderObjectData), .offset = 0, .binding = 0, .type = engine::UniformBindingType::UNIFORM_BUFFER},
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						{.size = sizeof(Decal) * AssetManager::MAX_DECALS, .offset = 0, .binding = 4, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "decal_buffer"},
						//{.size = sizeof(glm::mat4) * AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 3, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"}
			},
					.inputs = {
					{.renderPass = "ShadowPass", .index = 1, .bindingPoint = 2},
					{.renderPass = "ShadowPass", .index = 0, .bindingPoint = 3},
				},
					.renderPass = "DefaultRenderPass",
					.name = "DefaultBindGroup",
			};
			/*
			engine::BindGroupInfo decalBindGroup = {
					.bindingInfo = {
						{.size = sizeof(ShaderObjectData), .offset = 0, .binding = 0, .type = engine::UniformBindingType::UNIFORM_BUFFER},
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},
						//{.size = sizeof(ObjectData) * AssetManager::MAX_OBJECTS, .offset = 0, .binding = 2, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "object_buffer"},
						//{.size = sizeof(glm::mat4) * AssetManager::MAX_TRANSFORMS, .offset = 0, .binding = 3, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "transform_buffer"}
			},
					.inputs = {
				},
					.renderPass = "DefaultRenderPass",
					.name = "DecalBindGroup",
			};*/

			engine::BindGroupInfo shadowBindGroup = {
					.bindingInfo = {
						{.size = sizeof(ShaderObjectData), .offset = 0, .binding = 0, .type = engine::UniformBindingType::UNIFORM_BUFFER},
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},

			},
					.inputs = {},
					.renderPass = "ShadowPass",
					.name = "ShadowBindGroup",
			};


			engine::BindGroupInfo prepassBindGroup = {
					.bindingInfo = {
						{.size = sizeof(ShaderObjectData), .offset = 0, .binding = 0, .type = engine::UniformBindingType::UNIFORM_BUFFER},
						{.size = sizeof(PBRMaterial) * AssetManager::MAX_MATERIALS, .offset = 0, .binding = 1, .type = engine::UniformBindingType::SHADER_STORAGE, .buffer = "material_buffer"},

			},
					.inputs = {},
					.renderPass = "DepthPrePass",
					.name = "PrePassBindGroup",
			};

			m_pDefaultBindGroup = resourceManager->createBindGroup(defaultBindGroup);
			m_pShadowBindGroup = resourceManager->createBindGroup(shadowBindGroup);
			//m_pDecalBindGroup = resourceManager->createBindGroup(decalBindGroup);
			m_pPrePassBindGroup = resourceManager->createBindGroup(prepassBindGroup);

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

		template <typename T>
		auto getNode(uint32_t index)
		{
			return m_pSceneManager.get(index);
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
			auto renderer = m_pContext->Renderer();
			renderer->BeginFrame();
			renderer->Begin();
		}

		void Flush()
		{
			auto renderer = m_pContext->Renderer();

			renderer->Flush(m_pCurrentRenderPass, engine::DrawType::INDEXED);
		}

		void Push(std::shared_ptr<Node<RenderModel>> renderModel, glm::mat4 transform, const std::string& layout)
		{
			auto renderer = m_pContext->Renderer();

			Ref<PushConstant> push_constant;

			Ref<BindGroup> selectedBindGroup;

			if (layout == "ShadowLayout") {
				selectedBindGroup = m_pShadowBindGroup;
			}
			else if (layout == "DefaultLayout") {
				selectedBindGroup = m_pDefaultBindGroup;
			}
			else if (layout == "TerrainLayout") {
				selectedBindGroup = m_pDefaultBindGroup;
			}
			/*else if (layout == "DecalLayout") {
				selectedBindGroup = m_pDecalBindGroup;
			}*/
			else if (layout == "SkyLayout") {
				selectedBindGroup = m_pDefaultBindGroup;
			}
			else if (layout == "prepassLayout") {
				selectedBindGroup = m_pPrePassBindGroup;
			}
			else {
				selectedBindGroup = m_pDefaultBindGroup;
			}

			for (auto& mesh : renderModel->data.renderMeshes)
			{
				if (renderer->numPushedCommands() >= engine::MAX_COMMAND_QUEUE_SIZE) {
					Flush();
				}

				uint32_t uniform_index = m_pAssetManager.m_pMaterials.at(mesh.material_key).index;
				renderer->Push({ .mesh = mesh.mesh,
								.material = selectedBindGroup,
								.pushConstant = push_constant,
								.objectConstant = {
													.transform = transform,
													.material_index = uniform_index
												  },
								.layout_index = m_pRenderLayouts.at(layout),
								.uniformIndex = uniform_index });

			}
		}

		void EndScene()
		{

			auto renderer = m_pContext->Renderer();
			glm::vec3 v;
			renderer->End(v);

			renderer->Submit();
			renderer->EndFrame();
		}

		void Destroy() {
			m_pAssetManager.Destroy();
		}

	};
}