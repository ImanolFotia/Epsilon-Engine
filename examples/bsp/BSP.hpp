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
        };

        struct BSPMap
        {
            std::list<BSPFace> m_pFaces;
            engine::Ref<engine::PushConstant> pushConstantRef;
            PushConstant pushConstant;
        };

        std::vector<int> m_pVisibleFaces;

        engine::Ref<engine::RenderPass> m_pRenderPass;
        engine::Ref<engine::RenderPass> m_pShadowRenderPass;

        BSPMap m_pMap;

        std::shared_ptr<utils::Camera> m_pCamera;

        engine::Ref<engine::Material> dummyMaterial;
        engine::Ref<engine::Material> shadowDummyMaterial;
        engine::Ref<engine::Buffer> gpuBuffer;

    public:
        explicit BSP(const std::string &appname) : Epsilon::Epsilon(appname)
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

            engine::MaterialInfo material = {
                .name = "DefaultMaterial",
                .bindingInfo = {{.size = sizeof(ShaderData), .type = en::UNIFORM_BUFFER, .binding = 0}}};

            engine::MaterialInfo shadowMaterial = {
                .name = "ShadowMaterial",
                .bindingInfo = {{.size = sizeof(ShaderData), .type = en::UNIFORM_BUFFER, .binding = 0}}};

            shadowDummyMaterial = Epsilon::getContext().ResourceManager()->createMaterial(shadowMaterial,
                                                                                          m_pShadowRenderPass);

            dummyMaterial = Epsilon::getContext().ResourceManager()->createMaterial(
                material,
                m_pRenderPass,
                {{.renderPass = "Shadow", .index = 0, .bindingPoint = 1},
                 {.renderPass = "Shadow", .index = 1, .bindingPoint = 2}});

            m_pMap.pushConstant.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.0125f));

            m_pMap.pushConstantRef = Epsilon::getContext().ResourceManager()->createPushConstant(
                "ModelMatrix",
                {.size = sizeof(PushConstant), .data = &m_pMap.pushConstant});

            // gpuBuffer = Epsilon::getContext().ResourceManager()->createGPUBuffer(sizeof(PushConstant), engine::BufferStorageType::STORAGE_BUFFER);

            const char *filename = "./assets/models/hl2/background01.bsp";

            SourceBSP bspMap(filename);

            auto calculateNormals = [](std::vector<engine::Vertex> &vertices,
                                       const std::vector<unsigned int> &indices) -> void
            {
                auto &v1 = vertices[indices[0]].position;
                auto &v2 = vertices[indices[1]].position;
                auto &v3 = vertices[indices[2]].position;

                glm::vec3 n = glm::cross(v3 - v1, v2 - v1);

                n = glm::normalize(n);

                if (glm::isnan(n.x))
                {
                    n = glm::vec3(0.0, 1.0, 0.0);
                }

                for (auto &v : vertices)
                {
                    v.normal = n;
                }
            };

            for (unsigned i = 0; i < bspMap.Faces().size(); i++)
            {
                if (bspMap.Faces()[i].vertices.size() <= 0)
                    continue;
                auto &face = m_pMap.m_pFaces.emplace_back();

                std::vector<engine::Vertex> vertices;

                for (auto &vtx : bspMap.Faces()[i].vertices)
                {
                    auto &v = vertices.emplace_back();
                    v.position = vtx;
                    if (bspMap.Faces()[i].tool == true)
                    {
                        v.color = glm::vec4(0.2, 0.5, 1.0, 0.5);
                    }
                    else if (bspMap.Faces()[i].trigger == true)
                    {
                        v.color = glm::vec4(0.5, 0.5, 0.0, 0.5);
                    }
                    else
                    {
                        v.color = glm::vec4(0.0, 0.0, 0.0, 1.0);
                    }
                }

                for (int j = 0; j < bspMap.Faces()[i].uvs.size(); j++)
                {
                    vertices[j].texCoords = bspMap.Faces()[i].uvs[j];
                }

                calculateNormals(vertices, bspMap.Faces()[i].indices);

                face.mesh = Epsilon::getContext().ResourceManager()->createMesh({
                    .name = "BSPFace" + std::to_string(i),
                    .vertices = vertices,
                    .indices = bspMap.Faces()[i].indices,
                });
            }
        }

        void onReady()
        {

            std::cout << m_pMap.m_pFaces.size() << " faces" << std::endl;
        }

        void onRender()
        {

            engine::ObjectData objectData;

            setupShadowCamera();

            for (auto &face : m_pMap.m_pFaces)
            {
                objectData.layout_index = 0;
                objectData.mesh = face.mesh;
                objectData.material = shadowDummyMaterial;
                objectData.modelMatrix = glm::mat4(1.0f);
                objectData.pushConstant = m_pMap.pushConstantRef;
                Epsilon::getContext().Renderer()->Push(objectData);
            }

            Epsilon::getContext().Renderer()->Begin();
            Epsilon::getContext().Renderer()->Flush(m_pShadowRenderPass, engine::DrawType::INDEXED_INDIRECT);

            setupCamera();

            for (auto &face : m_pMap.m_pFaces)
            {
                objectData.layout_index = 0;
                objectData.mesh = face.mesh;
                objectData.material = dummyMaterial;
                objectData.modelMatrix = glm::mat4(1.0f);
                objectData.pushConstant = m_pMap.pushConstantRef;
                Epsilon::getContext().Renderer()->Push(objectData);
            }

            // drawFrame(m_pRenderPass);

            engine::Context::getSingleton().Renderer()->Flush(m_pRenderPass, engine::DrawType::INDEXED_INDIRECT);
            engine::Context::getSingleton().Renderer()->End();
            engine::Context::getSingleton().Renderer()->Sync();
        }

        void onExit() {}

    private:
        bool updateCam = true;
        double m_pTime = 0.0;

        void setupCamera()
        {

            framework::Input::KeyBoard::KeyboardEventHandler.addListener(([this](auto *sender, beacon::args *args)
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

            ShaderData camData;
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

            Epsilon::getContext().Renderer()->UpdateRenderPassUniforms(m_pRenderPass, engine::RENDERPASS_SET, &camData);
            // PushShaderData(camData);
        }

        glm::mat4 lightMatrix = glm::mat4(1.0);

        void setupShadowCamera()
        {

            ShaderData camData;

            camData.iResolution = glm::vec2(3000, 3000);

            // 30.6363, 13.117, -38.7729

            camData.view = glm::lookAt(glm::vec3(23.6365, 25.117, -45.7729),
                                       glm::vec3(23.6365, 0.897448, -22.8326),
                                       glm::vec3(0.0f, 1.0f, 0.0f));

            camData.proj = glm::ortho(-25.0f,
                                      25.0f,
                                      -25.0f,
                                      25.0f, -20.f, 100.0f);

            camData.lightPosition = m_pCamera->getPosition();
            camData.proj[1][1] *= -1;
            lightMatrix = camData.proj * camData.view * glm::mat4(1.0);
            camData.lightMatrix = lightMatrix;
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

            ShaderInfo mainShaderInfo = {
                .stages = {
                    {.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
                    {.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}},
                .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

            ShaderInfo shadowShaderInfo = {
                .stages = {
                    {.entryPoint = "main", .shaderCode = shadowVertexCode, .stage = VERTEX},
                    {.entryPoint = "main", .shaderCode = shadowFragmentCode, .stage = FRAGMENT}},
                .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

            std::vector<VertexDescriptorInfo> vertexInfo = {{XYZ_FLOAT, offsetof(Vertex, position)},
                                                            {XY_FLOAT, offsetof(Vertex, texCoords)},
                                                            {XYZ_FLOAT, offsetof(Vertex, normal)},
                                                            {XYZW_FLOAT, offsetof(Vertex, color)},
                                                            {XYZ_FLOAT, offsetof(Vertex, tangent)},
                                                            {XYZ_FLOAT, offsetof(Vertex, bitangent)}};
            PipelineLayout mainLayout = {
                .shaderInfo = mainShaderInfo,
                .vertexLayout = vertexInfo,
                .cullMode = CullMode::BACK,
                .windingMode = WindingMode::CLOCKWISE};

            PipelineLayout shadowLayout = {
                .shaderInfo = shadowShaderInfo,
                .vertexLayout = vertexInfo,
                .cullMode = CullMode::FRONT,
                .windingMode = WindingMode::CLOCKWISE};

            // Configure the default render pass object
            RenderPassInfo renderPassInfo =
                RenderPassFactory()
                    .name("Default")
                    .numDescriptors(6)
                    .size(sizeof(Vertex))
                    .depthAttachment(true)
                    .subpasses({})
                    .dimensions({.width = 1920, .height = 1080})
                    .attachments({{
                                      .format = COLOR_RGBA,
                                      .isDepthAttachment = false,
                                      .isSwapChainAttachment = true,
                                  },
                                  {.format = DEPTH_F32_STENCIL_8,
                                   .isDepthAttachment = true}})
                    .pipelineLayout(mainLayout)
                    .pushConstant(sizeof(PushConstant))
                    .uniformBindings({{.size = sizeof(ShaderData), .offset = 0, .binding = 0, .type = UniformBindingType::UNIFORM_BUFFER},
                                      {.size = 0, .offset = 0, .binding = 1, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER},
                                      {.size = 0, .offset = 0, .binding = 2, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER},
                                      /*{.size = 0, .offset = 0, .binding = 3, .type = UniformBindingType::SHADER_STORAGE}*/});

            RenderPassInfo shadowRenderPassInfo =
                RenderPassFactory()
                    .name("Shadow")
                    .numDescriptors(6)
                    .size(sizeof(Vertex))
                    .depthAttachment(true)
                    .subpasses({})
                    .dimensions({.width = 3000, .height = 3000})
                    .attachments(
                        {{.format = COLOR_R_32F,
                          .wrapMode = CLAMP_TO_BORDER,
                          .filtering = LINEAR,
                          .compareFunc = LESS_OR_EQUAL,
                          .depthCompare = true,
                          .isSampler = true,
                          .isDepthAttachment = false,
                          .isSwapChainAttachment = false},
                         {.format = COLOR_R_32F,
                          .wrapMode = CLAMP_TO_BORDER,
                          .filtering = engine::POINT,
                          .compareFunc = ALWAYS,
                          .depthCompare = false,
                          .isSampler = true,
                          .isDepthAttachment = false,
                          .isSwapChainAttachment = false},

                         {.format = DEPTH_F32_STENCIL_8,
                          .isDepthAttachment = true}})
                    .pipelineLayout(shadowLayout)
                    .pushConstant(sizeof(PushConstant))
                    .uniformBindings({{.size = sizeof(ShaderData), .offset = 0, .binding = 0, .type = UniformBindingType::UNIFORM_BUFFER},
                                      {.size = 0, .offset = 0, .binding = 1, .type = UniformBindingType::TEXTURE_IMAGE_COMBINED_SAMPLER},
                                      /*{.size = 0, .offset = 0, .binding = 3, .type = UniformBindingType::SHADER_STORAGE}*/});

            m_pRenderPass = Epsilon::getContext().ResourceManager()->createDefaultRenderPass(renderPassInfo);
            m_pShadowRenderPass = Epsilon::getContext().ResourceManager()->createRenderPass(shadowRenderPassInfo);
        }
    };

}
#endif // EPSILON_BSP_HPP
