//
// Created by solaire on 1/2/23.
//

#ifndef EPSILON_BSP_HPP
#define EPSILON_BSP_HPP


#include <Epsilon.hpp>
#include <beacon/beacon.hpp>

#include "SourceBSP.hpp"
#include <framework/utils/helpers/camera.hpp>

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

        BSPMap m_pMap;

        std::shared_ptr<utils::Camera> m_pCamera;

        engine::Ref<engine::Material> dummyMaterial;

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
            dummyMaterial = m_pContext.ResourceManager()->createMaterial(material, m_pRenderPass);

            m_pMap.pushConstant.model = glm::scale(glm::mat4(1.0f), glm::vec3(0.0125f));

            m_pMap.pushConstantRef = m_pContext.ResourceManager()->createPushConstant(
                    {.size = sizeof(PushConstant), .data = &m_pMap.pushConstant});

            const char *filename = "../../../assets/models/hl2/d3_citadel_05.bsp";

            SourceBSP bspMap(filename);

            for (unsigned i = 0; i < bspMap.Faces().size(); i++) {
                if (bspMap.Faces()[i].vertices.size() <= 0) continue;
                auto &face = m_pMap.m_pFaces.emplace_back();

                std::vector<engine::Vertex> vertices;

                for (auto &vtx: bspMap.Faces()[i].vertices) {
                    auto &v = vertices.emplace_back();
                    v.position = vtx;
                    if(bspMap.Faces()[i].tool == true) {
                        v.color = glm::vec4(0.2, 0.5, 1.0, 0.5);
                    } else if(bspMap.Faces()[i].trigger == true){
                        v.color = glm::vec4(0.5, 0.5, 0.0, 0.5);
                    } else {
                        v.color = glm::vec4(0.0, 0.0, 0.0, 1.0);
                    }
                }

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

            setupCamera();

            engine::ObjectData objectData;

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

        void setupCamera() {


            framework::Input::KeyBoard::KeyboardEventHandler.addListener(([this](auto *sender, beacon::args *args) {
                if (args == nullptr)
                    return;

                auto obj = args->to<framework::Input::KeyboardArgs>();

                if (obj.key_up_index == framework::Input::GLFW::Key::M) {
                    updateCam = !updateCam;
                }
            }));

            ShaderData camData;
            camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);

            static auto startTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            if (updateCam) {
                m_pContext.Window().HideCursor();
                m_pCamera->Update(m_pContext.Window().getWindow());
                m_pCamera->UpdateMatrices(0, getWindowDimensions().first, getWindowDimensions().second, false);
            } else {
                m_pContext.Window().ShowCursor();
            }
/*
            camData.view = glm::lookAt(glm::vec3(300.0f, 300.0f, 300.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(0.0f, 0.0f, 1.0f));


            camData.proj = glm::perspective(glm::radians(90.0f), getWindowDimensions().first /
                                                                 ((float) getWindowDimensions().second +
                                                                  std::numeric_limits<float>::epsilon()), 0.1f, 3000.0f);
*/
            camData.view = m_pCamera->getViewMatrix();
            camData.proj = m_pCamera->getProjectionMatrix();
            camData.proj[1][1] *= -1;
            camData.iTime += time;
            PushShaderData(camData);
        }


        void setupRenderPass() {

            using namespace engine;
            //Load the shader that draws the board
            auto boardVertexCode = utils::readFile("../../../assets/shaders/vertex.spv");
            auto boardFragmentCode = utils::readFile("../../../assets/shaders/fragment.spv");

            ShaderInfo shaderInfo = {
                    .stages = {
                            {.entryPoint = "main", .shaderCode = boardVertexCode, .stage = VERTEX},
                            {.entryPoint = "main", .shaderCode = boardFragmentCode, .stage = FRAGMENT}},
                    .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};

            std::vector<VertexDescriptorInfo> vertexInfo = {{XYZ_FLOAT,  offsetof(Vertex, position)},
                                                            {XY_FLOAT,   offsetof(Vertex, texCoords)},
                                                            {XYZ_FLOAT,  offsetof(Vertex, normal)},
                                                            {XYZW_FLOAT, offsetof(Vertex, color)},
                                                            {XYZ_FLOAT,  offsetof(Vertex, tangent)},
                                                            {XYZ_FLOAT,  offsetof(Vertex, bitangent)}};
            PipelineLayout layout = {
                    .shaderInfo = shaderInfo,
                    .vertexLayout = vertexInfo,
                    .cullMode = CullMode::BACK,
                    .windingMode = WindingMode::CLOCKWISE
            };

            //Configure the default render pass object
            RenderPassInfo renderPassInfo =
                    RenderPassFactory()
                            .numDescriptors(6)
                            .size(sizeof(Vertex))
                            .depthAttachment(true)
                            .subpasses({})
                            .attachments(
                                    {
                                            {
                                                    .format = COLOR_RGBA,
                                                    .isDepthAttachment = false,
                                                    .isSwapChainAttachment = true
                                            },
                                            {
                                                    .format = DEPTH_F32_STENCIL_8,
                                                    .isDepthAttachment = true
                                            }
                                    })
                            .pipelineLayout(layout)
                            .pushConstant(sizeof(PushConstant))
                            .bufferInfo({.size = sizeof(ShaderData), .offset = 0});

            m_pRenderPass = m_pContext.ResourceManager()->createDefaultRenderPass(renderPassInfo);

        }

    };

}
#endif //EPSILON_BSP_HPP
