//
// Created by solaire on 1/2/23.
//

#ifndef EPSILON_BSP_HPP
#define EPSILON_BSP_HPP

#include <filesystem>

#include <Epsilon.hpp>
#include <beacon/beacon.hpp>

#include "SourceBSP.hpp"
#include <core/framework/utils/helpers/camera.hpp>
#include <core/framework/IO/KeyBoard.hpp>

namespace BSP
{
	class BSP : public Epsilon::Epsilon
	{

		struct PushConstant
		{
			alignas(16) glm::mat4 model;
		};

		struct BSPFace
		{
			engine::Ref<engine::Mesh> mesh;
			engine::Ref<engine::Texture> texture;
			engine::Ref<engine::BindGroup> material;
			uint32_t layoutIndex = 0;
		};

		struct BSPMap
		{
			std::list<BSPFace> m_pFaces{};
			engine::Ref<engine::PushConstant> pushConstantRef;
			PushConstant pushConstant;
		};

		struct Material
		{
			int32_t diffuse = -1;
			int32_t normal = -1;
		};

		std::vector<int> m_pVisibleFaces;

		engine::Ref<engine::RenderPass> m_pRenderPass;
		engine::Ref<engine::RenderPass> m_pShadowRenderPass;

		BSPMap m_pMap;

		std::shared_ptr<utils::Camera> m_pCamera;

		engine::Ref<engine::BindGroup> dummyMaterial;
		engine::Ref<engine::BindGroup> skyMaterial;
		engine::Ref<engine::BindGroup> shadowDummyMaterial;
		engine::Ref<engine::Buffer> gpuBuffer;

		bool m_pUpdateCamera = true;
		double m_pTime = 0.0;
		ShaderData m_pCameraData;

	public:
		explicit BSP(const std::string& appname) : Epsilon::Epsilon(appname)
		{
			m_pCamera = std::make_shared<utils::Camera>(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

			Epsilon::getSingleton().onCreate = [this]
			{ onCreate(); };
			Epsilon::getSingleton().onReady = [this]
			{ onReady(); };
			Epsilon::getSingleton().onRender = [this]
			{ onRender(); };
			Epsilon::getSingleton().onExit = [this]
			{ onExit(); };
		}

		void onCreate()
		{

			setupRenderPass();

			using en = engine::UniformBindingType;

			auto resourceManager = Epsilon::getContext().ResourceManager();

			engine::BindGroupInfo skyInfo = {
				.bindingInfo = {
					{.size = sizeof(ShaderData), .binding = 0, .type = en::UNIFORM_BUFFER}},
				.renderPass = "Default",
				.name = "SkyMaterial" };

			skyMaterial = resourceManager->createBindGroup(skyInfo);

			engine::BindGroupInfo shadowInfo = {
				.bindingInfo = {{.size = sizeof(ShaderData),
								 .binding = 0,
								 .type = en::UNIFORM_BUFFER}},
				.renderPass = "Shadow",
				.name = "ShadowMaterial",
			};

			shadowDummyMaterial = resourceManager->createBindGroup(shadowInfo);

			m_pMap.pushConstant.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.0125f));

			m_pMap.pushConstantRef = resourceManager->createPushConstant(
				"ModelMatrix",
				{ .size = sizeof(PushConstant), .data = &m_pMap.pushConstant });

			const char* filename = "./assets/models/hl2/background01.bsp";

			SourceBSP bspMap(filename);

			gpuBuffer = resourceManager->createGPUBuffer("material_buffer", sizeof(Material) * bspMap.numMaterials(), engine::BufferStorageType::STORAGE_BUFFER);

			Material* materialBufferPtr = reinterpret_cast<Material*>(resourceManager->mapBuffer(gpuBuffer));

			engine::BindGroupInfo bindGroupInfo = {
				.bindingInfo = {
					{.size = sizeof(ShaderData), .offset = 0, .binding = 0, .type = en::UNIFORM_BUFFER},
					{.size = sizeof(Material) * bspMap.numMaterials(), .offset = 0, .binding = 3, .type = en::SHADER_STORAGE, .buffer = "material_buffer"}},
				.inputs = {
					{.renderPass = "Shadow", .index = 1, .bindingPoint = 1},
					{.renderPass = "Shadow", .index = 0, .bindingPoint = 2},
				},
				.renderPass = "Default",
				.name = "DefaultMaterial",
			};

			dummyMaterial = resourceManager->createBindGroup(bindGroupInfo);

			int32_t index = 0;
			int32_t id = 0;
			for (auto& [materialName, FaceList] : bspMap.Faces())
			{
				BSPFace bspFace;
				std::vector<common::Vertex> vertices;
				std::vector<unsigned int> indices;

				int highestIndex = 0;
				for (auto& face : FaceList)
				{
					if (face.vertices.size() <= 0)
						continue;

					std::vector<common::Vertex> tmpVertices{};
					for (auto& vtx : face.vertices)
					{
						auto& v = tmpVertices.emplace_back();
						v.position = vtx;
						bspFace.layoutIndex = 0;
						bspFace.material = dummyMaterial;
						if (face.tool == true)
						{
							v.color = glm::vec4(0.2, 0.5, 1.0, 0.5);
							bspFace.material = skyMaterial;
							bspFace.layoutIndex = 1;
						}
						else if (face.trigger == true)
						{
							v.color = glm::vec4(0.5, 0.5, 0.0, 0.5);
						}
						else
						{
							v.color = glm::vec4(0.0, 0.0, 0.0, 1.0);
						}
					}

					for (int j = 0; j < face.uvs.size(); j++)
					{
						tmpVertices[j].texCoords = face.uvs[j];
					}

					if (face.isDisplacement)
					{

						calculateNormalsDisplacement(tmpVertices, face.indices);
						generateTangentSpaceVectorsDisplacement(tmpVertices, face.indices);
					}
					else
					{
						calculateNormals(tmpVertices, face.indices);
						generateTangentSpaceVectors(tmpVertices, face.indices);
					}

					for (auto& v : tmpVertices)
						vertices.push_back(v);

					for (auto& i : face.indices)
						indices.push_back(highestIndex + i);

					highestIndex = vertices.size();
				}
				bspFace.mesh = resourceManager->createMesh({
					.vertices = vertices,
					.indices = indices,
					.name = "BSPFace_" + materialName,
					});

				{
					int width, height, num_channels;
					std::string path = "./assets/textures/" + materialName + ".tga";

					std::transform(path.begin(), path.end(), path.begin(),
						[](unsigned char c)
						{ return std::tolower(c); });

					unsigned char* pixels = framework::load_image_from_file(path.c_str(), &width, &height, &num_channels);

					if (width == 0 || height == 0 || pixels == nullptr)
					{
						materialBufferPtr[index].diffuse = 0;
						materialBufferPtr[index].normal = -1;
					}
					else
					{

						std::cout << "Loaded: " << materialName << std::endl;
						engine::TextureCreationInfo texInfo = {
							.width = (uint32_t)width,
							.height = (uint32_t)height,
							.numChannels = (uint32_t)num_channels,
							.format = engine::COLOR_RGBA, //(index == 0 ? engine::COLOR_RGBA : engine::NON_COLOR_RGBA),
							.wrapMode = engine::REPEAT,
							.filtering = engine::LINEAR,
							.name = materialName,
							.pixels = pixels,
						};

						resourceManager->createTexture(texInfo);
						materialBufferPtr[index].diffuse = id;
						materialBufferPtr[index].normal = -1;
						framework::free_image_data(pixels);
						id++;

						{

							std::string normal_path = "./assets/textures/" + materialName + "_normal.tga";
							unsigned char* pixels = framework::load_image_from_file(normal_path.c_str(), &width, &height, &num_channels);
							if (pixels != nullptr)
							{
								engine::TextureCreationInfo texInfo = {
									.width = (uint32_t)width,
									.height = (uint32_t)height,
									.numChannels = (uint32_t)num_channels,
									.format = engine::NON_COLOR_RGBA, //(index == 0 ? engine::COLOR_RGBA : engine::NON_COLOR_RGBA),
									.wrapMode = engine::REPEAT,
									.filtering = engine::LINEAR,
									.name = materialName,
									.pixels = pixels,
								};

								resourceManager->createTexture(texInfo);
								materialBufferPtr[index].normal = id;
								framework::free_image_data(pixels);
								id++;
							}
						}
					}
					index++;
				}

				m_pMap.m_pFaces.push_back(bspFace);
			}
			resourceManager->unmapBuffer(gpuBuffer);
		}

		double timeSinceCursorChange = 0.0;

		void onReady()
		{

			m_pCameraData.lightPosition = glm::vec3(1.0, 50.0, 0.0);
			m_pCameraData.iTime = 0;
			m_pCameraData.iFrame = Frame();

			Epsilon::getContext().Window().HideCursor();
			framework::Input::KeyBoard::KeyboardEventHandler.addListener(
				([this](auto* sender, beacon::args* args)
					{
						if (args == nullptr)
							return;

						auto obj = args->to<framework::Input::KeyboardArgs>();

						// std::cout << "pressed" << std::endl;
#if USE_GLFW
						if (obj.key_up_index == framework::Input::GLFW::Key::M && timeSinceCursorChange > 1000)
						{

							m_pUpdateCamera = !m_pUpdateCamera;
							if (m_pUpdateCamera)
							{

								Epsilon::getContext().Window().HideCursor();
							}
							else
							{

								Epsilon::getContext().Window().ShowCursor();
							}
							timeSinceCursorChange = 0;
						}
						if (obj.key_up_index == framework::Input::GLFW::Key::P)
						{
							auto pos = m_pCamera->getPosition();
							auto dir = m_pCamera->getDirection();
							std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
							std::cout << dir.x << " " << dir.y << " " << dir.z << std::endl;
						}
#endif
					}));
		}

		void onRender()
		{

			timeSinceCursorChange += framework::Clock::Delta();
			engine::ObjectData objectData;

			setupShadowCamera();

			for (auto& face : m_pMap.m_pFaces)
			{
				objectData.layout_index = 0;
				objectData.mesh = face.mesh;
				objectData.material = shadowDummyMaterial;
				objectData.modelMatrix = glm::mat4(1.0f);
				objectData.pushConstant = m_pMap.pushConstantRef;
				objectData.uniformIndex = 0; // ;
				Epsilon::getContext().Renderer()->Push(objectData);
			}

			Epsilon::getContext().Renderer()->BeginFrame();
			Epsilon::getContext().Renderer()->Begin();
			Epsilon::getContext().Renderer()->Flush(m_pShadowRenderPass, engine::DrawType::INDEXED);

			setupCamera();

			int index = 0;
			for (auto& face : m_pMap.m_pFaces)
			{

				objectData.layout_index = face.layoutIndex;
				objectData.mesh = face.mesh;
				objectData.material = face.material;
				objectData.modelMatrix = glm::mat4(1.0f);
				objectData.pushConstant = m_pMap.pushConstantRef;
				objectData.uniformIndex = index;
				Epsilon::getContext().Renderer()->Push(objectData);
				index++;
			}

			// drawFrame(m_pRenderPass);
			engine::Context::getSingleton().Renderer()->Flush(m_pRenderPass, engine::DrawType::INDEXED_INDIRECT);
			engine::Context::getSingleton().Renderer()->End(m_pCameraData.lightPosition);

			engine::Context::getSingleton().Renderer()->Submit();
			engine::Context::getSingleton().Renderer()->EndFrame();
		}

		void onExit()
		{
		}

	private:
		auto setupCamera() -> void
		{
			const auto& [screenX, screenY] = getWindowDimensions();
			m_pCameraData.iResolution = glm::vec2(screenX, screenY);

			static auto startTime = std::chrono::high_resolution_clock::now();
			auto currentTime = std::chrono::high_resolution_clock::now();
			m_pTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

			if (m_pUpdateCamera)
			{
				m_pCamera->Update(Epsilon::getContext().Window().getWindow());
				m_pCamera->UpdateMatrices(0, screenX, screenY, false);
			}

			m_pCameraData.view = m_pCamera->getViewMatrix();
			m_pCameraData.proj = m_pCamera->getProjectionMatrix();
			m_pCameraData.lightMatrix = lightMatrix;
			m_pCameraData.proj[1][1] *= -1;
			m_pCameraData.iTime += m_pTime;
			m_pCameraData.viewPosition = m_pCamera->getPosition();

			Epsilon::getContext().Renderer()->UpdateRenderPassUniforms(m_pRenderPass, engine::RENDERPASS_SET, &m_pCameraData);
			// PushShaderData(m_pCameraData);
		}

		glm::mat4 lightMatrix = glm::mat4(1.0);

		auto setupShadowCamera() -> void
		{

			const auto& [screenX, screenY] = getWindowDimensions();
			m_pCameraData.iResolution = glm::vec2(1500, 1500);

			// 30.6363, 13.117, -38.7729

			m_pCameraData.view = glm::lookAt(glm::vec3(23.6365, 25.117, -45.7729),
				glm::vec3(23.6365, 0.897448, -22.8326),
				glm::vec3(0.0f, 1.0f, 0.0f));

			m_pCameraData.proj = glm::ortho(-25.0f,
				25.0f,
				-25.0f,
				25.0f, -20.f, 100.0f);

			// m_pCameraData.lightPosition = m_pCamera->getPosition();
			m_pCameraData.proj[1][1] *= -1;
			lightMatrix = m_pCameraData.proj * m_pCameraData.view * glm::mat4(1.0);
			m_pCameraData.lightMatrix = lightMatrix;
			m_pCameraData.viewPosition = m_pCamera->getPosition();
			m_pCameraData.iTime += m_pTime;
			//Epsilon::getContext().Renderer()->UpdateRenderPassUniforms(m_pShadowRenderPass, engine::RENDERPASS_SET, &m_pCameraData);
			// PushShaderData(m_pCameraData);

			//new calcs
			float cascadeSplits = 0.0f;

			float nearClip = m_pCamera->getNearPlane();
			float farClip = 25.0f;
			float clipRange = farClip - nearClip;

			float minZ = nearClip;
			float maxZ = nearClip + clipRange;


			float cascadeSplitLambda = 0.95f;

			float range = maxZ - minZ;
			float ratio = maxZ / minZ;

			float p = 1.0f;
			float log = minZ * ratio;
			float uniform = minZ + range * p;
			float d = cascadeSplitLambda * (log - uniform) + uniform;
			cascadeSplits = (d - nearClip) / clipRange;


			// Calculate orthographic projection matrix for each cascade
			float lastSplitDist = 0.0;
			float splitDist = cascadeSplits;

			float quantStep = 1.0 / 3000;

			glm::vec3 frustumCorners[8] = {
				glm::vec3(-1.0f,  1.0f, 0.0f),
				glm::vec3(1.0f,  1.0f, 0.0f),
				glm::vec3(1.0f, -1.0f, 0.0f),
				glm::vec3(-1.0f, -1.0f, 0.0f),
				glm::vec3(-1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f, -1.0f,  1.0f),
				glm::vec3(-1.0f, -1.0f,  1.0f),
			};

			glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(m_pCamera->getFoV()), glm::clamp((float)screenY / (float)screenX, -10.0f, 10.0f), 0.1f, 25.0f);

			glm::mat4 ViewMatrix = glm::lookAt(
				m_pCamera->getPosition(),
				m_pCamera->getPosition() + m_pCamera->getDirection(),
				glm::vec3(0.0f, 1.0f, 0.0f));


			// Project frustum corners into world space
			glm::mat4 invCam = glm::inverse(ProjectionMatrix * ViewMatrix);
			for (uint32_t i = 0; i < 8; i++) {
				glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
				frustumCorners[i] = invCorner / invCorner.w;
			}

			for (uint32_t i = 0; i < 4; i++) {
				glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
			}


			// Get frustum center
			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (uint32_t i = 0; i < 8; i++) {
				frustumCenter += frustumCorners[i];
			}
			frustumCenter /= 8.0f;

			float radius = 0.0f;
			for (uint32_t i = 0; i < 8; i++) {
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			radius = glm::floor(radius) + glm::floor(radius) * quantStep + quantStep;

			glm::vec3 maxExtents = glm::vec3(radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 lightDir = glm::normalize(-glm::vec3(23.0, 25.0, -45.0));
			//glm::vec3 lightDir = glm::normalize(glm::vec3(0.0, -1.0, 0.0));


			glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0, 0.0f));
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, -20.0f, 100.0f);


			m_pCameraData.view = lightViewMatrix;

			m_pCameraData.proj = lightOrthoMatrix;

			// m_pCameraData.lightPosition = m_pCamera->getPosition();
			m_pCameraData.proj[1][1] *= -1;
			lightMatrix = m_pCameraData.proj * m_pCameraData.view * glm::mat4(1.0);
			m_pCameraData.lightMatrix = lightMatrix;
			m_pCameraData.viewPosition = m_pCamera->getPosition();
			Epsilon::getContext().Renderer()->UpdateRenderPassUniforms(m_pShadowRenderPass, engine::RENDERPASS_SET, &m_pCameraData);

		}

		auto setupRenderPass() -> void
		{

			using namespace engine;
			// Load the shader that draws the board
			auto vertexCode = utils::readFile("./assets/shaders/vertex.spv");
			auto fragmentCode = utils::readFile("./assets/shaders/fragment.spv");

			auto shadowVertexCode = utils::readFile("./assets/shaders/shadow-vertex.spv");
			auto shadowFragmentCode = utils::readFile("./assets/shaders/shadow-fragment.spv");

			auto skyVertexCode = utils::readFile("./assets/shaders/sky-vertex.spv");
			auto skyFragmentCode = utils::readFile("./assets/shaders/sky-fragment.spv");

			ShaderInfo mainShaderInfo = {
				.stages = {
					{.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
					{.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}},
				.usedStages = ShaderModuleStage(VERTEX | FRAGMENT) };

			ShaderInfo shadowShaderInfo = {
				.stages = {
					{.entryPoint = "main", .shaderCode = shadowVertexCode, .stage = VERTEX},
					{.entryPoint = "main", .shaderCode = shadowFragmentCode, .stage = FRAGMENT}},
				.usedStages = ShaderModuleStage(VERTEX | FRAGMENT) };

			ShaderInfo skyShaderInfo = {
				.stages = {
					{.entryPoint = "main", .shaderCode = skyVertexCode, .stage = VERTEX},
					{.entryPoint = "main", .shaderCode = skyFragmentCode, .stage = FRAGMENT}},
				.usedStages = ShaderModuleStage(VERTEX | FRAGMENT) };

			VertexLayout vertexLayout = {
				.descriptors = {
					{XYZ_FLOAT, offsetof(common::Vertex, position)},
					{XY_FLOAT, offsetof(common::Vertex, texCoords)},
					{XYZ_FLOAT, offsetof(common::Vertex, normal)},
					{XYZW_FLOAT, offsetof(common::Vertex, color)},
					{XYZ_FLOAT, offsetof(common::Vertex, tangent)},
					{XYZ_FLOAT, offsetof(common::Vertex, bitangent)}
				},
				.size = sizeof(common::Vertex)
			};
			PipelineLayout mainLayout = {
				.shaderInfo = mainShaderInfo,
				.vertexLayout = vertexLayout,
				.cullMode = CullMode::BACK,
				.windingMode = WindingMode::CLOCKWISE };

			PipelineLayout shadowLayout = {
				.shaderInfo = shadowShaderInfo,
				.vertexLayout = vertexLayout,
				.cullMode = CullMode::FRONT,
				.windingMode = WindingMode::CLOCKWISE };

			PipelineLayout skyLayout = {
				.shaderInfo = skyShaderInfo,
				.vertexLayout = vertexLayout,
				.cullMode = CullMode::BACK,
				.windingMode = WindingMode::CLOCKWISE };

			// Configure the default render pass object
			RenderPassInfo renderPassInfo =
				RenderPassFactory()
				.name("Default")
				.depthAttachment(true)
				.isSwapChainAttachment(true)
				.subpasses({})
				.dimensions({ .width = 1280, .height = 720 })
				.inputs({ {.size = sizeof(ShaderData), .offset = 0, .binding = 0, .type = UniformBindingType::UNIFORM_BUFFER},
						 {.size = 0, .offset = 0, .binding = 1, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER},
						 {.size = 0, .offset = 0, .binding = 2, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER},
						 {.size = sizeof(Material), .offset = 0, .binding = 3, .type = UniformBindingType::SHADER_STORAGE} })
				.outputs({ {
							  .format = COLOR_RGBA,
							  .blendEnable = true,
							  .clearColor = {0.1f, 0.1f, 0.1f, 1.0001f},
							  .isDepthAttachment = false,
							  .isSwapChainAttachment = true,
							  .clearAttachment = true,
						  },
						  {.format = DEPTH_F32_STENCIL_8,
						   .depthStencilValue = {1, 0},
						   .isDepthAttachment = true} })
						   .pipelineLayout(mainLayout)
				.pipelineLayout(skyLayout)
				.pushConstant(sizeof(PushConstant));

			RenderPassInfo shadowRenderPassInfo =
				RenderPassFactory()
				.name("Shadow")
				.depthAttachment(true)
				.subpasses({})
				.dimensions({ .width = 3000, .height = 3000 })
				.inputs({ {.size = sizeof(ShaderData), .offset = 0, .binding = 0, .type = UniformBindingType::UNIFORM_BUFFER} })
				.outputs(
					{ {.format = COLOR_RG_32F,
					  .wrapMode = CLAMP_TO_BORDER,
					  .filtering = engine::LINEAR,
					  .compareFunc = ALWAYS,
					  .depthCompare = false,
					  .clearColor = {1.0, 1.0, 1.0, 1.0},
					  .isSampler = true,
					  .isDepthAttachment = false,
					  .isSwapChainAttachment = false,
					  .clearAttachment = true},

					 {.format = DEPTH_F32_STENCIL_8,
					  .filtering = engine::LINEAR,
					  .compareFunc = LESS_OR_EQUAL,
					  .depthCompare = true,
					  .depthStencilValue = {1, 0},
					  .isSampler = true,
					  .isDepthAttachment = true} })
				.pipelineLayout(shadowLayout)
				.pushConstant(sizeof(PushConstant));

			m_pRenderPass = Epsilon::getContext().ResourceManager()->createDefaultRenderPass(renderPassInfo);
			m_pShadowRenderPass = Epsilon::getContext().ResourceManager()->createRenderPass(shadowRenderPassInfo);
		}

		auto calculateNormals(std::vector<common::Vertex>& vertices,
			const std::vector<unsigned int>& indices) -> void
		{

			uint32_t size = indices.size();
			auto v1 = vertices[indices[0]].position;
			auto v2 = vertices[indices[1]].position;
			auto v3 = vertices[indices[size - 1]].position;

			glm::vec3 n = glm::cross(v3 - v1, v2 - v1);

			if (glm::isnan(n.x))
			{
				v1 = vertices[indices[size - 1]].position;
				n = glm::cross(v3 - v1, v2 - v1);
			}

			n = glm::normalize(n);

			for (auto& v : vertices)
			{
				v.normal = n;
			}
		}

		auto calculateNormalsDisplacement(std::vector<common::Vertex>& vertices,
			const std::vector<unsigned int>& indices) -> void
		{

			uint32_t size = indices.size();

			for (int i = 0; i < size; i += 3)
			{
				auto v1 = vertices[indices[i]].position;
				auto v2 = vertices[indices[i + 1]].position;
				auto v3 = vertices[indices[i + 2]].position;

				glm::vec3 n = glm::cross(v3 - v1, v2 - v1);

				if (glm::isnan(n.x))
				{
					v1 = vertices[indices[size - 1]].position;
					n = glm::cross(v3 - v1, v2 - v1);
				}

				n = glm::normalize(n);

				vertices[indices[i]].normal = n;
				vertices[indices[i + 1]].normal = n;
				vertices[indices[i + 2]].normal = n;
			}
		}
		auto generateTangentSpaceVectorsDisplacement(std::vector<common::Vertex>& vertices, const std::vector<uint32_t>& indices) -> void
		{
			uint32_t size = indices.size();
			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1, bitangent1;
			// std::cout << m_pMesh.m_Tris.size() <<  std::endl;
			for (int i = 0; i < indices.size(); i += 3)
			{
				glm::vec3 v0 = vertices[indices[i]].position;
				glm::vec3 v1 = vertices[indices[i + 1]].position;
				glm::vec3 v2 = vertices[indices[i + 2]].position;

				glm::vec3 edge1 = v0 - v1;
				glm::vec3 edge2 = v0 - v2;

				glm::vec2 t0 = vertices[indices[i]].texCoords;
				glm::vec2 t1 = vertices[indices[i + 1]].texCoords;
				glm::vec2 t2 = vertices[indices[i + 2]].texCoords;

				glm::vec2 deltaUV1 = t0 - t1;
				glm::vec2 deltaUV2 = t0 - t2;

				tangent1 = glm::cross(vertices[indices[i]].normal, edge1);
				bitangent1 = glm::cross(vertices[indices[i]].normal, tangent1);

				vertices[indices[i]].tangent = glm::normalize(tangent1);
				vertices[indices[i + 1]].tangent = glm::normalize(tangent1);
				vertices[indices[i + 2]].tangent = glm::normalize(tangent1);

				// bitangent1 = glm::normalize(bitangent1);

				vertices[indices[i]].bitangent = glm::normalize(bitangent1);
				vertices[indices[i + 1]].bitangent = glm::normalize(bitangent1);
				vertices[indices[i + 2]].bitangent = glm::normalize(bitangent1);
			}

		}
		auto generateTangentSpaceVectors(std::vector<common::Vertex>& vertices, const std::vector<uint32_t>& indices) -> void
		{
			uint32_t size = indices.size();
			// calculate tangent/bitangent vectors of both triangles
			glm::vec3 tangent1, bitangent1;
			// std::cout << m_pMesh.m_Tris.size() <<  std::endl;
			// for (int i = 0; i < indices.size(); i += 3)
			//{
			glm::vec3 v0 = vertices[indices[0]].position;
			glm::vec3 v1 = vertices[indices[1]].position;
			glm::vec3 v2 = vertices[indices[size - 1]].position;

			glm::vec3 edge1 = v0 - v1;
			glm::vec3 edge2 = v0 - v2;

			glm::vec2 t0 = vertices[indices[0]].texCoords;
			glm::vec2 t1 = vertices[indices[1]].texCoords;
			glm::vec2 t2 = vertices[indices[size - 1]].texCoords;

			glm::vec2 deltaUV1 = t0 - t1;
			glm::vec2 deltaUV2 = t0 - t2;

			tangent1 = glm::cross(vertices[indices[0]].normal, edge1);
			bitangent1 = glm::cross(vertices[indices[0]].normal, tangent1);
			for (int i = 0; i < indices.size(); i += 3)
			{
				vertices[indices[i]].tangent = glm::normalize(tangent1);
				vertices[indices[i + 1]].tangent = glm::normalize(tangent1);
				vertices[indices[i + 2]].tangent = glm::normalize(tangent1);

				// bitangent1 = glm::normalize(bitangent1);

				vertices[indices[i]].bitangent = glm::normalize(bitangent1);
				vertices[indices[i + 1]].bitangent = glm::normalize(bitangent1);
				vertices[indices[i + 2]].bitangent = glm::normalize(bitangent1);
			}
		}
	};
}

#endif // EPSILON_BSP_HPP
