#pragma once

#include "asset_manager.hpp"
#include "nodes/node_manager.hpp"

#include "trees/octree.hpp"

namespace engine::scene {

	class Scene {
		AssetManager m_pAssetManager;
		//OctreeContainer<std::shared_ptr<NodeBase>> m_pOctree;
		SceneManager m_pSceneManager;

		std::unordered_map<std::string, RenderPassInfo> m_RenderPasses;


	public:
		Scene() {

			m_RenderPasses = engine::parsers::parse_renderpasses();
		}

		void BeginScene() {
			auto renderer = Context().Renderer();
			renderer->BeginFrame();
			renderer->Begin();
		}


		void Push(const RenderModel& renderModel, std::string layout) {

			auto renderer = Context().Renderer();

			Ref<PushConstant> push_constant;

			for (auto& mesh : renderModel.renderMeshes) {
				renderer->Push({ .mesh = mesh.mesh,
								.material = mesh.bind_group,
								.pushConstant = push_constant,
								.layout_index = 0,
								.uniformIndex = 0 });
			}
		}

		void EndScene() {

			auto renderer = Context().Renderer();
			glm::vec3 v;
			renderer->End(v);

			renderer->Submit();
			renderer->EndFrame();
		}
	};
}