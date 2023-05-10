#pragma once

#include <unordered_map>
#include <string>

#include <core/engine/renderer/resource_manager.hpp>

namespace engine
{

    struct PBRMaterial
    {
        int32_t albedo_texture_index = -1;
        int32_t normal_texture_index = -1;
        int32_t metallic_texture_index = -1;
        int32_t roughnes_texture_index = -1;

        glm::vec4 albedo_color{};
        float metallic_color{};
        float roughness_color{};
    };

    struct RenderMesh
    {
        Ref<Mesh> mesh;
        Ref<Material> material;
        PBRMaterial material;
        glm::mat4 transform = glm::mat4(1.0f);
    };

    struct RenderModel
    {
        std::vector<RenderMesh> renderMeshes;
        int32_t layout_index = -1;
        glm::mat4 transform = glm::mat4(1.0f);
    };

    struct AudioObject {
        uint32_t source{};
        bool shouldPlay = false;
        float angle{};
        glm::vec3 position{};
        glm::vec3 direction{};
    };

    struct AudioBuffer {

    };

    struct

    class AssetManager
    {
        std::unordered_map<std::string, Ref<Texture>> m_pImages;
        std::unordered_map<std::string, RenderModel> m_pModels;
        std::unordered_map<std::string, int32_t> m_pAudioBuffers;

    public:
        const RenderModel &loadModel(const std::string &path)
        {
            if (m_pModels.contains(path))
                return m_pModels.at(path);

            framework::Model inModel("./assets/" + path);

            int index = 0;
            RenderModel model;
            for (auto &mesh : inModel.Meshes())
            {
                RenderMesh subRenderC;
                subRenderC.mesh = addMesh(name + "_submesh_" + std::to_string(index), mesh.data().mesh);
                //subRenderC.material = addMaterial(name + "_material", mesh.data().mesh);
                model.renderMeshes.push_back(subRenderC);
                index++;
            }

            m_pModels[path] = model;
        }

        const AudioObject& loadAudio(const std::string& path) {
            WAVfile audioFile;
            audioFile.Load("./assets/"+ path);

            int32_t buffer = al::createBuffer(audioFile.getNumberOfChannels(),
                audioFile.getFileSize(),
                audioFile.getBPS(),
                audioFile.getSampleRate(),
                audioFile.data().get());
        }
    private:
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