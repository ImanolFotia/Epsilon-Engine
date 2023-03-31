//
// Created by solaire on 1/2/23.
//

#ifndef EPSILON_BSP_HPP
#define EPSILON_BSP_HPP

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
			engine::Ref<engine::Material> material;
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
			uint16_t diffuse = 0;
			uint16_t normal = 0;
			uint16_t roughness = 0;
			uint16_t metallic = 0;
		};

		std::vector<int> m_pVisibleFaces;

		engine::Ref<engine::RenderPass> m_pRenderPass;
		engine::Ref<engine::RenderPass> m_pShadowRenderPass;

		BSPMap m_pMap;

		std::shared_ptr<utils::Camera> m_pCamera;

		engine::Ref<engine::Material> dummyMaterial;
		engine::Ref<engine::Material> skyMaterial;
		engine::Ref<engine::Material> shadowDummyMaterial;
		engine::Ref<engine::Buffer> gpuBuffer;

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

			std::array<std::string, 4> pbrTextures = {
				"./assets/images/textures/Pavement.png",
				"./assets/images/textures/Pavement_m.png",
				"./assets/images/textures/Pavement_n.png",
				"./assets/images/textures/Pavement_s.png" };
			std::array<engine::TextureCreationInfo, 4> textureInfos;
			//{
			int index = 0;
			for (auto& texture : pbrTextures)
			{
				int w, h, nc;
				unsigned char* pixels = framework::load_image_from_file(texture.c_str(),
					&w,
					&h,
					&nc);
				engine::TextureCreationInfo texInfo = engine::TextureBuilder()
					.width(w)
					.height(h)
					.numChannels(nc)
					.pixels(pixels)
					.name(texture);
				texInfo.filtering = engine::LINEAR;
				texInfo.wrapMode = engine::REPEAT;
				texInfo.format = (index == 0 ? engine::COLOR_RGBA : engine::NON_COLOR_RGBA);
				textureInfos[index] = (texInfo);
				index++;
			}
			engine::MaterialInfo material = {
				.bindingInfo = {{.size = sizeof(ShaderData), .binding = 0, .type = en::UNIFORM_BUFFER},
								{.size = 0, .offset = 0, .binding = 3, .type = en::TEXTURE_IMAGE_COMBINED_SAMPLER, .textureInfo = textureInfos[0]},
								{.size = 0, .offset = 0, .binding = 4, .type = en::TEXTURE_IMAGE_COMBINED_SAMPLER, .textureInfo = textureInfos[1]},
								{.size = 0, .offset = 0, .binding = 5, .type = en::TEXTURE_IMAGE_COMBINED_SAMPLER, .textureInfo = textureInfos[2]},
								{.size = 0, .offset = 0, .binding = 6, .type = en::TEXTURE_IMAGE_COMBINED_SAMPLER, .textureInfo = textureInfos[3]}},

				.name = "DefaultMaterial",
			};

			dummyMaterial = Epsilon::getContext().ResourceManager()->createMaterial(
				material,
				m_pRenderPass,
				{
					{.renderPass = "Shadow", .index = 1, .bindingPoint = 1},
					{.renderPass = "Shadow", .index = 0, .bindingPoint = 2},
				});

			engine::MaterialInfo skyInfo = {
				 .bindingInfo = {{.size = sizeof(ShaderData), .binding = 0, .type = en::UNIFORM_BUFFER}},
				 .name = "SkyMaterial"
			};

			skyMaterial = Epsilon::getContext().ResourceManager()->createMaterial(skyInfo, m_pRenderPass);

			for (auto& texture : textureInfos)
			{
				framework::free_image_data(texture.pixels);
			}

			engine::MaterialInfo shadowMaterial = {
				.bindingInfo = {{
					.size = sizeof(ShaderData),
					.binding = 0,
					.type = en::UNIFORM_BUFFER,
				}},
				.name = "ShadowMaterial",
			};

			shadowDummyMaterial = Epsilon::getContext().ResourceManager()->createMaterial(shadowMaterial,
				m_pShadowRenderPass);

			m_pMap.pushConstant.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.0125f));

			m_pMap.pushConstantRef = Epsilon::getContext().ResourceManager()->createPushConstant(
				"ModelMatrix",
				{ .size = sizeof(PushConstant), .data = &m_pMap.pushConstant });

			gpuBuffer = Epsilon::getContext().ResourceManager()->createGPUBuffer("material_buffer", sizeof(Material) * 100, engine::BufferStorageType::STORAGE_BUFFER);

			const char* filename = "./assets/models/hl2/background01.bsp";

			SourceBSP bspMap(filename);

			auto calculateNormals = [](std::vector<engine::Vertex>& vertices,
				const std::vector<unsigned int>& indices) -> void
			{
				uint32_t size = indices.size();
				auto v1 = vertices[indices[0]].position;
				auto v2 = vertices[indices[1]].position;
				auto v3 = vertices[indices[size - 1]].position;


				glm::vec3 n = glm::cross(v3 - v1, v2 - v1);

				n = glm::normalize(n);

				if (glm::isnan(n.x))
				{
					v1 = vertices[indices[size - 1]].position;
					n = glm::cross(v3 - v1, v2 - v1);
				}

				for (auto& v : vertices)
				{
					v.normal = n;
				}

			};

			auto generateTangentSpaceVectors = [](std::vector<engine::Vertex>& vertices, const std::vector<uint32_t>& indices)
			{
				uint32_t size = indices.size();
				uint32_t num_triangles = indices.size() / 3;
				// calculate tangent/bitangent vectors of both triangles
				glm::vec3 tangent1, bitangent1;
				// std::cout << m_pMesh.m_Tris.size() <<  std::endl;
				//for (int i = 0; i < indices.size(); i += 3)
				//{
					glm::vec3 v0 = vertices[indices[0]].position;
					glm::vec3 v1 = vertices[indices[1]].position;
					glm::vec3 v2 = vertices[indices[size - 1]].position;

					glm::vec3 edge1 = v1 - v0;
					glm::vec3 edge2 = v2 - v0;

					glm::vec2 t0 = vertices[indices[0]].texCoords / 512.0f;
					glm::vec2 t1 = vertices[indices[1]].texCoords / 512.0f;
					glm::vec2 t2 = vertices[indices[size - 1]].texCoords / 512.0f;

					glm::vec2 deltaUV1 = t1 - t0;
					glm::vec2 deltaUV2 = t2 - t0;

					float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;

					tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
					tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
					tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

					bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
					bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
					bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
					// tangent1 = glm::normalize(tangent1);
					for (int i = 0; i < indices.size(); i += 3) {
						vertices[indices[i]].tangent = glm::normalize(tangent1);
						vertices[indices[i + 1]].tangent = glm::normalize(tangent1);
						vertices[indices[i + 2]].tangent = glm::normalize(tangent1);

						// bitangent1 = glm::normalize(bitangent1);

						vertices[indices[i]].bitangent = glm::normalize(bitangent1);
						vertices[indices[i + 1]].bitangent = glm::normalize(bitangent1);
						vertices[indices[i + 2]].bitangent = glm::normalize(bitangent1);
					}
				};

				// m_pMap.m_pFaces.resize(bspMap.Faces().size());
				for (unsigned i = 0; i < bspMap.Faces().size(); i++)
				{
					if (bspMap.Faces()[i].vertices.size() <= 0)
						continue;
					BSPFace face;

					std::vector<engine::Vertex> vertices;
					vertices.resize(bspMap.Faces()[i].vertices.size());
					int index{};
					for (auto& vtx : bspMap.Faces()[i].vertices)
					{
						auto& v = vertices.at(index);
						v.position = vtx;
						face.layoutIndex = 0;
						face.material = dummyMaterial;
						if (bspMap.Faces()[i].tool == true)
						{
							v.color = glm::vec4(0.2, 0.5, 1.0, 0.5);
							face.material = skyMaterial;
							face.layoutIndex = 1;
						}
						else if (bspMap.Faces()[i].trigger == true)
						{
							v.color = glm::vec4(0.5, 0.5, 0.0, 0.5);
						}
						else
						{
							v.color = glm::vec4(0.0, 0.0, 0.0, 1.0);
						}
						index++;
					}

					for (int j = 0; j < bspMap.Faces()[i].uvs.size(); j++)
					{
						vertices[j].texCoords = bspMap.Faces()[i].uvs[j];
					}

					calculateNormals(vertices, bspMap.Faces()[i].indices);
					//if (!res) continue;

					generateTangentSpaceVectors(vertices, bspMap.Faces()[i].indices);

					face.mesh = Epsilon::getContext().ResourceManager()->createMesh({
						.vertices = vertices,
						.indices = bspMap.Faces()[i].indices,
						.name = "BSPFace" + std::to_string(i),
						});

					m_pMap.m_pFaces.push_back(face);

				}
			}

				void onReady()
			{

				std::cout << m_pMap.m_pFaces.size() << " faces" << std::endl;
				camData.lightPosition = glm::vec3(1.0, 50.0, 0.0);
			}

			void onRender()
			{

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

				for (auto& face : m_pMap.m_pFaces)
				{

					objectData.layout_index = face.layoutIndex;
					objectData.mesh = face.mesh;
					objectData.material = face.material;
					objectData.modelMatrix = glm::mat4(1.0f);
					objectData.pushConstant = m_pMap.pushConstantRef;
					objectData.uniformIndex = 0;
					Epsilon::getContext().Renderer()->Push(objectData);
				}

				// drawFrame(m_pRenderPass);
				engine::Context::getSingleton().Renderer()->Flush(m_pRenderPass, engine::DrawType::INDEXED_INDIRECT);
				engine::Context::getSingleton().Renderer()->End(camData.lightPosition);

				engine::Context::getSingleton().Renderer()->Submit();
				engine::Context::getSingleton().Renderer()->EndFrame();
			}

			void onExit() {}

		private:
			bool updateCam = true;
			double m_pTime = 0.0;
			ShaderData camData;

			void setupCamera()
			{

				framework::Input::KeyBoard::KeyboardEventHandler.addListener(
					([this](auto* sender, beacon::args* args)
						{
							if (args == nullptr)
								return;

							auto obj = args->to<framework::Input::KeyboardArgs>();

							// std::cout << "pressed" << std::endl;
#if USE_GLFW
							if (obj.key_up_index == framework::Input::GLFW::Key::M)
							{
								updateCam = !updateCam;
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

				camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);

				static auto startTime = std::chrono::high_resolution_clock::now();
				auto currentTime = std::chrono::high_resolution_clock::now();
				m_pTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

				if (updateCam)
				{
					Epsilon::getContext().Window().HideCursor();
					m_pCamera->Update(Epsilon::getContext().Window().getWindow());
					m_pCamera->UpdateMatrices(0, getWindowDimensions().first, getWindowDimensions().second, false);
				}
				else
				{
					Epsilon::getContext().Window().ShowCursor();
				}
				camData.view = m_pCamera->getViewMatrix();
				camData.proj = m_pCamera->getProjectionMatrix();
				camData.lightMatrix = lightMatrix;
				camData.proj[1][1] *= -1;
				camData.iTime += m_pTime;
				camData.viewPosition = m_pCamera->getPosition();

				Epsilon::getContext().Renderer()->UpdateRenderPassUniforms(m_pRenderPass, engine::RENDERPASS_SET, &camData);
				// PushShaderData(camData);
			}

			glm::mat4 lightMatrix = glm::mat4(1.0);

			void setupShadowCamera()
			{

				camData.iResolution = glm::vec2(3000, 3000);

				// 30.6363, 13.117, -38.7729

				camData.view = glm::lookAt(glm::vec3(23.6365, 25.117, -45.7729),
					glm::vec3(23.6365, 0.897448, -22.8326),
					glm::vec3(0.0f, 1.0f, 0.0f));

				camData.proj = glm::ortho(-25.0f,
					25.0f,
					-25.0f,
					25.0f, -20.f, 100.0f);

				// camData.lightPosition = m_pCamera->getPosition();
				camData.proj[1][1] *= -1;
				lightMatrix = camData.proj * camData.view * glm::mat4(1.0);
				camData.lightMatrix = lightMatrix;
				camData.viewPosition = m_pCamera->getPosition();
				camData.iTime += m_pTime;
				Epsilon::getContext().Renderer()->UpdateRenderPassUniforms(m_pShadowRenderPass, engine::RENDERPASS_SET, &camData);
				// PushShaderData(camData);
			}

			void setupRenderPass()
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

				std::vector<VertexDescriptorInfo> vertexInfo = { {XYZ_FLOAT, offsetof(Vertex, position)},
																{XY_FLOAT, offsetof(Vertex, texCoords)},
																{XYZ_FLOAT, offsetof(Vertex, normal)},
																{XYZW_FLOAT, offsetof(Vertex, color)},
																{XYZ_FLOAT, offsetof(Vertex, tangent)},
																{XYZ_FLOAT, offsetof(Vertex, bitangent)} };
				PipelineLayout mainLayout = {
					.shaderInfo = mainShaderInfo,
					.vertexLayout = vertexInfo,
					.cullMode = CullMode::BACK,
					.windingMode = WindingMode::CLOCKWISE };

				PipelineLayout shadowLayout = {
					.shaderInfo = shadowShaderInfo,
					.vertexLayout = vertexInfo,
					.cullMode = CullMode::FRONT,
					.windingMode = WindingMode::CLOCKWISE };

				PipelineLayout skyLayout = {
					.shaderInfo = skyShaderInfo,
					.vertexLayout = vertexInfo,
					.cullMode = CullMode::BACK,
					.windingMode = WindingMode::CLOCKWISE };

				// Configure the default render pass object
				RenderPassInfo renderPassInfo =
					RenderPassFactory()
					.name("Default")
					.numDescriptors(6)
					.size(sizeof(Vertex))
					.depthAttachment(true)
					.subpasses({})
					.dimensions({ .width = 1920, .height = 1080 })
					.attachments({ {
									  .format = COLOR_RGBA,
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
					.pushConstant(sizeof(PushConstant))
					.uniformBindings({ {.size = sizeof(ShaderData), .offset = 0, .binding = 0, .type = UniformBindingType::UNIFORM_BUFFER},
									  {.size = 0, .offset = 0, .binding = 1, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER},
									  {.size = 0, .offset = 0, .binding = 2, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER}
				/*{.size = 0, .offset = 0, .binding = 3, .type = UniformBindingType::SHADER_STORAGE}*/ });

				RenderPassInfo shadowRenderPassInfo =
					RenderPassFactory()
					.name("Shadow")
					.numDescriptors(6)
					.size(sizeof(Vertex))
					.depthAttachment(true)
					.subpasses({})
					.dimensions({ .width = 1500, .height = 1500 })
					.attachments(
						{ {.format = COLOR_R_32F,
						  .wrapMode = CLAMP_TO_BORDER,
						  .filtering = engine::POINT,
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
					.pushConstant(sizeof(PushConstant))
					.uniformBindings({ {.size = sizeof(ShaderData), .offset = 0, .binding = 0, .type = UniformBindingType::UNIFORM_BUFFER},
									  {.size = 0, .offset = 0, .binding = 1, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER},
						/*{.size = 0, .offset = 0, .binding = 3, .type = UniformBindingType::SHADER_STORAGE}*/ });

				m_pRenderPass = Epsilon::getContext().ResourceManager()->createDefaultRenderPass(renderPassInfo);
				m_pShadowRenderPass = Epsilon::getContext().ResourceManager()->createRenderPass(shadowRenderPassInfo);
			}
		};
	}
#endif // EPSILON_BSP_HPP
