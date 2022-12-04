#pragma once
#define GLM_FORCE_RADIANS

#include "LearningVulkanApplication.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <engine/renderer/drawables/primitives/cube.hpp>
#include <engine/renderer/drawables/primitives/sphere.hpp>

namespace ExampleApp
{
    class ExampleApp : public LearningVulkan::LearningVulkanApplication
    {
        struct Model
        {
            engine::Ref<engine::Mesh> mesh;
            engine::Ref<engine::Texture> texture;
            engine::Ref<engine::UniformBindings> bindings;
            engine::Ref<engine::Material> material;
            engine::MeshPushConstant pushConstant;
        };

        engine::Ref<engine::Buffer> uniformBuffer0;

    public:
        explicit ExampleApp(std::string appName) : LearningVulkan::LearningVulkanApplication(appName) {}

        void onCreate() override
        {
            try
            {
                const std::vector<engine::Vertex> vertices = {
                    {glm::vec3(-1.0f, -1.0f, 0.0f),
                     glm::vec2(0.0f, 0.0f),
                     glm::vec3(0.0f, 0.0f, 1.0f),
                     glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f)},
                    {glm::vec3(1.0f, -1.0f, 0.0f),
                     glm::vec2(1.0f, 0.0f),
                     glm::vec3(0.0f, .0f, 1.0f),
                     glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f)},
                    {glm::vec3(1.0f, 1.0f, 0.0f),
                     glm::vec2(1.0f, 1.0f),
                     glm::vec3(0.0f, 0.0f, 1.0f),
                     glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f)},
                    {glm::vec3(-1.0f, 1.0f, 0.0f),
                     glm::vec2(0.0f, 1.0f),
                     glm::vec3(0.0f, 0.0f, 1.0f),
                     glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f),
                     glm::vec3(0.0f, 0.0f, 0.0f)}};

                std::vector<uint32_t> indices = {
                    0, 1, 2, 2, 3, 0};

                using namespace engine;

                myObjectId.mesh = m_pContext.ResourceManager()->createMesh({.vertices = vertices,
                                                                            .indices = indices});

                auto cube_data = m_pCube.data();
                CubeId.mesh = m_pContext.ResourceManager()->createMesh({.vertices = cube_data.Vertices,
                                                                        .indices = cube_data.Indices});

                m_pSphere = engine::Sphere(20);
                auto sphere_data = m_pSphere.data();

                SphereId.mesh = m_pContext.ResourceManager()->createMesh({.vertices = sphere_data.Vertices,
                                                                          .indices = sphere_data.Indices});

                MaterialInfo material;
                {
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
                    
                    SphereId.material = m_pContext.ResourceManager()->createMaterial(material, renderPassRef);

                    framework::free_image_data(pixels);

                    // material = m_pRenderer->CreateMaterial(texture);
                }

                CubeId.material = SphereId.material;
                myObjectId.material = SphereId.material;

                {
                    int w, h, nc;
                    unsigned char *pixels = framework::load_image_from_file("../assets/images/texture2.png", &w, &h, &nc);

                    engine::TextureInfo texInfo = engine::TextureBuilder()
                                                      .width(w)
                                                      .height(h)
                                                      .numChannels(nc);

                    // auto texture = m_pRenderer->RegisterTexture(pixels, texInfo);

                    framework::free_image_data(pixels);
                    // material2 = m_pRenderer->CreateMaterial(texture);
                }
            }
            catch (std::exception &e)
            {
                std::cout << "\033[1;31mEXCEPTION ON: " << __PRETTY_FUNCTION__ << "\033[0m\n"
                          << e.what() << std::endl;
            }
        }

        void onRender() override
        {
            engine::ShaderData camData;
            camData.iResolution = glm::vec2(getWindowDimensions().first, getWindowDimensions().second);
            static auto startTime = std::chrono::high_resolution_clock::now();

            auto currentTime = std::chrono::high_resolution_clock::now();
            float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

            camData.view = glm::lookAt(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            camData.proj = glm::perspective(glm::radians(45.0f), getWindowDimensions().first / ((float)getWindowDimensions().second + std::numeric_limits<float>::epsilon()), 0.1f, 100.0f);
            camData.proj[1][1] *= -1;
            camData.iTime += time;

            PushCameraData(camData);

            CubeId.pushConstant.model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75, 0.0, 0.5));
            CubeId.pushConstant.model = glm::rotate(CubeId.pushConstant.model, (float)glfwGetTime(), glm::vec3(0.2f, -1.0f, 0.5f));
            CubeId.pushConstant.model = glm::scale(CubeId.pushConstant.model, glm::vec3(0.25));

            engine::ObjectData objectData;
            objectData.mesh = CubeId.mesh;
            objectData.material = CubeId.material;
            objectData.modelMatrix = CubeId.pushConstant.model;
            objectData.position = glm::vec3(-0.75, 0.0, 0.5);
            objectData.scale = glm::vec3(0.25);
            objectData.rotation = glm::rotate(CubeId.pushConstant.model, (float)glfwGetTime(), glm::vec3(0.2f, -1.0f, 0.5f));
            m_pContext.Renderer()->Push(objectData);
            // engine::DrawCommand drawCommand;

            // drawCommand = CubeId.mesh;
            //  engine::RenderObject renderObject;
            //  renderObject.objectId = CubeId;
            //   renderObject.materialId = material.id;
            //  renderObject.uniformData = camData;
            //  Draw(renderObject);

            myObjectId.pushConstant.model = glm::mat4(1.0f);
            // myObjectId->push_constant.model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(.0f, .0f, 1.0f));
            // myObjectId->push_constant.model = glm::scale(CubeId->push_constant.model, glm::vec3(0.5));

            // renderObject.objectId = myObjectId;
            // renderObject.materialId = material2.id;
            // renderObject.uniformData = camData;
            // Draw(renderObject);

            SphereId.pushConstant.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.75, 0.0, 0.5));
            SphereId.pushConstant.model = glm::rotate(SphereId.pushConstant.model, (float)glfwGetTime(), glm::vec3(-0.2f, 1.0f, 0.5f));
            SphereId.pushConstant.model = glm::scale(SphereId.pushConstant.model, glm::vec3(0.25));
            // renderObject.objectId = SphereId;
            //  renderObject.materialId = material.id;
            // renderObject.uniformData = camData;
            // Draw(renderObject);
        }

        void onExit() override
        {
        }

    private:
        Model myObjectId = {};
        Model CubeId = {};
        Model SphereId = {};

        engine::Material material = {};
        engine::Material material2 = {};

        engine::Cube m_pCube = {};
        engine::Sphere m_pSphere = {};
    };
}