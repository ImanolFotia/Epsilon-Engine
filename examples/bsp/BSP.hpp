//
// Created by solaire on 1/2/23.
//

#ifndef EPSILON_BSP_HPP
#define EPSILON_BSP_HPP


#include <Epsilon.hpp>
#include <beacon/beacon.hpp>

#include "SourceBSP.hpp"
#include <core/framework/utils/helpers/camera.hpp>

namespace BSP {
    class BSP : public Epsilon::Epsilon {


        struct PushConstant {
            alignas(16) glm::mat4 model;
        };

        struct BSPFace {
            engine::Ref<engine::Mesh> mesh;
            engine::Ref<engine::Texture> texture;
            engine::Ref<engine::Material> material;
        };

        struct BSPMap {
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


    public:

        explicit BSP(const std::string &appname) : Epsilon::Epsilon(appname) {
            m_pCamera = std::make_shared<utils::Camera>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        }

        void onCreate() override {

            setupRenderPass();

            engine::MaterialInfo material = {
                    .bindingInfo = {
                            .size = sizeof(ShaderData),
                            .offset = 0
                    }
            };

            shadowDummyMaterial = m_pContext.ResourceManager()->createMaterial(material, m_pShadowRenderPass);

            dummyMaterial = m_pContext.ResourceManager()->createMaterial(material, m_pRenderPass,
                                                                         {{.renderPass = m_pShadowRenderPass, .index = 0}});

            m_pMap.pushConstant.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.0125f));

            m_pMap.pushConstantRef = m_pContext.ResourceManager()->createPushConstant(
                    {.size = sizeof(PushConstant), .data = &m_pMap.pushConstant});

            const char *filename = "../../../assets/models/hl2/background01.bsp";

            SourceBSP bspMap(filename);

            auto calculateNormals = [](std::vector<engine::Vertex> &vertices,
                                       const std::vector<unsigned int> &indices) -> void {

                auto &v1 = vertices[indices[0]].position;
                auto &v2 = vertices[indices[1]].position;
                auto &v3 = vertices[indices[2]].position;

                glm::vec3 n = glm::cross(v3 - v1, v2 - v1);

                n = glm::normalize(n);

                for (auto& v: vertices) {
                    v.normal = n;
                }

            };

            for (unsigned i = 0; i < bspMap.Faces().size(); i++) {
                if (bspMap.Faces()[i].vertices.size() <= 0) continue;
                auto &face = m_pMap.m_pFaces.emplace_back();

                std::vector<engine::Vertex> vertices;

                for (auto &vtx: bspMap.Faces()[i].vertices) {
                    auto &v = vertices.emplace_back();
                    v.position = vtx;
                    if (bspMap.Faces()[i].tool == true) {
                        v.color = glm::vec4(0.2, 0.5, 1.0, 0.5);
                    } else if (bspMap.Faces()[i].trigger == true) {
                        v.color = glm::vec4(0.5, 0.5, 0.0, 0.5);
                    } else {
                        v.color = glm::vec4(0.0, 0.0, 0.0, 1.0);
                    }
                }

                calculateNormals(vertices, bspMap.Faces()[i].indices);

                face.mesh = m_pContext.ResourceManager()->createMesh({
                                                                             .vertices = vertices,
                                                                             .indices = bspMap.Faces()[i].indices,
                                                                     });

            }

        }

        void onReady() override {

            std::cout << m_pMap.m_pFaces.size() << " faces" << std::endl;
        }

        void onRender() override {


            engine::ObjectData objectData;


            setupShadowCamera();


            for (auto &face: m_pMap.m_pFaces) {
                objectData.layout_index = 0;
                objectData.mesh = face.mesh;
                objectData.material = shadowDummyMaterial;
                objectData.modelMatrix = glm::mat4(1.0f);
                objectData.pushConstant = m_pMap.pushConstantRef;
                m_pContext.Renderer()->Push(objectData);
            }

            m_pContext.Renderer()->Begin(m_pShadowRenderPass);
            m_pContext.Renderer()->Flush(m_pShadowRenderPass);

            setupCamera();

            for (auto &face: m_pMap.m_pFaces) {
                objectData.layout_index = 0;
                objectData.mesh = face.mesh;
                objectData.material = dummyMaterial;
                objectData.modelMatrix = glm::mat4(1.0f);
                objectData.pushConstant = m_pMap.pushConstantRef;
                m_pContext.Renderer()->Push(objectData);
            }

            drawFrame(m_pRenderPass);
        }

        void onExit() override {}


    private:

        bool updateCam = true;
        double m_pTime = 0.0;

        void setupCamera() {


            framework::Input::KeyBoard::KeyboardEventHandler.addListener(([this](auto *sender, beacon::args *args) {
                if (args == nullptr)
                    return;

                auto obj = args->to<framework::Input::KeyboardArgs>();

                //std::cout << "pressed" << std::endl;

                if (obj.key_up_index == framework::Input::GLFW::Key::M) {
                    updateCam = !updateCam;
                }

                if (obj.key_up_index == framework::Input::GLFW::Key::P) {
                    auto pos = m_pCamera->getPosition();
                    auto dir = m_pCamera->getDirection();
                    std::cout << pos.x << " " << pos.y << " " << pos.z << std::endl;
                    std::cout << dir.x << " " << dir.y << " " << dir.z << std::endl;
                }
            }));

            ShaderData camData;
            camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);

            static auto startTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            m_pTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            if (updateCam) {
                m_pContext.Window().HideCursor();
                m_pCamera->Update(m_pContext.Window().getWindow());
                m_pCamera->UpdateMatrices(0, getWindowDimensions().first, getWindowDimensions().second, false);
            } else {
                m_pContext.Window().ShowCursor();
            }
            camData.view = m_pCamera->getViewMatrix();
            camData.proj = m_pCamera->getProjectionMatrix();
            camData.lightMatrix = lightMatrix;
            camData.proj[1][1] *= -1;
            camData.iTime += m_pTime;
            PushShaderData(camData);
        }

        glm::mat4 lightMatrix = glm::mat4(1.0);

        void setupShadowCamera() {

            ShaderData camData;

            camData.iResolution = glm::vec2(2000, 2000);

            //30.6363, 13.117, -38.7729

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
            camData.iTime += m_pTime;
            PushShaderData(camData);
        }


        void setupRenderPass() {

            using namespace engine;
            //Load the shader that draws the board
            auto vertexCode = utils::readFile("../../../assets/shaders/vertex.spv");
            auto fragmentCode = utils::readFile("../../../assets/shaders/fragment.spv");

            auto shadowVertexCode = utils::readFile("../../../assets/shaders/shadow-vertex.spv");
            auto shadowFragmentCode = utils::readFile("../../../assets/shaders/shadow-fragment.spv");

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

            std::vector<VertexDescriptorInfo> vertexInfo = {{XYZ_FLOAT,  offsetof(Vertex, position)},
                                                            {XY_FLOAT,   offsetof(Vertex, texCoords)},
                                                            {XYZ_FLOAT,  offsetof(Vertex, normal)},
                                                            {XYZW_FLOAT, offsetof(Vertex, color)},
                                                            {XYZ_FLOAT,  offsetof(Vertex, tangent)},
                                                            {XYZ_FLOAT,  offsetof(Vertex, bitangent)}};
            PipelineLayout mainLayout = {
                    .shaderInfo = mainShaderInfo,
                    .vertexLayout = vertexInfo,
                    .cullMode = CullMode::BACK,
                    .windingMode = WindingMode::CLOCKWISE
            };

            PipelineLayout shadowLayout = {
                    .shaderInfo = shadowShaderInfo,
                    .vertexLayout = vertexInfo,
                    .cullMode = CullMode::FRONT,
                    .windingMode = WindingMode::CLOCKWISE
            };

            //Configure the default render pass object
            RenderPassInfo renderPassInfo =
                    RenderPassFactory()
                            .numDescriptors(6)
                            .size(sizeof(Vertex))
                            .depthAttachment(true)
                            .subpasses({})
                            .dimensions({.width = 1280, .height = 720})
                            .attachments(
                                    {
                                            {
                                                    .format = COLOR_RGBA,
                                                    .isDepthAttachment = false,
                                                    .isSwapChainAttachment = true,
                                            },
                                            {
                                                    .format = DEPTH_F32_STENCIL_8,
                                                    .isDepthAttachment = true
                                            }
                                    })
                            .pipelineLayout(mainLayout)
                            .pushConstant(sizeof(PushConstant))
                            .bufferInfo({.size = sizeof(ShaderData), .offset = 0});


            RenderPassInfo shadowRenderPassInfo =
                    RenderPassFactory()
                            .numDescriptors(6)
                            .size(sizeof(Vertex))
                            .depthAttachment(true)
                            .subpasses({})
                            .dimensions({.width = 2000, .height = 2000})
                            .attachments(
                                    {
                                            {
                                                    .format = COLOR_R_32F,
                                                    .wrapMode = CLAMP_TO_BORDER,
                                                    .filtering = LINEAR,
                                                    .compareFunc = LESS_OR_EQUAL,
                                                    .depthCompare = true,
                                                    .isSampler = true,
                                                    .isDepthAttachment = false,
                                                    .isSwapChainAttachment = false
                                            },
                                            {
                                                    .format = DEPTH_F32_STENCIL_8,
                                                    .isDepthAttachment = true
                                            }
                                    })
                            .pipelineLayout(shadowLayout)
                            .pushConstant(sizeof(PushConstant))
                            .bufferInfo({.size = sizeof(ShaderData), .offset = 0});

            m_pRenderPass = m_pContext.ResourceManager()->createDefaultRenderPass(renderPassInfo);
            m_pShadowRenderPass = m_pContext.ResourceManager()->createRenderPass(shadowRenderPassInfo);

        }

    };

}
#endif //EPSILON_BSP_HPP
