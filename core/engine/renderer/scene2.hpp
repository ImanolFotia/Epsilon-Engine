#pragma once

#include "resource_manager.hpp"
#include "renderer.hpp"
#include "../context.hpp"
#include "types.hpp"

#include "../../common/common.hpp"

#include <core/framework/entity_component/ecs.hpp>

namespace engine {

	struct PBRMaterial {
		int32_t albedo_texture_index = -1;
		int32_t normal_texture_index = -1;
		int32_t metallic_texture_index = -1;
		int32_t roughnes_texture_index = -1;

		glm::vec4 albedo_color{};
		float metallic_color{};
		float roughness_color{};
	};

	struct RenderInfo {
		int32_t mesh_index = -1;
		int32_t material_index = -1;
		int32_t push_constant_index = -1;
		int32_t layout_index = -1;
		PBRMaterial material;
		glm::mat4 transform = glm::mat4(1.0f);
	};

	struct AudioInfo {
		int32_t source_index = -1;
		bool should_play = false;
	};


	struct EntityNode {
		std::string name;
		RenderInfo renderInfo{};
		AudioInfo audioInfo{};
		custom_type userType;
		std::list< std::shared_ptr<EntityNode>> children;
	};


	struct ScenePushConstant {
		alignas(32) glm::mat4 modelMatrix{};
		glm::ivec4 texture_indices;
	};

	struct SceneTreeContainer {

		using IndexContainer = std::unordered_map<std::string, uint32_t>;

		std::vector<Ref<Mesh>> m_pMeshes;
		std::vector<Ref<Material>> m_pMaterials;
		std::vector<Ref<Texture>> m_pTextures;

		std::vector<Ref<PushConstant>> m_pPushConstantsRefs;
		std::list<std::shared_ptr<ScenePushConstant>> m_pPushConstants;

		IndexContainer m_pMeshesIndexes;
		IndexContainer m_pMaterialsIndexes;
		IndexContainer m_pTexturesIndexes;
		IndexContainer m_pLayoutIndexes;
		IndexContainer m_pPushConstantIndexes;
		IndexContainer m_pPushConstantRefsIndexes;

		std::unordered_map<std::string, std::shared_ptr<EntityNode>> m_pNodeMap;

		std::shared_ptr<EntityNode> m_pRootNode;

		std::shared_ptr<EntityNode> root() {
			return m_pRootNode;
		}

		Ref<Mesh> mesh(int32_t index) {
			return m_pMeshes.at(index);
		}

		Ref<Material> material(int32_t index) {
			return m_pMaterials.at(index);
		}

		Ref<Texture> texture(int32_t index) {
			return m_pTextures.at(index);
		}

		auto& NodeMap() {
			return m_pNodeMap;
		}

		std::shared_ptr<EntityNode> Node(std::string key) {
			return m_pNodeMap.at(key);
		}
	};

	template<class container_type = SceneTreeContainer>
	class SceneManager {

		std::hash<std::string> hash_var(const std::string& str) {
			return std::hash<std::string>{}(str);
		}


		using MaterialIndex = uint32_t;
		using MeshIndex = uint32_t;

		container_type m_pContainer;

	public:
		SceneManager() {
			m_pContainer.m_pRootNode = std::make_shared<EntityNode>();
		}

		std::shared_ptr<EntityNode> at(const std::string& name) {
			return m_pContainer.m_pNodeMap.at(name);
		}

		void insert(EntityNode node, const std::string& parent_node) {

			using NodePtr = std::shared_ptr<EntityNode>;
			NodePtr parent = nullptr;
			if (parent_node == "root") {

				NodePtr parent = m_pContainer.m_pRootNode;
			}
			else {
				if (!m_pContainer.NodeMap().contains(parent_node)) return;

				NodePtr parent = m_pContainer.NodeMap().at(parent_node);
			}

			NodePtr new_node = parent->children.push_back(
				std::make_shared<EntityNode>(
					std::move(node)
				)
			);

			m_pContainer.NodeMap()[node.name] = new_node;
		}

		Ref<Mesh> getMesh(int32_t index) {
			m_pContainer.m_pMeshes.at(index);
		}

		Ref<Material> getMaterial(int32_t index) {
			m_pContainer.m_pMaterials.at(index);
		}

		Ref<PushConstant> getPushConstant(int32_t index) {
			m_pContainer.m_pPushConstant.at(index);
		}

		MaterialIndex addMaterial(const std::string& name, MaterialInfo material_info) {

			auto resourceManager = Context::getSingleton().ResourceManager();

			size_t pos_index = m_pContainer.m_pMaterials.size();

			m_pContainer.m_pMaterialsIndexes[name] = pos_index;

			auto material = resourceManager->createMaterial(material_info);
			m_pContainer.m_pMaterials.push_back(material);

			return pos_index;
		}

		MeshIndex addMesh(const std::string& name, common::Mesh mesh) {

			auto resourceManager = Context::getSingleton().ResourceManager();

			size_t pos_index = m_pContainer.m_pMeshes.size();

			auto tMesh = resourceManager->createMesh({
				.vertices = mesh.Vertices,
				.indices = mesh.Indices,
				.name = name
			});

			m_pContainer.m_pMeshesIndexes[name] = pos_index;
			m_pContainer.m_pMeshes.push_back(tMesh);

			return pos_index;
		}

		MeshIndex addTexture(const std::string& name, TextureCreationInfo texInfo) {

			auto resourceManager = Context::getSingleton().ResourceManager();

			size_t pos_index = m_pContainer.m_pTextures.size();

			auto tTexture = resourceManager->createTexture(texInfo);

			m_pContainer.m_pTextures.push_back(tTexture);
			m_pContainer.m_pTexturesIndexes[name] = pos_index;

			return pos_index;

		}

		MeshIndex addPushConstant(const std::string& name, ScenePushConstant&& pushConstant) {

			size_t pos_index = m_pContainer.m_pPushConstants.size();

			auto resourceManager = Context::getSingleton().ResourceManager();

			m_pContainer.m_pPushConstants.push_back(pushConstant);

			auto& pushConst = std::prev(m_pContainer.m_pPushConstants.end());

			auto ref = resourceManager->createPushConstant({
				name,
				{.size = sizeof(ScenePushConstant), .data = &pushConst }
			});

			m_pContainer.m_pPushConstantsRefs.push_back(ref);

			m_pContainer.m_pPushConstantRefsIndexes[name] = pos_index;
			m_pContainer.m_pPushConstantIndexes[name] = pos_index;

			return pos_index;

		}

		void addLayoutIndex(const std::string& name, uint32_t index) {
			m_pContainer.m_pLayoutIndexes[name] = index;
		}

		uint32_t getLayoutIndex(const std::string& name) {
			return m_pContainer.m_pLayoutIndexes.at(name);
		}

		uint32_t getMeshIndex(const std::string& name) {
			return m_pContainer.m_pMeshIndexes.at(name);
		}

		uint32_t getMaterialIndex(const std::string& name) {
			return m_pContainer.m_pMaterialIndexes.at(name);
		}

		uint32_t getPushConstantIndex(const std::string& name) {
			return m_pContainer.m_pPushConstantRefsIndexes.at(name);
		}

		auto root() {
			return m_pContainer.root();
		}
	};

}