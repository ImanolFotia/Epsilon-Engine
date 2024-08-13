#pragma once

#include <imgui.h>

#include <memory>

namespace Editor::UI {
	class MaterialEditor : public UIElement {

	public:
		std::shared_ptr<engine::Scene> m_pSceneRef = nullptr;
		int32_t selected_entity = -1;
		void draw() override {
			ImGui::Begin("Material");
			if (selected_entity > 0 && m_pSceneRef != nullptr) {
				auto context = m_pSceneRef->getContext();
				auto assetManager = m_pSceneRef->getAssetManager();
				std::shared_ptr<engine::NodeBase> entity = m_pSceneRef->getNode(selected_entity);
				auto children = m_pSceneRef->getChildren(entity);

				bool containsRenderModel = false;
				for (auto& child : children) {

					if (child.first == typeid(engine::RenderModel)) {
						containsRenderModel = true;
						break;
					}
				}

				if (containsRenderModel) {


					auto r = m_pSceneRef->getChild<engine::RenderModel>(entity);
					auto render_node = std::static_pointer_cast<engine::Node<engine::RenderModel>>(r);

					int max_mesh_id = render_node->data.renderMeshes[0].size();
					static int current_mesh = 0;
					current_mesh = glm::min(max_mesh_id - 1, current_mesh);
					ImGui::SliderInt("Mesh", &current_mesh, 0, max_mesh_id - 1);
					if (render_node->data.renderMeshes[0][current_mesh].numMaterials > 0) {
						auto mat_key = render_node->data.renderMeshes[0][current_mesh].material_keys[0];
						bool wasModified = false;
						auto& material = assetManager->getMaterial(mat_key);
						{
							bool isColorTexture = material.material.albedo_texture_index > 0;
							ImGui::SeparatorText("Albedo");
							if (ImGui::Checkbox("Use texture##material_color_check", &isColorTexture)) {
								material.material.albedo_texture_index *= -1; wasModified = true;
							}
							if (isColorTexture && material.material.albedo_texture_index > 0) {
								auto& albedo = assetManager->getImage(material.Slot[(int)engine::TextureSlot::Albedo]);
								if (albedo.isValid()) {
									auto imageId = context->Renderer()->getDebugRenderer()->getImageByIndex(albedo)->id;
									ImGui::Image(imageId, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x));
								}
							}
							else {
								if (ImGui::ColorPicker4("Albedo##albedo_picker", &material.material.albedo_color[0])) {
									wasModified = true;
								}

							}
						}
						{
							bool isRoughnessTexture = material.material.metallic_texture_index > 0;
							if (ImGui::Checkbox("Use texture##material_roughness_check", &isRoughnessTexture)) {
								material.material.metallic_texture_index *= -1; wasModified = true;
							}
							if (isRoughnessTexture && material.material.metallic_texture_index > 0) {
								auto& roughness = assetManager->getImage(material.Slot[(int)engine::TextureSlot::Metallic]);
								auto imageId = context->Renderer()->getDebugRenderer()->getImageByIndex(roughness)->id;
								ImGui::Image(imageId, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x));
							}
							else {
								if (ImGui::DragFloat("Roughness##roughness_picker", &material.material.roughness_color, 0.01, 0.0, 1.0)) {

									wasModified = true;
								}
							}
						}
						{
							bool isMetallicTexture = material.material.metallic_texture_index > 0;
							if (ImGui::Checkbox("Use texture##material_metallic_check", &isMetallicTexture)) {
								material.material.metallic_texture_index *= -1; wasModified = true;
							}
							if (isMetallicTexture && material.material.metallic_texture_index > 0) {
								auto& metallic = assetManager->getImage(material.Slot[(int)engine::TextureSlot::Metallic]);
								auto imageId = context->Renderer()->getDebugRenderer()->getImageByIndex(metallic)->id;
								ImGui::Image(imageId, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().x));
							}
							else {
								if (ImGui::DragFloat("Metallic##metallic_picker", &material.material.metallic_color, 0.01, 0.0, 1.0)) {
									wasModified = true;
								}
							}
						}
						{
							if (ImGui::DragFloat("Specular##specular_picker", &material.material.specular, 0.01, 0.0, 1.0)) {
								wasModified = true;
							}
						}

						if (wasModified) {
							assetManager->setMaterial(mat_key, material);
							assetManager->setMaterial(material);
						}
					}
				}
			}
			ImGui::End();

		}
	};
}