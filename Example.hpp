#pragma once
#define GLM_FORCE_RADIANS

#include "Epsilon.hpp"
#include "engine/renderer/drawables/primitives/quad.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <engine/renderer/drawables/primitives/cube.hpp>
#include <engine/renderer/drawables/primitives/sphere.hpp>

namespace ExampleApp {
    class ExampleApp : public Epsilon::Epsilon {
        struct Model {
            engine::Ref<engine::Mesh> mesh;
            engine::Ref<engine::Texture> texture;
            engine::Ref<engine::UniformBindings> bindings;
            engine::Ref<engine::Material> material;
            engine::Ref<engine::PushConstant> pushConstantRef;
            MeshPushConstant pushConstant;
        };

        engine::Ref<engine::RenderPass> m_pRenderPass;

    public:
        explicit ExampleApp(const std::string &appName) : Epsilon::Epsilon(appName) {}

        void onReady() override {}

        void onCreate() override {

            using namespace engine;
            auto vertexCode = utils::readFile("../assets/shaders/vertex.spv");
            auto fragmentCode = utils::readFile("../assets/shaders/fragment.spv");


            ShaderInfo shaderInfo = {
                    .stages = {
                            {.entryPoint = "main", .shaderCode = vertexCode, .stage = VERTEX},
                            {.entryPoint = "main", .shaderCode = fragmentCode, .stage = FRAGMENT}},
                    .usedStages = ShaderModuleStage(VERTEX | FRAGMENT)};


            RenderPassInfo renderPassInfo =
                    RenderPassFactory()
                            .numDescriptors(6)
                            .size(sizeof(Vertex))
                            .depthAttachment(true)
                            .subpasses({})
                            .vertexLayout({{XYZ_FLOAT,  offsetof(Vertex, position)},
                                           {XY_FLOAT,   offsetof(Vertex, texCoords)},
                                           {XYZ_FLOAT,  offsetof(Vertex, normal)},
                                           {XYZW_FLOAT, offsetof(Vertex, color)},
                                           {XYZ_FLOAT,  offsetof(Vertex, tangent)},
                                           {XYZ_FLOAT,  offsetof(Vertex, bitangent)}})
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
                            .pipelineLayout( {.shaderInfo = shaderInfo})
                            .pushConstant(sizeof(MeshPushConstant))
                            .bufferInfo({.size = sizeof(ShaderData), .offset = 0});

                            

            m_pRenderPass = m_pContext.ResourceManager()->createDefaultRenderPass(renderPassInfo);

            try {

                using namespace engine;
                auto quad_data = m_pQuad.data();
                myObjectId.mesh = m_pContext.ResourceManager()->createMesh({.vertices = quad_data.Vertices,
                                                                                   .indices = quad_data.Indices});

                auto cube_data = m_pCube.data();
                CubeId.mesh = m_pContext.ResourceManager()->createMesh({.vertices = cube_data.Vertices,
                                                                               .indices = cube_data.Indices});


                CubeId.pushConstant.model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75, 0.0, 0.5));
                CubeId.pushConstant.model = glm::rotate(CubeId.pushConstant.model, 1.0f, glm::vec3(0.2f, -1.0f, 0.5f));
                CubeId.pushConstant.model = glm::scale(CubeId.pushConstant.model, glm::vec3(0.25));

                CubeId.pushConstantRef = m_pContext.ResourceManager()->createPushConstant(
                        {.size = sizeof(MeshPushConstant), .data = &CubeId.pushConstant});

                m_pSphere = engine::Sphere(20);
                auto sphere_data = m_pSphere.data();

                SphereId.mesh = m_pContext.ResourceManager()->createMesh({.vertices = sphere_data.Vertices,
                                                                                 .indices = sphere_data.Indices});

                SphereId.pushConstantRef = m_pContext.ResourceManager()->createPushConstant(
                        {.size = sizeof(MeshPushConstant), .data = &SphereId.pushConstant});

                {
                    MaterialInfo material;
                    material.bindingInfo.offset = 0;
                    material.bindingInfo.size = sizeof(ShaderData);
                    int w, h, nc;
                    unsigned char *pixels = framework::load_image_from_file("../assets/images/texture.png",
                                                                            &w,
                                                                            &h,
                                                                            &nc);
                    engine::TextureInfo texInfo = engine::TextureBuilder()
                            .width(w)
                            .height(h)
                            .numChannels(nc)
                            .pixels(pixels);

                    material.textures.push_back(texInfo);

                    //uniformBuffer0 =

                    SphereId.material = m_pContext.ResourceManager()->createMaterial(material, m_pRenderPass);

                    framework::free_image_data(pixels);

                    // material = m_pRenderer->CreateMaterial(texture);
                }


                {
                    MaterialInfo material;
                    material.bindingInfo.offset = 0;
                    material.bindingInfo.size = sizeof(ShaderData);
                    int w, h, nc;
                    unsigned char *pixels = framework::load_image_from_file("../assets/images/pieces.png",
                                                                            &w,
                                                                            &h,
                                                                            &nc);
                    engine::TextureInfo texInfo = engine::TextureBuilder()
                            .width(w)
                            .height(h)
                            .numChannels(nc)
                            .pixels(pixels);

                    material.textures.push_back(texInfo);

                    //uniformBuffer0 =

                    CubeId.material = m_pContext.ResourceManager()->createMaterial(material, m_pRenderPass);

                    framework::free_image_data(pixels);

                    // material = m_pRenderer->CreateMaterial(texture);
                }

                myObjectId.pushConstantRef = m_pContext.ResourceManager()->createPushConstant(
                        {.size = sizeof(MeshPushConstant), .data = &myObjectId.pushConstant});
                {

                    MaterialInfo material;
                    material.bindingInfo.offset = 0;
                    material.bindingInfo.size = sizeof(ShaderData);
                    int w, h, nc;
                    unsigned char *pixels = framework::load_image_from_file("../assets/images/pieces.png", &w, &h,
                                                                            &nc);

                    engine::TextureInfo texInfo = engine::TextureBuilder()
                            .width(w)
                            .height(h)
                            .numChannels(nc)
                            .pixels(pixels);

                    material.textures.push_back(texInfo);

                    //uniformBuffer0 =

                    myObjectId.material = m_pContext.ResourceManager()->createMaterial(material, m_pRenderPass);
                    // auto texture = m_pRenderer->RegisterTexture(pixels, texInfo);

                    framework::free_image_data(pixels);
                    // material2 = m_pRenderer->CreateMaterial(texture);
                }
            }
            catch (std::exception &e) {
                std::cout << "\033[1;31mEXCEPTION ON: " << __PRETTY_FUNCTION__ << "\033[0m\n"
                          << e.what() << std::endl;
            }
        }

        void onRender() override {
            ShaderData camData;
            camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            camData.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f),
                                       glm::vec3(0.0f, 0.0f, 0.0f),
                                       glm::vec3(1.0f, 0.0f, 0.0f));
            /*
            camData.proj = glm::perspective(glm::radians(45.0f), getWindowDimensions().first /
                                                                 ((float) getWindowDimensions().second +
                                                                  std::numeric_limits<float>::epsilon()), 0.1f, 100.0f);
*/
            camData.proj = glm::ortho(-(getWindowDimensions().first / 2.0f),
                                      (getWindowDimensions().first / 2.0f),
                                      -((getWindowDimensions().second / 2.0f)),
                                      (getWindowDimensions().second / 2.0f), 0.1f, 1000.0f);
            camData.proj[1][1] *= -1;
            camData.iTime += time;

            PushShaderData(camData);

            // engine::DrawCommand drawCommand;

            // drawCommand = CubeId.mesh;
            //  engine::RenderObject renderObject;
            //  renderObject.objectId = CubeId;
            //   renderObject.materialId = material.id;
            //  renderObject.uniformData = camData;
            //  Draw(renderObject);

            myObjectId.pushConstant.model = glm::mat4(1.0f);
            myObjectId.pushConstant.model = glm::translate(myObjectId.pushConstant.model, glm::vec3(0.0, 0.0, -500.0));
            // myObjectId.pushConstant.model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(.0f, .0f, 1.0f));
            myObjectId.pushConstant.model = glm::scale(myObjectId.pushConstant.model, glm::vec3( getWindowDimensions().second/2, getWindowDimensions().second/2,0.0));

            engine::ObjectData objectData;
            objectData;
            objectData.layout_index = 0;
            objectData.mesh = myObjectId.mesh;
            objectData.material = myObjectId.material;
            objectData.modelMatrix = myObjectId.pushConstant.model;
            objectData.position = glm::vec3(-0.75, 0.0, 0.5);
            objectData.scale = glm::vec3(0.25);
            objectData.rotation = glm::rotate(myObjectId.pushConstant.model, (float) glfwGetTime(),
                                              glm::vec3(0.2f, -1.0f, 0.5f));
            objectData.pushConstant = myObjectId.pushConstantRef;
            m_pContext.Renderer()->Push(objectData);


            SphereId.pushConstant.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -150));
            SphereId.pushConstant.model = glm::rotate(SphereId.pushConstant.model, (float) glfwGetTime(),
                                                      glm::vec3(-0.2f, 1.0f, 0.5f));
            SphereId.pushConstant.model = glm::scale(SphereId.pushConstant.model, glm::vec3(100));

            objectData;
            objectData.layout_index = 0;
            objectData.mesh = SphereId.mesh;
            objectData.material = SphereId.material;
            objectData.modelMatrix = SphereId.pushConstant.model;
            objectData.position = glm::vec3(-0.75, 0.0, 0.5);
            objectData.scale = glm::vec3(0.25);
            objectData.rotation = glm::rotate(SphereId.pushConstant.model, (float) glfwGetTime(),
                                              glm::vec3(0.2f, -1.0f, 0.5f));
            objectData.pushConstant = SphereId.pushConstantRef;
            m_pContext.Renderer()->Push(objectData);
            // renderObject.objectId = SphereId;
            //  renderObject.materialId = material.id;
            // renderObject.uniformData = camData;
            // Draw(renderObject);


            CubeId.pushConstant.model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75, 0.0, 0.5));
            CubeId.pushConstant.model = glm::rotate(CubeId.pushConstant.model, (float) glfwGetTime(),
                                                    glm::vec3(0.2f, -1.0f, 0.5f));
            CubeId.pushConstant.model = glm::scale(CubeId.pushConstant.model, glm::vec3(0.25));
            //CubeId.pushConstantRef;
            objectData.mesh = CubeId.mesh;
            objectData.material = CubeId.material;
            objectData.modelMatrix = CubeId.pushConstant.model;
            objectData.position = glm::vec3(-0.75, 0.0, 0.5);
            objectData.scale = glm::vec3(0.25);
            objectData.rotation = glm::rotate(CubeId.pushConstant.model, (float) glfwGetTime(),
                                              glm::vec3(0.2f, -1.0f, 0.5f));
            objectData.pushConstant = CubeId.pushConstantRef;
            objectData.layout_index = 0;
            m_pContext.Renderer()->Push(objectData);

            drawFrame(m_pRenderPass);
        }

        void onExit() override {
        }

    private:
        Model myObjectId = {};
        Model CubeId = {};
        Model SphereId = {};

        engine::Material material = {};
        engine::Material material2 = {};

        engine::Cube m_pCube = {};
        engine::Sphere m_pSphere = {};
        engine::Quad m_pQuad = {};
    };
}