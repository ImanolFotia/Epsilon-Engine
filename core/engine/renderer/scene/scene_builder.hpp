#pragma once

#include "resource_manager.hpp"
#include "renderer.hpp"
#include "../context.hpp"
#include "types.hpp"
#include <core/framework/audio/WAVFile.h>
#include <core/framework/loaders/model.h>

#include "../../common/common.hpp"

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

	struct SubRenderComponent {
		Ref<Mesh> mesh;
		Ref<Material> material;
		PBRMaterial material;
		glm::mat4 transform = glm::mat4(1.0f);
	};

	struct RenderComponent {
		std::vector<SubRenderComponent> renderComponents;
		int32_t layout_index = -1;
		glm::mat4 transform = glm::mat4(1.0f);
	};

	static RenderComponent createRenderComponent(const std::string& name, framework::Model model) {
		int index = 0;
		for (auto& mesh : model.Meshes()) {
			SubRenderComponent subRenderC;
			subRenderC.mesh = addMesh(name + "_submesh_" + std::to_string(index), mesh.data().mesh);
			subRenderC.material = addMaterial(name + "_material", mesh.data().mesh);
			index++;
		}
	}

	static RenderInfo createRenderInfo(const std::vector<common::Vertex>& vertices, const std::vector<uint32_t>& indices) {

	}


	Ref<Material> addMaterial(const std::string& name, MaterialInfo material_info) {

		auto resourceManager = Context::getSingleton().ResourceManager();

		auto material = resourceManager->createMaterial(material_info);

		m_pContainer.m_pMaterials.push_back(material);

		return pos_index;
	}

	Ref<Mesh> addMesh(const std::string& name, common::Mesh mesh) {

		auto resourceManager = Context::getSingleton().ResourceManager();


		auto ref = resourceManager->createMesh({
			.vertices = mesh.Vertices,
			.indices = mesh.Indices,
			.name = name
			});


		return ref;
	}

	Ref<Texture> addTexture(const std::string& name, TextureCreationInfo texInfo) {

		auto resourceManager = Context::getSingleton().ResourceManager();

		auto ref = resourceManager->createTexture(texInfo);

		return ref;

	}

	Ref<PushConstant> addPushConstant(const std::string& name, ScenePushConstant&& pushConstant) {

		auto resourceManager = Context::getSingleton().ResourceManager();

		auto ref = resourceManager->createPushConstant({
			name,
			{.size = sizeof(ScenePushConstant), .data = &pushConst }
			});


		return ref;

	}
}