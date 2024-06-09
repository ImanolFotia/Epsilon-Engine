#pragma once
#include <cstdint>
#include <glm/glm.hpp>

#include <core/engine/renderer/drawables/primitives/quad.hpp>
#include <core/engine/scene/asset_manager.hpp>
#include <core/engine/scene/scene.hpp>

#include "../utils/node_factory.hpp"

namespace Editor {
class GrassSystem {

  uint32_t m_GrassNodeIndex = 0;
  glm::ivec3 m_PrevCamCoords{};
  glm::ivec4 m_PrevBounds{};

public:
  std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> Init(std::shared_ptr<engine::Scene> scene,
                                                                 std::shared_ptr<engine::AssetManager> assetManager) {

    common::MeshMaterial defaultMaterial;
    defaultMaterial.roughness = 0.34;
    defaultMaterial.metallic = 0;
    defaultMaterial.specular = 0.5;
    defaultMaterial.color = glm::vec4(0.15, 0.6, 0.15, 1.0);
    defaultMaterial.normal_path = "textures/grass_normal.png";
    defaultMaterial.name = "grass_material";

    engine::Quad grass_lod[3] = {engine::Quad(4, glm::vec2(1.0 / 4.0, 1.0)), engine::Quad(3, glm::vec2(0.3f, 1.0)),
                                 engine::Quad(2, glm::vec2(0.5f, 1.0))};

    for (int lod = 0; lod < 3; lod++) {
      for (auto &vtx : grass_lod[lod].data().Vertices) {
        float tmp = vtx.position.y;
        vtx.position.y = -vtx.position.z;
        vtx.position.z = tmp;

        tmp = vtx.normal.y;
        vtx.normal.y = vtx.normal.z;
        vtx.normal.z = tmp;

        vtx.position.x = vtx.position.x * 2.0f;

        vtx.position.y += 0.5;
      }

      for (auto &vtx : grass_lod[lod].data().Vertices) {

        vtx.position.x *= 0.125f;
        vtx.position.x += 0.05f;
        float y = vtx.position.y;
        vtx.position.x *= 1.0 - (y * y * y * y);
        vtx.position.x *= 0.5f;
      }

      grass_lod[lod].generateTangentSpaceVectors();
    }

    auto GrassBlade_Lod0 = assetManager->createModelFromMesh("Grass Blade Lod0", grass_lod[0].data(), defaultMaterial);
    auto GrassBlade_Lod1 = assetManager->createModelFromMesh("Grass Blade Lod1", grass_lod[1].data(), defaultMaterial);
    auto GrassBlade_Lod2 = assetManager->createModelFromMesh("Grass Blade Lod2", grass_lod[2].data(), defaultMaterial);

    GrassBlade_Lod0.renderMeshes[1] = GrassBlade_Lod1.renderMeshes[0];
    GrassBlade_Lod0.renderMeshes[2] = GrassBlade_Lod2.renderMeshes[0];
    GrassBlade_Lod0.isInstanced = true;

    std::mt19937 e2(555);
    for (int i = 0; i < 256; i++) {
      for (int j = 0; j < 256; j++) {
        // pAddDefaultPlane(glm::vec3((float)(i * 0.15) - 15, 0.0f, (float)(j * 0.15) - 15));
        glm::vec3 position = glm::vec3((float)(i * 0.15) - 15, 0.0f, (float)(j * 0.15) - 15);
        std::random_device rd;

        std::uniform_real_distribution<float> dist(-0.2, 0.2);
        std::uniform_real_distribution<float> dist2(-glm::pi<float>(), glm::pi<float>());

        glm::mat4 transform_matrix =
            glm::translate(glm::mat4(1.0), position + glm::vec3(dist(e2), 0.0, dist(e2)) + glm::vec3(0.5, 0.0, 0.5));
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0), dist2(e2), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 model_matrix = transform_matrix * rotation_matrix;
        GrassBlade_Lod0.transforms.push_back(model_matrix);
      }
    }

    auto node = Utils::CreateNode(glm::mat4(1.0), scene);
    m_GrassNodeIndex = node->Index();

    return node;
  }

  void Update(glm::vec3 camPos, std::shared_ptr<engine::Scene> scene) {

    glm::ivec3 cam_coords = camPos;
    if (m_PrevCamCoords.x != cam_coords.x || m_PrevCamCoords.z != cam_coords.z) {
      auto grass = scene->getNode(m_GrassNodeIndex);
      auto grass_model_node = scene->getChild<engine::RenderModel>(grass);
      m_PrevCamCoords = cam_coords;
      const int num_tiles = 32;
      const int tile_size = 256 / 32;

      glm::ivec4 current_coords = glm::ivec4(-15 + cam_coords.x, -15 + cam_coords.z,
                                             (256.0f * 0.15) - 15 + cam_coords.x, (256.0f * 0.15) - 15 + cam_coords.z);

      float i = 0.0f;
      int j = 0;
      std::mt19937 e2(555);
      for (auto &transform : grass_model_node->data.transforms) {
        glm::vec3 position = glm::vec3((float)(glm::floor(i / 256.0f) * 0.15) - 15 + cam_coords.x, 0.0f,
                                       (float)((j % 256) * 0.15) - 15 + cam_coords.z);

        std::uniform_real_distribution<float> dist(-0.2, 0.2);
        std::uniform_real_distribution<float> dist2(-glm::pi<float>(), glm::pi<float>());

        glm::mat4 transform_matrix =
            glm::translate(glm::mat4(1.0), position + glm::vec3(dist(e2), 0.0, dist(e2)) + glm::vec3(0.5, 0.0, 0.5));
        glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0), dist2(e2), glm::vec3(0.0, 1.0, 0.0));
        glm::mat4 model_matrix = transform_matrix * rotation_matrix;
        transform = model_matrix;
        i += 1;
        j++;
      }
    }
  }
};
} // namespace Editor