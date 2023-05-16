#pragma once

#include <unordered_map>
#include <string>
#include <queue>

#include <core/engine/renderer/resource_manager.hpp>
#include <core/framework/audio/WAVFile.h>
#include <core/framework/loaders/model.h>

namespace engine
{

	struct ShaderObjectData {
		glm::mat4 transform;
		uint32_t material_index;
	};

	struct PBRMaterial
	{
		uint32_t albedo_texture_index = -1;
		uint32_t normal_texture_index = -1;
		uint32_t metallic_texture_index = -1;
		uint32_t roughness_texture_index = -1;

		glm::vec4 albedo_color{};
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
		PBRMaterial pbr_material;
		uint32_t buffer_index;
		glm::mat4 transform = glm::mat4(1.0f);
	};

	struct RenderModel
	{
		std::vector<RenderMesh> renderMeshes;
		int32_t layout_index = -1;
		glm::mat4 transform = glm::mat4(1.0f);
	};

	struct AudioBuffer
	{
		Ref<audio::AudioBuffer> buffer;
		std::vector<Ref<audio::AudioSource>> sources;
	};

	struct TextureInfo
	{
		TextureFormat format;
		WrapMode wrapMode;
		Filtering filtering;
	};

	class AudioObject
	{

		Ref<audio::AudioSource> source{};
		AudioBuffer buffer{};
		float angle{};
		float gain{};
		glm::vec3 position{};
		glm::vec3 direction{};
		glm::vec3 velocity{};

		friend class AssetManager;

	public:
		// Setters
		void Position(glm::vec3 p)
		{
			auto audioManager = Context::getSingleton().AudioManager();
			audioManager->setSourcePosition(source, p);
		}

		void Direction(glm::vec3 d)
		{
			auto audioManager = Context::getSingleton().AudioManager();
			audioManager->setSourceDirection(source, d);
		}

		void Velocity(glm::vec3 v)
		{
			auto audioManager = Context::getSingleton().AudioManager();
			audioManager->setSourceVelocity(source, v);
		}

		void Angle(float a)
		{
			auto audioManager = Context::getSingleton().AudioManager();
			audioManager->setSourceAngle(source, a);
		}

		void Gain(float g)
		{
			auto audioManager = Context::getSingleton().AudioManager();
			audioManager->setSourceGain(source, g);
		}

		void Pitch(float p)
		{
			auto audioManager = Context::getSingleton().AudioManager();
			audioManager->setSourcePitch(source, p);
		}

		// Getters
		glm::vec3 Position()
		{
			auto audioManager = Context::getSingleton().AudioManager();
			return audioManager->getSourcePosition(source);
		}

		glm::vec3 Direction()
		{
			auto audioManager = Context::getSingleton().AudioManager();
			return audioManager->getSourceDirection(source);
		}

		glm::vec3 Velocity()
		{
			auto audioManager = Context::getSingleton().AudioManager();
			return audioManager->getSourceVelocity(source);
		}

		float Angle()
		{
			auto audioManager = Context::getSingleton().AudioManager();
			return audioManager->getSourceAngle(source);
		}

		float Gain()
		{
			auto audioManager = Context::getSingleton().AudioManager();
			return audioManager->getSourceGain(source);
		}

		float Pitch()
		{
			auto audioManager = Context::getSingleton().AudioManager();
			return audioManager->getSourcePitch(source);
		}
	};

	class AssetManager
	{


		std::unordered_map<std::string, Ref<Texture>> m_pImages;
		std::unordered_map<std::string, RenderModel> m_pModels;
		std::unordered_map<std::string, AudioBuffer> m_pAudioBuffers;
		std::unordered_map<std::string, PBRMaterialIndex> m_pMaterials;

		std::unordered_map<std::string, Ref<Buffer>> m_pGPUBuffers;

		std::queue<uint32_t> m_pFreeMaterialIndexes;
		uint32_t m_pMaterialCurrentIndex = 0;


	public:

		static const size_t MAX_MATERIALS = 10000;
		static const size_t MAX_OBJECTS = 10000;
		AssetManager()
		{
			auto resourceManager = Context().ResourceManager();

			m_pGPUBuffers["material_buffer"] = resourceManager->createGPUBuffer("material_buffer", sizeof(PBRMaterial) * MAX_MATERIALS, engine::BufferStorageType::STORAGE_BUFFER);
			m_pGPUBuffers["object_buffer"] = resourceManager->createGPUBuffer("object_buffer", sizeof(ObjectData) * MAX_OBJECTS, engine::BufferStorageType::STORAGE_BUFFER);
		}

		const RenderModel &loadModel(const std::string &path)
		{

			auto resourceManager = Context().ResourceManager();

			if (m_pModels.contains(path))
				return m_pModels.at(path);

			framework::Model inModel("./assets/" + path);

			int index = 0;
			RenderModel &model = m_pModels[path];
			for (auto &mesh : inModel.Meshes())
			{
				RenderMesh subRenderC;
				subRenderC.mesh = addMesh(path + "_submesh_" + std::to_string(index), mesh.data().mesh);
				// subRenderC.material = addMaterial(name + "_material", mesh.data().mesh);
				model.renderMeshes.push_back(subRenderC);

				auto mesh_material = mesh.Material();

				PBRMaterial material;

				std::string material_name = path + "_submesh_" + std::to_string(index) + "_material";

				if (m_pMaterials.contains(material_name)) {
					subRenderC.pbr_material = m_pMaterials.at(material_name).material;
					subRenderC.buffer_index = m_pMaterials.at(material_name).index;
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


				m_pMaterials[material_name].material = material;
				subRenderC.pbr_material = material;

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

				m_pMaterials[material_name].index = mat_index;

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