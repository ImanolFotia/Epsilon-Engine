#pragma once

#include "ui_element.hpp"
#include "native/file_dialog.hpp"
#include "../types/asset_type.hpp"
#include "../utils/path_utils.hpp"

#include <unordered_map>
#include <algorithm>
#include <string>

namespace Editor::UI {
	class Assets : public UIElement {


		engine::AssetManager* m_pAssetManagerRef;
		std::shared_ptr<engine::Renderer> m_pRendererRef;
		std::shared_ptr<engine::ResourceManager> m_pResourceManagerRef;

		AssetProperties selected_asset;
		UI::NativeFileDialog m_pFileDialog;

		std::shared_ptr<engine::Scene> m_pScene = nullptr;
	public:
		Assets() = default;

		Assets(std::shared_ptr<engine::Scene> scene) : m_pScene{ scene } {
			m_pFileDialog.Setup(".");
		}

		void draw() override {

			auto assetManager = m_pScene->getAssetManager();
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			ImGui::Begin("Assets");
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0, 0.0, 0.0, 0.0));
			if (ImGui::Button(ICON_FA_PLUS, ImVec2(25, 25))) {

				std::thread th([this, assetManager]() {
					m_pFileDialog.Show();
					auto result = m_pFileDialog.GetLastPath();
					if (result.size() > 0) {
						auto ext = getExtension(result.back());
						if (ext == "gltf" || ext == "glb")
							assetManager->loadModel(result.back());
					}
					});
				th.detach();
				//addEntityCallback();
				//selected_node = nullptr;
			}
			ImGui::PopStyleColor();
			if (ImGui::TreeNode("Assets"))
			{
				static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

				if (ImGui::BeginTable("3ways", 1, flags, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5)))
				{
					// The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
					ImGui::TableHeadersRow();


					for (auto& [name, model] : assetManager->getModels()) {
						DisplayNode({ .name = name, .mType = AssetType::Model, .index = 0 });
					}

					for (auto& [name, audio] : assetManager->getAudios()) {
						DisplayNode({ .name = name, .mType = AssetType::Audio, .index = 0 });
					}

					for (auto& [name, audio] : assetManager->getShaders()) {
						DisplayNode({ .name = name, .mType = AssetType::Shader, .index = 0 });
					}

					for (auto [hash, texture] : assetManager->getImages()) {
						DisplayNode({ .name = assetManager->getImageName(hash), .mType = AssetType::Texture, .index = texture.Index(), });
					}

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}

			ImGui::SeparatorText("Properties");

			ImGui::BeginChild("Properties", ImVec2(0, 0), true);

			if (selected_asset.mType == AssetType::Model) {

				auto& model = assetManager->getModel(selected_asset.name);

				ImGui::Text("Name:"); ImGui::SameLine();
				ImGui::InputText("##model_name_hidden_input", selected_asset.name.data(), selected_asset.name.size(), ImGuiInputTextFlags_ReadOnly);

				ImGui::Text("Submeshes:"); ImGui::SameLine();
				ImGui::InputText("##model_submeshes_hidden_input", std::to_string(model.renderMeshes.size()).data(), std::to_string(model.renderMeshes.size()).size(), ImGuiInputTextFlags_ReadOnly);

				ImGui::Text("Animated:");  ImGui::SameLine();
				ImGui::InputText("##model_animated_hidden_input", std::string(model.hasAnimation ? "Yes" : "No").data(), model.hasAnimation ? 4 : 3, ImGuiInputTextFlags_ReadOnly);

				if (model.hasAnimation) {
					ImGui::Text("Animations: %d", model.animations.size());
				}

			}
			if (selected_asset.mType == AssetType::Audio) {

			}
			if (selected_asset.mType == AssetType::Texture) {

				ImVec2 size = ImGui::GetContentRegionAvail();
				float margin = 60;
				std::shared_ptr<ImGuiRenderer::ImageInfo> currentImage = nullptr;
				currentImage = m_pScene->getContext()->Renderer()->getDebugRenderer()->getImages().at(selected_asset.name);
				if (currentImage != nullptr) {
					glm::ivec2 image_size = currentImage->size;

					ImGui::Text("Name: %s", currentImage->name.c_str());
					ImGui::Text("Dimensions: %dx%d", currentImage->size.x, currentImage->size.y);

					float widget_size = glm::min(size.y, size.x);
					ImGui::Image(currentImage->id, ImVec2((((float)widget_size * ((float)image_size.x / (float)image_size.y))), (widget_size)), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0));
				}
			}

			if (selected_asset.mType == AssetType::Shader) {
				auto shader = assetManager->getShader(selected_asset.name);
				ImGui::Text("Name: %s", shader.name.c_str());
				ImGui::Text("Source:");
				
				if (ImGui::Button(ICON_FA_PENCIL_ALT"##fragment_btn")) {
					std::string tmpStr = shader.filePaths[0];
					std::replace(tmpStr.begin(), tmpStr.end(), '/', '\\');
					auto result = ShellExecute(NULL, "open", tmpStr.c_str(), NULL, NULL, SW_SHOW);
				} ImGui::SameLine();
				ImGui::TextWrapped("Fragment: %s", shader.filePaths[0].c_str());
				
				if (ImGui::Button(ICON_FA_PENCIL_ALT"##vertex_btn")) {
					std::string tmpStr = shader.filePaths[1];
					std::replace(tmpStr.begin(), tmpStr.end(), '/', '\\');
					auto result = ShellExecute(NULL, "open", tmpStr.c_str(), NULL, NULL, SW_SHOW);
				} ImGui::SameLine();
				ImGui::TextWrapped("Vertex: %s", shader.filePaths[1].c_str());
				
				ImGui::Text("Binaries:"); 
				
				if (ImGui::Button(ICON_FA_PENCIL_ALT"##fragment_spirv_btn")) {
					std::string tmpStr = shader.spirvFilePaths[0];
					std::replace(tmpStr.begin(), tmpStr.end(), '/', '\\');
					auto result = ShellExecute(NULL, "open", tmpStr.c_str(), NULL, NULL, SW_SHOW);
				} ImGui::SameLine();
				ImGui::TextWrapped("Fragment: %s", shader.spirvFilePaths[0].c_str()); 
				
				if (ImGui::Button(ICON_FA_PENCIL_ALT"##vertex_spirv_btn")) {
					std::string tmpStr = shader.spirvFilePaths[1];
					std::replace(tmpStr.begin(), tmpStr.end(), '/', '\\');
					auto result = ShellExecute(NULL, "open", tmpStr.c_str(), NULL, NULL, SW_SHOW);
				}  ImGui::SameLine();
				ImGui::TextWrapped("Vertex: %s", shader.spirvFilePaths[1].c_str());

			}

			ImGui::EndChild();

			ImGui::End();
		}

	private:
		void DisplayNode(AssetProperties asset)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			//ICON_FA_FILE_CODE script
			//ICON_FA_FILE_AUDIO audio
			//ICON_FA_IMAGES
			//ICON_FA_CUBE

			const char* icon = ICON_FA_CUBE;

			if (asset.mType == AssetType::Audio) icon = ICON_FA_FILE_AUDIO;
			if (asset.mType == AssetType::Script) icon = ICON_FA_FILE_CODE;
			if (asset.mType == AssetType::Texture) icon = ICON_FA_IMAGES;
			if (asset.mType == AssetType::Shader) icon = ICON_FA_GLOBE_EUROPE;
			{

				ImGui::TreeNodeEx(std::string(icon + std::string(" ") + asset.name).c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
				if (ImGui::IsItemClicked()) {
					selected_asset = asset;
				}

			}
		}
	};
}