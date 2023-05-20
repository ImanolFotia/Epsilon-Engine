#pragma once

#include <unordered_map>
#include <string>
#include <queue>

#include <core/engine/renderer/resource_manager.hpp>
#include <core/framework/loaders/model.h>
#include <core/framework/loaders/gltf.hpp>

#include "audio/audio_object.hpp"

namespace engine
{

	struct ShaderObjectData {
		uint32_t transform_index;
		uint32_t material_index;
	};

	struct PBRMaterial
	{
		uint32_t albedo_texture_index = -1;
		uint32_t normal_texture_index = -1;
		uint32_t metallic_texture_index = -1;
		uint32_t roughness_texture_index = -1;

		alignas(16) glm::vec4 albedo_color{};
		float metallic_color{};
		float roughness_color{};
	};

	struct PBRMaterialIndex {
		PBRMaterial material;
		uint32_t index;
	};

	struct RenderMesh
	{
		Ref<Mesh> mesh;
		Ref<BindGroup> bind_group;
		std::size_t material_key;
	};

	struct RenderModel
	{
		std::vector<RenderMesh> renderMeshes;
		std::string name;
	};


	struct TextureInfo
	{
		TextureFormat format;
		WrapMode wrapMode;
		Filtering filtering;
	};

	

	class AssetManager
	{
		std::unordered_map<std::string, Ref<Texture>> m_pImages;
		std::unordered_map<std::string, RenderModel> m_pModels;
		std::unordered_map<std::string, AudioBuffer> m_pAudioBuffers;
		std::unordered_map<std::size_t, PBRMaterialIndex> m_pMaterials;

		std::unordered_map<std::string, Ref<Buffer>> m_pGPUBuffers;

		std::queue<uint32_t> m_pFreeMaterialIndexes;
		uint32_t m_pMaterialCurrentIndex = 0;

		std::queue<uint32_t> m_pFreeObjectIndexes;
		uint32_t m_pObjectCurrentIndex = 0;

		friend class Scene;

	public:

		static const size_t MAX_MATERIALS = 10000;
		static const size_t MAX_OBJECTS = 10000;
		static const size_t MAX_TRANSFORMS = 10000;
		AssetManager()
		{
			
		}

		void Init() {
			auto resourceManager = Context().ResourceManager();

			m_pGPUBuffers["material_buffer"] = resourceManager->createGPUBuffer("material_buffer", sizeof(PBRMaterial) * MAX_MATERIALS, engine::BufferStorageType::STORAGE_BUFFER);
			//m_pGPUBuffers["object_buffer"] = resourceManager->createGPUBuffer("object_buffer", sizeof(ShaderObjectData) * MAX_OBJECTS, engine::BufferStorageType::STORAGE_BUFFER);
			//m_pGPUBuffers["transform_buffer"] = resourceManager->createGPUBuffer("transform_buffer", sizeof(glm::mat4) * MAX_TRANSFORMS, engine::BufferStorageType::STORAGE_BUFFER);
		}

		const RenderModel& createModelFromMesh(const std::string name, const common::Mesh& mesh, const common::MeshMaterial& material) {

			auto resourceManager = Context().ResourceManager();

			if (m_pModels.contains(name)) {
				return m_pModels.at(name);
			}

			RenderModel& model = m_pModels[name];
			model.name = name;

			std::string material_name = name + "_material";
			size_t material_name_hash = std::hash<std::string>{}(material_name);

			RenderMesh subRenderC;
			subRenderC.material_key = material_name_hash;
			subRenderC.mesh = addMesh(name, mesh);
			model.renderMeshes.push_back(subRenderC);

			PBRMaterial pbr_material;



			if (m_pMaterials.contains(material_name_hash)) {
				return model;
			}


			pbr_material.albedo_color = material.color;
			pbr_material.metallic_color = material.metallic;
			pbr_material.roughness_color = material.roughness;

			if (!material.albedo_path.empty())
			{
				auto albedo = addTexture(material.albedo_path, { .format = COLOR_RGBA,
																.wrapMode = REPEAT,
																.filtering = LINEAR });
				pbr_material.albedo_texture_index = albedo.Index();
			}
			if (!material.metallic_path.empty())
			{
				auto metallic = addTexture(material.metallic_path, { .format = NON_COLOR_RGBA,
																	.wrapMode = REPEAT,
																	.filtering = LINEAR });
				pbr_material.metallic_texture_index = metallic.Index();
			}
			if (!material.roughness_path.empty())
			{
				auto roughness = addTexture(material.roughness_path, { .format = NON_COLOR_RGBA,
																	  .wrapMode = REPEAT,
																	  .filtering = LINEAR });
				pbr_material.roughness_texture_index = roughness.Index();
			}
			if (!material.normal_path.empty())
			{
				auto normal = addTexture(material.normal_path, { .format = NON_COLOR_RGBA,
																.wrapMode = REPEAT,
																.filtering = LINEAR });
				pbr_material.normal_texture_index = normal.Index();
			}

			uint32_t mat_index = 0;
			if (!m_pFreeMaterialIndexes.empty()) {
				mat_index = m_pFreeMaterialIndexes.front();
				m_pFreeMaterialIndexes.pop();
			}
			else {
				mat_index = m_pMaterialCurrentIndex;
				m_pMaterialCurrentIndex++;
			}


			PBRMaterial* materialBufferPtr = reinterpret_cast<PBRMaterial*>(resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"]));

			materialBufferPtr[mat_index] = pbr_material;

			m_pMaterials[material_name_hash].index = mat_index;
			m_pMaterials[material_name_hash].material = pbr_material;


			resourceManager->unmapBuffer(m_pGPUBuffers["material_buffer"]);


			return model;
		}

		ShaderObjectData createObjectData(const RenderModel& model) {

			auto resourceManager = Context().ResourceManager();
			ShaderObjectData* objectBufferPtr = reinterpret_cast<ShaderObjectData*>(resourceManager->mapBuffer(m_pGPUBuffers["object_buffer"]));

			for (auto& mesh : model.renderMeshes) {
				auto index = getShaderDataIndex(); 
				objectBufferPtr[index].material_index = m_pMaterials.at(mesh.material_key).index;
			}

			resourceManager->unmapBuffer(m_pGPUBuffers["object_buffer"]);
		}

		const RenderModel &loadModel(const std::string &path)
		{
			auto resourceManager = Context().ResourceManager();

			if (m_pModels.contains(path))
				return m_pModels.at(path);

			//if()
			const std::string ext = path.substr(path.find_last_of('.')+1, path.length());

			std::unique_ptr<framework::ModelBase> inModel;

			if (ext == "eml") {
				inModel = std::make_unique<framework::Model>("./assets/" + path);
			}
			else {
				inModel = std::make_unique<framework::gltfModel>("./assets/" + path);
			}

			int index = 0;
			RenderModel &model = m_pModels[path];


			model.name = path;
			for (auto &mesh : inModel->Meshes())
			{
				RenderMesh subRenderC;
				subRenderC.mesh = addMesh(path + "_submesh_" + std::to_string(index), mesh.data().mesh);

				std::string material_name = path + "_submesh_" + std::to_string(index) + "_material";

				subRenderC.material_key = std::hash<std::string>{}(material_name);

				model.renderMeshes.push_back(subRenderC);

				auto mesh_material = mesh.Material();

				PBRMaterial material;

				size_t material_name_hash = std::hash<std::string>{}(material_name);

				if (m_pMaterials.contains(material_name_hash)) {
					model.renderMeshes.push_back(subRenderC);
					continue;
				}

				if (!mesh_material.albedo.empty())
				{
					auto albedo = addTexture(mesh_material.albedo, {.format = COLOR_RGBA,
																	.wrapMode = REPEAT,
																	.filtering = LINEAR});
					material.albedo_texture_index = albedo.Index();
				}
				if (!mesh_material.metallic.empty())
				{
					auto metallic = addTexture(mesh_material.metallic, {.format = NON_COLOR_RGBA,
																		.wrapMode = REPEAT,
																		.filtering = LINEAR});
					material.metallic_texture_index = metallic.Index();
				}
				if (!mesh_material.roughness.empty())
				{
					auto roughness = addTexture(mesh_material.roughness, {.format = NON_COLOR_RGBA,
																		  .wrapMode = REPEAT,
																		  .filtering = LINEAR});
					material.roughness_texture_index = roughness.Index();
				}
				if (!mesh_material.normal.empty())
				{
					auto normal = addTexture(mesh_material.normal, {.format = NON_COLOR_RGBA,
																	.wrapMode = REPEAT,
																	.filtering = LINEAR});
					material.normal_texture_index = normal.Index();
				}

				m_pMaterials[material_name_hash].material = material;
				subRenderC.material_key = material_name_hash;

				PBRMaterial* materialBufferPtr = reinterpret_cast<PBRMaterial*>(resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"]));

				uint32_t mat_index = 0;
				if (!m_pFreeMaterialIndexes.empty()) {
					mat_index = m_pFreeMaterialIndexes.front();
					m_pFreeMaterialIndexes.pop();
				}
				else {
					mat_index = m_pMaterialCurrentIndex;
					m_pMaterialCurrentIndex++;
				}

				m_pMaterials[material_name_hash].index = mat_index;

				materialBufferPtr[mat_index] = material;

				resourceManager->unmapBuffer(m_pGPUBuffers["material_buffer"]);

				model.renderMeshes.push_back(subRenderC);

				index++;
			}
			return model;
		}

		const AudioObject &loadAudio(const std::string &path)
		{

			auto audioManager = Context::getSingleton().AudioManager();

			if (m_pAudioBuffers.contains(path))
			{
				AudioBuffer &buffer = m_pAudioBuffers.at(path);
				buffer.sources.push_back(audioManager->createSource(path + "_source_" + std::to_string(buffer.sources.size()), {.buffer = buffer.buffer}));

				AudioObject object;
				object.buffer = buffer;
				object.source = buffer.sources.back();

				return object;
			}
			framework::WAVfile audioFile;
			audioFile.Load(("./assets/" + path).c_str());

			auto buffer = audioManager->createBuffer(path, {
															   .numChannels = audioFile.getNumberOfChannels(),
															   .size = audioFile.getFileSize(),
															   .bps = audioFile.getBPS(),
															   .bitrate = audioFile.getSampleRate(),
															   .data = audioFile.data().get(),
														   });

			AudioBuffer &audioBuffer = m_pAudioBuffers[path];
			audioBuffer.buffer = buffer;

			audioBuffer.sources.push_back(audioManager->createSource(path + "_source_" + std::to_string(audioBuffer.sources.size()), {.buffer = audioBuffer.buffer}));

			AudioObject object;
			object.buffer = audioBuffer;
			object.source = audioBuffer.sources.back();

			return object;
		}



	private:

		uint32_t getShaderDataIndex() {

			uint32_t obj_index = 0;
			if (!m_pFreeObjectIndexes.empty()) {
				obj_index = m_pFreeObjectIndexes.front();
				m_pFreeObjectIndexes.pop();
			}
			else {
				obj_index = m_pObjectCurrentIndex;
				m_pObjectCurrentIndex++;
			}

			return obj_index;

		}

		Ref<Texture> addTexture(const std::string &path, const TextureInfo &info)
		{

			auto resourceManager = Context::getSingleton().ResourceManager();

			if (m_pImages.contains(path))
			{
				return m_pImages.at(path);
			}

			int width, height, num_channels;
			std::string texture_path = "./assets/" + path;
			unsigned char *pixels = framework::load_image_from_file(texture_path.c_str(), &width, &height, &num_channels);
			Ref<Texture> ref;
			if (pixels != nullptr)
			{
				engine::TextureCreationInfo texInfo = {
					.width = (uint32_t)width,
					.height = (uint32_t)height,
					.numChannels = (uint32_t)num_channels,
					.format = info.format,
					.wrapMode = info.wrapMode,
					.filtering = info.filtering,
					.name = path,
					.pixels = pixels,
				};

				ref = resourceManager->createTexture(texInfo);

				framework::free_image_data(pixels);

				m_pImages[path] = ref;
			}

			return ref;
		}

		Ref<Mesh> addMesh(const std::string &name, common::Mesh mesh)
		{

			auto resourceManager = Context::getSingleton().ResourceManager();

			auto ref = resourceManager->createMesh({.vertices = mesh.Vertices,
													.indices = mesh.Indices,
													.name = name});

			return ref;
		}
	};
}