#pragma once
#include "core/engine/renderer/renderer.hpp"
#include "core/engine/scene/asset_manager.hpp"
#include "core/engine/scene/nodes/node.hpp"
#include "core/engine/scene/parsers/renderpass.hpp"

namespace engine {
class RenderGraph {
  using Context_ptr = std::shared_ptr<Context>;
  using AssetManager_ptr = std::shared_ptr<AssetManager>;
  uint32_t meshCount = 0;
  Context_ptr context = nullptr;
  AssetManager_ptr assetManager = nullptr;
  Ref<RenderPass> m_pCurrentRenderPass;

  struct RenderArgs {
    std::shared_ptr<Node<RenderModel>> renderModel;
    glm::mat4 transform = glm::mat4(1.0f);
    parsers::RenderLayout layout{};
    int submeshCount = -1; //-1 to render all submeshes, >1 to render selected submeshes
    std::vector<int> subMeshes{};
    std::string material{};
    unsigned int count = 1;
    std::span<glm::mat4> transforms;
  };

  enum RenderStage {
    DEPTH_PREPASS = 0,
    SHADOW_PASS,
    OPAQUE_PASS,
    COPY_PASS,
    TRANSPARENT_PASS,
    COMPUTE_PASS
  };

  struct RenderGraphNode {
    std::string name;
    RenderStage stage;
  };

  void SetRenderPass(engine::Ref<engine::RenderPass> renderPass);

  uint32_t Push(const RenderArgs &renderArgs) {

    uint32_t push_index = meshCount;

    if (context->Window().getSize().width <= 0)
      return push_index;

    auto renderer = context->Renderer();
    auto &renderLayout = renderArgs.layout;
    Ref<BindGroup> selectedBindGroup = renderLayout.bindGroup;
    const auto transform_buffer = assetManager->getTransformBuffer();
    const auto object_buffer = assetManager->getObjectBuffer();
    const int submeshCount = renderArgs.submeshCount >= 1 ? renderArgs.submeshCount : renderArgs.renderModel->data.renderMeshes[0].size();
    const auto &meshCollection = renderArgs.renderModel->data.renderMeshes[0];

    for (int i = 0; i < submeshCount; i++) {
      constexpr Ref<PushConstant> push_constant;
      const RenderMesh *mesh = nullptr;
      mesh = &meshCollection[i];
      if (renderArgs.submeshCount >= 1)
        mesh = &meshCollection[renderArgs.subMeshes[i]];

      uint32_t material_indices[4] = {0};
      const uint32_t firstInstance = meshCount;
      transform_buffer[meshCount] = renderArgs.transform;
      for (int j = 0; j < renderArgs.count; j++) {

        if (renderer->numPushedCommands() >= MAX_COMMAND_QUEUE_SIZE)
          Flush();

        for (int k = 0; k < mesh->numMaterials; k++) {
          uint32_t uniform_index = assetManager->m_pMaterials.at(mesh->material_keys[k]).index;
          material_indices[k] = uniform_index;
        }

        if (renderArgs.count > 1)
          transform_buffer[meshCount] = renderArgs.transforms[j];

        object_buffer[meshCount] = {.object_id = static_cast<unsigned int>(renderArgs.renderModel->Parent()->Index()) + j,
                                       .transform_index = meshCount,
                                       .material_index = {material_indices[0], material_indices[1], material_indices[2], material_indices[3]},
                                       .numMaterials = static_cast<uint32_t>(mesh->numMaterials),
                                       .animationIndex = renderArgs.renderModel->data.animationIndex};
        meshCount++;
      }
      renderer->Push({.mesh = mesh->mesh,
                      .material = selectedBindGroup,
                      .pushConstant = push_constant,
                      .objectConstant = {.transform = renderArgs.transform,
                                         .material_index = material_indices[0],
                                         .animation_offset = renderArgs.renderModel->data.animationIndex},
                      .layout_index = renderLayout.pipelineLayoutIndex,
                      .uniformIndex = firstInstance,
                      .count = renderArgs.count});
    }

    return push_index;
  }
  void Flush(const DrawType drawType = DrawType::INDEXED, const bool clean_attachments = true) const {

    if (context->Window().getSize().width > 0) {
      const auto renderer = context->Renderer();

      renderer->Flush({.renderPassRef = m_pCurrentRenderPass, .type = drawType, .frameBufferIndex = 0, .clean_attachments = clean_attachments});
    }
  }

  std::vector<> graph;

public:
  void Render(std::shared_ptr<Scene> scene) {
    for (auto &render_node: graph) {

    }
  }
};
}