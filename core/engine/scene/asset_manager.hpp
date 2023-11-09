#pragma once

#include <unordered_map>
#include <string>
#include <queue>
#include <any>

#include <core/engine/renderer/resource_manager.hpp>
#include <core/framework/loaders/model.h>
#include <core/framework/loaders/gltf.hpp>

#include <ktx.h>
#include <core/framework/loaders/DDS.h>

#include "audio/audio_object.hpp"

#include "structs/box.hpp"

#include <core/framework/vfs/filesystem.hpp>
#include <stack>

namespace engine
{

	struct ShaderData
	{
		alignas(4) float iTime = 0.0f;
		alignas(8) glm::vec2 iResolution{};
		alignas(8) glm::vec3 lightPosition{};
		alignas(8) glm::vec3 viewPosition{};
		alignas(16) glm::mat4 view{};
		alignas(16) glm::mat4 proj{};
		alignas(16) glm::mat4 lightMatrix{};
		alignas(4) uint32_t iFrame {};
		alignas(8) glm::vec2 iMouse{};
	};
	struct ShaderObjectData {
		uint32_t object_id{};
		uint32_t transform_index{};
		uint32_t material_index[4] = {};
		uint32_t numMaterials = 1;
		uint32_t animationIndex = 0;
	};

	struct CursorInfo {
		glm::vec4 position{};
		alignas(16) glm::vec3 normal{};
		alignas(16) glm::vec3 selected_item_position;
		alignas(4) int id{};
		int selected{};
		int mask{};
		float circle_radius = 5.0;
	};

	enum class TextureSlot : unsigned short {
		Albedo = 0,
		Normal,
		Metallic,
		Roughness
	};

	struct PBRMaterial
	{
		int32_t albedo_texture_index = -1;
		int32_t normal_texture_index = -1;
		int32_t metallic_texture_index = -1;
		int32_t roughness_texture_index = -1;

		alignas(16) glm::vec4 albedo_color = glm::vec4(0.75f, 0.75f, 0.75f, 1.0f);
		float metallic_color = 1.0;
		float roughness_color = 0.038;
		float transmission = -1.0f;

		float specular = 1.0f;
	};

	struct PBRMaterialIndex {
		PBRMaterial material;
		uint32_t index{};
		size_t Slot[4] = {0};
	};

	struct RenderMesh
	{
		unsigned int id = 0;
		Ref<Mesh> mesh{};
		Ref<BindGroup> bind_group{};
		std::size_t material_keys[4];
		std::size_t numMaterials = 0;
	};

	struct RenderModel
	{
		std::vector<RenderMesh> renderMeshes;
		std::vector<framework::Animation> animations;
		std::vector<glm::mat4> transforms{};
		std::vector<std::any> instance_data;
		common::MIN_MAX_POINTS min_max_points;
		framework::Skeleton skeleton;
		std::string name{};
		std::string bindGroup = "DefaultLayout";
		std::string shadowBindGroup = "ShadowLayout";
		uint32_t animationIndex = 0;
		bool hasAnimation = false;
		bool isInstanced = false;
		bool visible = false;
		bool loaded = false;
	};

	struct ShaderAsset {
		std::string name;
		std::vector<std::string> filePaths;
		std::vector<std::string> spirvFilePaths;
	};

	struct GPUAnimationData {
		glm::mat4 transforms[300];
	};

	struct TextureInfo
	{
		TextureFormat format;
		WrapMode wrapMode;
		Filtering filtering;
	};


	struct Decal {
		glm::mat4 transform;
		uint32_t material_id;
	};

	class AssetManager
	{
		std::unordered_map<size_t, Ref<Texture>> m_pImages;
		std::unordered_map<std::string, RenderModel> m_pModels;
		std::unordered_map<std::string, AudioBuffer> m_pAudioBuffers;
		std::unordered_map<std::string, ShaderAsset> m_pShaders;
		std::unordered_map<std::size_t, PBRMaterialIndex> m_pMaterials;

		std::unordered_map<size_t, std::string> m_pImageName;

		std::unordered_map<std::string, Ref<Buffer>> m_pGPUBuffers;

		std::queue<uint32_t> m_pFreeMaterialIndexes;
		uint32_t m_pMaterialCurrentIndex = 0;

		std::queue<uint32_t> m_pFreeObjectIndexes;
		uint32_t m_pObjectCurrentIndex = 0;

		std::shared_ptr<Context> m_pContext;

		friend class Scene;

		uint32_t mesh_counter = 0;

	public:

		static const size_t MAX_MATERIALS = 10000;
		static const size_t MAX_OBJECTS = 100000;
		static const size_t MAX_TRANSFORMS = 100000;
		static const size_t MAX_DECALS = 1000;
		AssetManager()
		{

		}

		const std::unordered_map<size_t, Ref<Texture>>& getImages() {
			return m_pImages;
		}

		const std::string& getImageName(size_t hash) {
			return m_pImageName.at(hash);
		}

		const Ref<Texture> getImage(const std::string& name) {
			if(m_pImages.contains(std::hash<std::string>{}(name)))
			return m_pImages[std::hash<std::string>{}(name)];
		}

		const Ref<Texture> getImage(size_t hash) {
			if (m_pImages.contains(hash))
				return m_pImages[hash];
			return Ref<Texture>::makeEmpty();
		}

		const std::unordered_map<std::string, RenderModel>& getModels() {
			return m_pModels;
		}

		const std::unordered_map<std::string, AudioBuffer>& getAudios() {
			return m_pAudioBuffers;
		}


		std::vector<glm::mat4*> transformBuffer;
		std::vector < ShaderObjectData*> objectBuffer;
		CursorInfo* infoBufferPtr;
		std::vector<GPUAnimationData*> animationTransformBufferPtr;

		void Init() {

			Filesystem::Mount("./assets/buttons.zip");

			auto resourceManager = m_pContext->ResourceManager();

			m_pGPUBuffers["material_buffer"] = resourceManager->createGPUBuffer("material_buffer", sizeof(PBRMaterial) * MAX_MATERIALS, engine::BufferStorageType::STORAGE_BUFFER);

			m_pGPUBuffers["decal_buffer"] = resourceManager->createGPUBuffer("decal_buffer", sizeof(Decal) * MAX_DECALS, engine::BufferStorageType::STORAGE_BUFFER);

			m_pGPUBuffers["info_buffer"] = resourceManager->createGPUBuffer("info_buffer", 64, engine::BufferStorageType::STORAGE_BUFFER, 1);

			m_pGPUBuffers["object_buffer"] = resourceManager->createGPUBuffer("object_buffer", sizeof(ShaderObjectData) * MAX_OBJECTS, engine::BufferStorageType::STORAGE_BUFFER);

			m_pGPUBuffers["transform_buffer"] = resourceManager->createGPUBuffer("transform_buffer", sizeof(glm::mat4) * MAX_TRANSFORMS, engine::BufferStorageType::STORAGE_BUFFER);

			m_pGPUBuffers["animation_transform_buffer"] = resourceManager->createGPUBuffer("animation_transform_buffer", sizeof(GPUAnimationData)*100, engine::BufferStorageType::STORAGE_BUFFER);
			
			transformBuffer.resize(vk::MAX_FRAMES_IN_FLIGHT);
			objectBuffer.resize(vk::MAX_FRAMES_IN_FLIGHT);
			//infoBufferPtr.resize(vk::MAX_FRAMES_IN_FLIGHT);
			animationTransformBufferPtr.resize(vk::MAX_FRAMES_IN_FLIGHT);

			for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
				transformBuffer[i] = reinterpret_cast<glm::mat4*>(resourceManager->mapBuffer(m_pGPUBuffers["transform_buffer"], i));
				objectBuffer[i] = reinterpret_cast<ShaderObjectData*>(resourceManager->mapBuffer(m_pGPUBuffers["object_buffer"], i));
				animationTransformBufferPtr[i] = reinterpret_cast<GPUAnimationData*>(resourceManager->mapBuffer(m_pGPUBuffers["animation_transform_buffer"], i));
			}
			infoBufferPtr = reinterpret_cast<CursorInfo*>(resourceManager->mapBuffer(m_pGPUBuffers["info_buffer"], 0));

		}

		Ref<Buffer> CreateGPUBuffer(const std::string& name, size_t size, engine::BufferStorageType type = engine::BufferStorageType::STORAGE_BUFFER, int count = -1) {
			if (m_pGPUBuffers.contains(name)) {
				IO::Error("Trying to create a buffer that already exists");
				return Ref<Buffer>::makeEmpty();
			}

			if(name.empty()) {
				IO::Error("GPU buffer name can't be null");
				return Ref<Buffer>::makeEmpty();
			}

			if (size <= 0) {
				IO::Error("GPU buffer size can't be zero");
				return Ref<Buffer>::makeEmpty();
			}
			auto resourceManager = m_pContext->ResourceManager();
			m_pGPUBuffers[name] = resourceManager->createGPUBuffer("material_buffer", size, type, count);

			return m_pGPUBuffers[name];
		}

		void RegisterShader(ShaderAsset shader) {
			m_pShaders[shader.name] = shader;
		}

		ShaderAsset& getShader(const std::string& name) {
			return m_pShaders[name];
		}

		glm::mat4* getTransformBuffer() {
			uint32_t currFrame = m_pContext->Renderer()->CurrentFrameInFlight();
			return transformBuffer[currFrame];
		}

		ShaderObjectData* getObjectBuffer() {
			uint32_t currFrame = m_pContext->Renderer()->CurrentFrameInFlight();
			return objectBuffer[currFrame] ;
		}

		GPUAnimationData* getAnimationBuffer() {
			uint32_t currFrame = m_pContext->Renderer()->CurrentFrameInFlight();
			return animationTransformBufferPtr[currFrame];
		}


		CursorInfo* getBufferPointer() {

			uint32_t currFrame = m_pContext->Renderer()->CurrentFrameInFlight();
			return infoBufferPtr;// [currFrame] ;
		}

		CursorInfo* getBufferPointer(int frame) {

			return infoBufferPtr;// [frame] ;
		}

		void Destroy() {

			auto resourceManager = m_pContext->ResourceManager();
		}

		RenderModel createModelFromMesh(const std::string& name, const common::Mesh& mesh, std::initializer_list<common::MeshMaterial> materials) {

			auto resourceManager = m_pContext->ResourceManager();

			if (m_pModels.contains(name)) {
				for (auto& mesh : m_pModels.at(name).renderMeshes) {
					mesh.id = mesh_counter;
					mesh_counter++;
				}
				return m_pModels.at(name);
			}

			RenderModel& model = m_pModels[name];
			model.name = name;
			RenderMesh subRenderC;
			subRenderC.numMaterials = materials.size();
			subRenderC.mesh = addMesh(name, mesh);
			subRenderC.id = mesh_counter;
			mesh_counter++;
			unsigned int material_index = 0;
			for (auto& material : materials) {
				std::string material_name = name + "_material" + std::to_string(material_index);
				size_t material_name_hash = std::hash<std::string>{}(material_name);


				subRenderC.material_keys[material_index] = material_name_hash;

				PBRMaterial pbr_material;


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


				for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
					PBRMaterial* materialBufferPtr = reinterpret_cast<PBRMaterial*>(resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
					materialBufferPtr[mat_index] = pbr_material;
				}

				m_pMaterials[material_name_hash].index = mat_index;
				m_pMaterials[material_name_hash].material = pbr_material;

				if (!material.albedo_path.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Albedo] = std::hash<std::string>{}(material.albedo_path);
				}
				if (!material.metallic_path.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Metallic] = std::hash<std::string>{}(material.metallic_path);
				}
				if (!material.roughness_path.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Roughness] = std::hash<std::string>{}(material.roughness_path);
				}
				if (!material.normal_path.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Normal] = std::hash<std::string>{}(material.normal_path);
				}

				material_index++;
			}

			if (subRenderC.numMaterials == 0) {
				subRenderC.numMaterials = 1;
				std::string material_name = name + "_material" + std::to_string(material_index);
				size_t material_name_hash = std::hash<std::string>{}(material_name);
				subRenderC.material_keys[material_index] = material_name_hash;

				uint32_t mat_index = 0;
				if (!m_pFreeMaterialIndexes.empty()) {
					mat_index = m_pFreeMaterialIndexes.front();
					m_pFreeMaterialIndexes.pop();
				}
				else {
					mat_index = m_pMaterialCurrentIndex;
					m_pMaterialCurrentIndex++;
				}
				PBRMaterial pbr_material;
				m_pMaterials[material_name_hash].index = mat_index;
				m_pMaterials[material_name_hash].material = pbr_material;
			}

			model.renderMeshes.push_back(subRenderC);

			return model;
		}


		RenderModel createModelFromMesh(const std::string& name, const common::Mesh& mesh, const common::MeshMaterial& material) {

			auto resourceManager = m_pContext->ResourceManager();

			if (m_pModels.contains(name)) {
				for (auto& mesh : m_pModels.at(name).renderMeshes) {
					mesh.id = mesh_counter;
					mesh_counter++;
				}
				return m_pModels.at(name);
			}

			RenderModel& model = m_pModels[name];
			model.name = name;

			std::string material_name = name + "_material";
			size_t material_name_hash = std::hash<std::string>{}(material_name);

			RenderMesh subRenderC;
			subRenderC.material_keys[0] = material_name_hash;
			subRenderC.numMaterials = 1;
			subRenderC.mesh = addMesh(name, mesh);
			subRenderC.id = mesh_counter;
			mesh_counter++;
			model.renderMeshes.push_back(subRenderC);

			if (m_pMaterials.contains(material_name_hash)) {
				return model;
			}
			PBRMaterial pbr_material;



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


			for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
				PBRMaterial* materialBufferPtr = reinterpret_cast<PBRMaterial*>(resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
				materialBufferPtr[mat_index] = pbr_material;
			}

			m_pMaterials[material_name_hash].index = mat_index;
			m_pMaterials[material_name_hash].material = pbr_material;

			if (!material.albedo_path.empty()) {
				m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Albedo] = std::hash<std::string>{}(material.albedo_path);
			}
			if (!material.metallic_path.empty()) {
				m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Metallic] = std::hash<std::string>{}(material.metallic_path);
			}
			if (!material.roughness_path.empty()) {
				m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Roughness] = std::hash<std::string>{}(material.roughness_path);
			}
			if (!material.normal_path.empty()) {
				m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Normal] = std::hash<std::string>{}(material.normal_path);
			}

			return model;
		}

		RenderModel& getModel(const std::string& name) {
			return m_pModels.at(name);
		}

		std::stack<int> m_pFreeAnimationIndices;
		uint32_t m_pCurrentAnimationIndex = 0;

		uint32_t getAnimationIndex() {
			uint32_t anim_index = 0;
			if (m_pFreeAnimationIndices.size() > 0) {
				anim_index = m_pFreeAnimationIndices.top();
				m_pFreeAnimationIndices.pop();
			}
			else {
				anim_index = m_pCurrentAnimationIndex;
				m_pCurrentAnimationIndex++;
			}

			return anim_index;
		}

		void FreeAnimationIndex(uint32_t index) {
			m_pFreeAnimationIndices.push(index);
		}
		RenderModel loadModel(const std::string& path, const std::string& name = "")
		{
			std::string prefix = "./assets/";
			std::string model_name = name;
			if (name == "")
				model_name = path;

			auto resourceManager = m_pContext->ResourceManager();
			if (m_pModels.contains(model_name)) {
				for (auto& mesh : m_pModels.at(model_name).renderMeshes) {
					mesh.id = mesh_counter;
					mesh_counter++;
				}


				if (m_pModels.at(model_name).hasAnimation) {
					m_pModels.at(model_name).animationIndex = getAnimationIndex();
				}

				return m_pModels.at(model_name);
			}

			if (model_name.find(":") != std::string::npos) {
				prefix = "";
			}

			//if()
			const std::string ext = model_name.substr(model_name.find_last_of('.') + 1, model_name.length());

			std::unique_ptr<framework::ModelBase> inModel;

			if (ext == "eml") {
				inModel = std::make_unique<framework::Model>(prefix + model_name);
			}
			else {
				inModel = std::make_unique<framework::gltfModel>(prefix + model_name);
			}


			int index = 0;
			RenderModel& model = m_pModels[model_name];



			model.hasAnimation = inModel->HasAnimation();


			if (model.hasAnimation) {
				model.animationIndex = getAnimationIndex();
			}


			model.name = model_name;
			model.animations = inModel->Animations();
			model.skeleton = inModel->Skeleton();
			model.min_max_points = inModel->getMinMax();
			for (auto& mesh : inModel->Meshes())
			{
				if (mesh.data().mesh.Indices.size() <= 0) continue;

				RenderMesh subRenderC;
				if (inModel->HasAnimation()) {
					subRenderC.mesh = addMesh(model_name + "_submesh_" + std::to_string(index), inModel->AnimatedMeshes().at(index));
				}
				else {
					subRenderC.mesh = addMesh(model_name + "_submesh_" + std::to_string(index), mesh.data().mesh);
				}

				std::string material_name = model_name + "_submesh_" + std::to_string(index) + "_material";
				subRenderC.numMaterials = 1;
				subRenderC.material_keys[0] = std::hash<std::string>{}(material_name);
				subRenderC.id = mesh_counter;
				mesh_counter++;

				size_t material_name_hash = std::hash<std::string>{}(material_name);

				if (m_pMaterials.contains(material_name_hash)) {
					model.renderMeshes.push_back(subRenderC);
					continue;
				}
				auto mesh_material = mesh.Material();

				PBRMaterial material = loadModelMaterials(mesh_material);
				m_pMaterials[material_name_hash].material = material;

				if (!mesh_material.albedo.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Albedo] = std::hash<std::string>{}(mesh_material.albedo);
				}
				if (!mesh_material.metallic.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Metallic] = std::hash<std::string>{}(mesh_material.metallic);
				}
				if (!mesh_material.roughness.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Roughness] = std::hash<std::string>{}(mesh_material.roughness);
				}
				if (!mesh_material.normal.empty()) {
					m_pMaterials[material_name_hash].Slot[(int)TextureSlot::Normal] = std::hash<std::string>{}(mesh_material.normal);
				}

				subRenderC.material_keys[0] = material_name_hash;
				subRenderC.numMaterials = 1;

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

				for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
					PBRMaterial* materialBufferPtr = reinterpret_cast<PBRMaterial*>(resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
					materialBufferPtr[mat_index] = material;
				}

				model.renderMeshes.push_back(subRenderC);

				index++;
			}
			model.loaded = true;

			//
			return model;
		}

		PBRMaterial loadModelMaterials(framework::Mesh::MeshMaterial& mesh_material) {
			PBRMaterial material;

			material.albedo_color = mesh_material.color;
			material.roughness_color = mesh_material.roughness_color;
			material.metallic_color = mesh_material.metallic_color;
			material.transmission = mesh_material.transmission;

			if (!mesh_material.albedo.empty())
			{
				auto albedo = addTexture(mesh_material.albedo, { .format = COLOR_RGBA,
																.wrapMode = REPEAT,
																.filtering = LINEAR });

				if (albedo.empty()) material.albedo_texture_index = -1;
				else material.albedo_texture_index = albedo.Index();
			}
			else {
				material.albedo_texture_index = -1;
			}
			if (!mesh_material.metallic.empty())
			{
				auto metallic = addTexture(mesh_material.metallic, { .format = NON_COLOR_RGBA,
																	.wrapMode = REPEAT,
																	.filtering = LINEAR });

				if (metallic.empty()) material.metallic_texture_index = -1;
				else material.metallic_texture_index = metallic.Index();
			}
			else {
				material.metallic_texture_index = -1;
			}
			if (!mesh_material.roughness.empty())
			{
				auto roughness = addTexture(mesh_material.roughness, { .format = NON_COLOR_RGBA,
																	  .wrapMode = REPEAT,
																	  .filtering = LINEAR });

				if (roughness.empty()) material.roughness_texture_index = -1;
				else material.roughness_texture_index = roughness.Index();
			}
			else {
				material.roughness_texture_index = -1;
			}
			if (!mesh_material.normal.empty())
			{
				auto normal = addTexture(mesh_material.normal, { .format = NON_COLOR_RGBA,
																.wrapMode = REPEAT,
																.filtering = LINEAR });

				if (normal.empty()) material.normal_texture_index = -1;
				else material.normal_texture_index = normal.Index();
			}
			else {
				material.normal_texture_index = -1;
			}

			return material;
		}

		AudioObject loadAudio(const std::string& path)
		{

			auto audioManager = m_pContext->AudioManager();

			if (m_pAudioBuffers.contains(path))
			{
				AudioBuffer& buffer = m_pAudioBuffers.at(path);
				buffer.sources.push_back(audioManager->createSource(path + "_source_" + std::to_string(buffer.sources.size()), { .buffer = buffer.buffer }));

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

			AudioBuffer& audioBuffer = m_pAudioBuffers[path];
			audioBuffer.buffer = buffer;

			audioBuffer.sources.push_back(audioManager->createSource(path + "_source_" + std::to_string(audioBuffer.sources.size()), { .buffer = audioBuffer.buffer }));

			AudioObject object;
			object.buffer = audioBuffer;
			object.source = audioBuffer.sources.back();

			return object;
		}

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

		Ref<Texture> addTextureFromBytes(const std::string& name, unsigned char* bytes, size_t size, int w, int h, int c, const TextureInfo& info) {

			std::vector<size_t> offsets;
			unsigned int format = info.format;
			bool isKTX = false;
			bool isDDS = false;
			int mipLevels = 0;
			Ref<Texture> ref = Ref<Texture>::makeEmpty();
			if (bytes != nullptr)
			{
				engine::TextureCreationInfo texInfo = {
					.width = (uint32_t)w,
					.height = (uint32_t)h,
					.numChannels = (uint32_t)c,
					.format = (engine::TextureFormat)format,
					.wrapMode = info.wrapMode,
					.filtering = info.filtering,
					.name = name,
					.isCompressed = false,
					.isKTX = isKTX,
					.mipLevels = (uint32_t)mipLevels,
					.offsets = offsets,
					.size = size,
					.pixels = bytes,
				};

				auto resourceManager = m_pContext->ResourceManager();
				ref = resourceManager->createTexture(texInfo);

				m_pContext->Renderer()->getDebugRenderer()->addTexture(ref);
			}

			m_pImages[std::hash<std::string>{}(name)] = ref;
			m_pImageName[std::hash<std::string>{}(name)] = name;

			return ref;
		}

		Ref<Texture> addTexture(const std::string& path, const TextureInfo& info)
		{

			auto resourceManager = m_pContext->ResourceManager();

			if (m_pImages.contains(std::hash<std::string>{}(path)))
			{
				return m_pImages.at(std::hash<std::string>{}(path));
			}

			int width, height, num_channels, mipLevels = 1;
			size_t size = 0;
			bool isCompressed = false;
			std::string texture_path = "./assets/" + path;

			const std::string ext = path.substr(path.find_last_of('.') + 1, path.length());
			unsigned char* pixels;
			ktxTexture* ktxTex;
			ktxTexture2* ktxTex2;
			std::vector<size_t> offsets; 
			unsigned int format = info.format;
			bool isKTX = false;
			bool isDDS = false;
			if (ext == "ktx") {
				ktxResult result = KTX_SUCCESS;

				result = ktxTexture_CreateFromNamedFile(texture_path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex);

				if (result != KTX_SUCCESS) {
					throw std::runtime_error("Error loading texture: " + texture_path);
				}
				if (ktxTex->isCompressed)
				{
					isCompressed = true;
				}
				pixels = ktxTexture_GetData(ktxTex);
				pixels = ktxTexture_GetData(ktxTex);
				ktx_size_t ktxTextureSize = ktxTexture_GetDataSize(ktxTex);

				width = ktxTex->baseWidth;
				height = ktxTex->baseHeight;
				mipLevels = ktxTex->numLevels;
				isKTX = true;
				size = ktxTextureSize;
				num_channels = 4;

				for (int i = 0; i < mipLevels; i++) {
					ktx_size_t offset;
					KTX_error_code result = ktxTexture_GetImageOffset(ktxTex, i, 0, 0, &offset);
					offsets.push_back(offset);
				}
			}
			else if (ext == "ktx2") {
				ktxResult result = KTX_SUCCESS;

				result = ktxTexture2_CreateFromNamedFile(texture_path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTex2);

				if (result != KTX_SUCCESS) {
					throw std::runtime_error("Error loading texture: " + texture_path);
				}
				if (ktxTex2->isCompressed)
				{
					isCompressed = true;
				}
				pixels = ktxTex2->pData;
				ktx_size_t ktxTextureSize = ktxTex2->dataSize;

				width = ktxTex2->baseWidth;
				height = ktxTex2->baseHeight;
				mipLevels = ktxTex2->numLevels;
				isKTX = true;
				size = ktxTextureSize;
				num_channels = 4;

				for (int i = 0; i < mipLevels; i++) {
					ktx_size_t offset;

					KTX_error_code result = ktxTexture_GetImageOffset(ktxTexture(ktxTex2), i, 0, 0, &offset);
					offsets.push_back(offset);
				}
			}
			else if(ext == "dds") {
				int baseLevel = 0;
				DDS ddsfile(texture_path, baseLevel);
				isDDS = true;
				width = ddsfile.width();
				height = ddsfile.height();
				mipLevels = ddsfile.mipLevels();
				size = ddsfile.size();
				num_channels = 4;
				isCompressed = true;
				unsigned int desired_format = format;
				if(desired_format == NON_COLOR_RGBA) {
					format = TextureFormat::NON_COLOR_RGBA_BC3;
				}
				else {
					format = ddsfile.format();
				}

				if (desired_format == NON_COLOR_RGBA && ddsfile.format() == COLOR_RGBA_BC7) {
					format = NON_COLOR_RGBA_BC7;
				}
				else if (desired_format == COLOR_RGBA && ddsfile.format() == NON_COLOR_RGBA_BC7) {
					format = COLOR_RGBA_BC7;
				}
				else if (desired_format == NON_COLOR_RGBA && ddsfile.format() == NON_COLOR_RGBA_BC7) {
					format = NON_COLOR_RGBA_BC7;
				}
				pixels = ddsfile.data();
				size_t offset = 0;

				int w = width, h = height;
				unsigned int s = 0;
				for (unsigned int level = 0; level < mipLevels; ++level)
				{

					s = ((w + 3) / 4) * (((h + 3) / 4) * ddsfile.blockSize());
					offsets.push_back(offset);
					offset += s;
					w /= 2;
					h /= 2;

					if (w < 1) w = 1;
					if (h < 1) h = 1;

				}


			} else {
				pixels = framework::load_image_from_file(texture_path.c_str(), &width, &height, &num_channels);
			}
			Ref<Texture> ref = Ref<Texture>::makeEmpty();
			if (pixels != nullptr)
			{
				engine::TextureCreationInfo texInfo = {
					.width = (uint32_t)width,
					.height = (uint32_t)height,
					.numChannels = (uint32_t)num_channels,
					.format = (engine::TextureFormat)format,
					.wrapMode = info.wrapMode,
					.filtering = info.filtering,
					.name = path,
					.isCompressed = isCompressed,
					.isKTX = isKTX,
					.mipLevels = (uint32_t)mipLevels,
					.offsets = offsets,
					.size = size,
					.pixels = pixels,
				};

				ref = resourceManager->createTexture(texInfo);

				m_pContext->Renderer()->getDebugRenderer()->addTexture(ref);

				if (!isCompressed && !isDDS) {
					framework::free_image_data(pixels);
				}/*
				else {
					ktxTexture_Destroy(ktxTex);
					ktxTexture_Destroy(ktxTexture(ktxTex2));
				}*/

				if (isDDS) {
					DDS::free_data(pixels);
				}

				m_pImages[std::hash<std::string>{}(path)] = ref;
				m_pImageName[std::hash<std::string>{}(path)] = path;
			}

			return ref;
		}

		Ref<Mesh> addMesh(const std::string& name, common::Mesh mesh)
		{

			auto resourceManager = m_pContext->ResourceManager();

			auto ref = resourceManager->createMesh({ .vertices = mesh.Vertices,
													.indices = mesh.Indices,
													.name = name });

			return ref;
		}

		Ref<Mesh> addMesh(const std::string& name, common::AnimatedMesh mesh)
		{

			auto resourceManager = m_pContext->ResourceManager();

			auto ref = resourceManager->createMesh({ .vertices = mesh.Vertices,
													.indices = mesh.Indices,
													.name = name });

			return ref;
		}

		PBRMaterialIndex& getMaterial(size_t key) {
			return m_pMaterials.at(key);
		}

		void setMaterial(size_t key, PBRMaterialIndex material) {
			m_pMaterials.at(key) = material;
		}


		std::unordered_map<std::string, ShaderAsset>  getShaders() {
			return m_pShaders;
		}


		size_t createMaterial(const std::string& name, PBRMaterial material) {

			auto resourceManager = m_pContext->ResourceManager();
			uint32_t mat_index = 0;
			if (!m_pFreeMaterialIndexes.empty()) {
				mat_index = m_pFreeMaterialIndexes.front();
				m_pFreeMaterialIndexes.pop();
			}
			else {
				mat_index = m_pMaterialCurrentIndex;
				m_pMaterialCurrentIndex++;
			}

			size_t material_name_hash = std::hash<std::string>{}(name);
			m_pMaterials[material_name_hash].index = mat_index;


			for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
				PBRMaterial* materialBufferPtr = reinterpret_cast<PBRMaterial*>(resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
				materialBufferPtr[mat_index] = material;
				resourceManager->unmapBuffer(m_pGPUBuffers["material_buffer"], i);
			}

			return material_name_hash;
		}

		void setMaterial(PBRMaterialIndex material) {

			auto resourceManager = m_pContext->ResourceManager();

			for (int i = 0; i < vk::MAX_FRAMES_IN_FLIGHT; i++) {
				PBRMaterial* materialBufferPtr = reinterpret_cast<PBRMaterial*>(resourceManager->mapBuffer(m_pGPUBuffers["material_buffer"], i));
				materialBufferPtr[material.index] = material.material;
				resourceManager->unmapBuffer(m_pGPUBuffers["material_buffer"], i);
			}
		}

	private:
	};
}