#pragma once

#include "ui_element.hpp"
#include <nlohmann/json.hpp>

namespace Editor::UI {
	class UIObjectProperty : public UIElement {
	public:

		std::vector<Property> m_pProperties;
		std::string m_pScriptName;
		std::shared_ptr<engine::Scene> m_pScenePtr = nullptr;

		const std::vector<Property>& getProperties() {
			return m_pProperties;
		}

		void setProperties(const std::string& script_name, std::vector<Property> props) {
			m_pScriptName = script_name;
			m_pProperties = props;
		}

		void setProperties(const std::string& script_name, const std::string& props) {
			m_pScriptName = script_name;
			nlohmann::json data = nlohmann::json::parse(props);

			for (auto& [key, property] : data.items()) {
				std::cout << "key: " << key << " property: " << property << std::endl;

				Property prop;
				if (property["Value"].is_string()) {
					prop.name = key;
					prop.type = PropertyType::STRING;
					prop.value = std::string(property["Value"].get<std::string>());
				}
				else if (property["Value"].is_boolean()) {
					prop.name = key;
					prop.type = PropertyType::BOOL;
					prop.value = property["Value"].get<bool>();
				}

				else if (property["Value"].is_number()) {
					prop.name = key;
					prop.type = PropertyType::NUMBER;
					prop.value = property["Value"].get<float>();
				}

				for (auto& [name, attribute] : property["Attributes"].items()) {
					if (name == "RangeAttribute") {
						auto attr_ptr = std::make_shared<RangeAttribute>();//prop.attributes.emplace_back<RangeAttribute>();
						attr_ptr->min = attribute["Min"].get<float>();
						attr_ptr->max = attribute["Max"].get<float>();
						prop.attributes.push_back(attr_ptr);
					}
					std::cout << attribute << std::endl;
				}
				m_pProperties.push_back(prop);
			}
		}

		NodeProperties* selected_node = nullptr;

		void SetNode(NodeProperties* selected) {
			selected_node = selected;
		}

		void draw() override {
			ImGui::Begin("Properties");
			if (selected_node) {
				char* name_vec = selected_node->name.data();
				char* path_vec = selected_node->model_path.data();
				ImGui::InputText("Name", name_vec, selected_node->name.size(), ImGuiInputTextFlags_ReadOnly);

				ImGui::SeparatorText("Object Script Properties");
				ImGui::InputText("Class Name", m_pScriptName.data(), m_pScriptName.size(), ImGuiInputTextFlags_ReadOnly);

				for (auto& prop : m_pProperties) {
					if (prop.type == PropertyType::NUMBER) {
						bool hasRange = false;
						float min = 0, max = 0;
						float x = std::any_cast<float>(prop.value);
						for (auto& attribute : prop.attributes) {
							if (attribute->name == "RangeAttribute") {
								hasRange = true;
								min = std::static_pointer_cast<RangeAttribute>(attribute)->min;
								max = std::static_pointer_cast<RangeAttribute>(attribute)->max;
							}
						}
						if (hasRange) {
							ImGui::SliderFloat(prop.name.c_str(), &x, min, max);
						}
						else {
							ImGui::DragFloat(prop.name.c_str(), &x, 0.01f);
						}
						prop.value = x;
					}
					else if (prop.type == PropertyType::BOOL) {
						bool x = std::any_cast<bool>(prop.value);
						if(ImGui::Checkbox(prop.name.c_str(), &x))
						prop.value = x;
					}
					else if (prop.type == PropertyType::STRING) {
						std::string x = std::any_cast<std::string>(prop.value);
						ImGui::InputText(prop.name.c_str(), x.data(), x.size(), ImGuiInputTextFlags_ReadOnly);
						prop.value = x;
					}
					else {}
				}



				//ImGui::Begin("Inspector");
				if (selected_node != nullptr) {

					char* name_vec = selected_node->name.data();
					char* path_vec = selected_node->model_path.data();
					//Gui::InputText("Name", name_vec, selected_node->name.size(), ImGuiInputTextFlags_ReadOnly);

					if (selected_node->mType == SceneNodeType::Render) {
						ImGui::SeparatorText("Render Properties");

						engine::RenderModel* renderModel = reinterpret_cast<engine::RenderModel*>(selected_node->node_ref);

						char* model_name_vec = renderModel->name.data();
						ImGui::InputText("Model Name", model_name_vec, renderModel->name.size(), ImGuiInputTextFlags_ReadOnly);
						//if (ImGui::Button("Model", buttonsize)) {
						ModelPopUp();
						//}
						ImGui::Checkbox("Instanced", &renderModel->isInstanced);
						if (renderModel->isInstanced) {
							ImGui::BeginChild("Instancing");
							ImGui::Button("Add instance");
							ImGui::BulletText("Total instances: %d", renderModel->transforms.size());
							ImGui::EndChild();
						}
						ImGui::RadioButton("Is visible", renderModel->visible);
						bool ha = renderModel->hasAnimation;
						ImGui::RadioButton("Has Animation", ha);

						if (renderModel->hasAnimation) {


							ImGui::SeparatorText("Animations");
							if (renderModel == nullptr) return;

							auto& animations = renderModel->animations;
							auto& skeleton = renderModel->skeleton;
							int anim_index = 0;
							for (auto& animation : animations) {
								char* anim_name_vec = animation.name.data();
								if (ImGui::RadioButton(anim_name_vec, skeleton.activeAnimation == anim_index)) {
									if (skeleton.activeAnimation != anim_index) {
										skeleton.activeAnimation = anim_index;
									}
								}
								anim_index++;
							}

							ImGui::SeparatorText("Active Animation");
							if (ImGui::Button("Reset")) {
								animations[skeleton.activeAnimation].current_time = 0;
							}
							ImGui::SameLine();
							if (ImGui::Button(animations[skeleton.activeAnimation].isActive ? "Pause" : "Play")) {
								animations[skeleton.activeAnimation].isActive = !animations[skeleton.activeAnimation].isActive;
							}

							ImGui::SameLine();
							if (ImGui::Button("Stop")) {
								animations[skeleton.activeAnimation].current_time = 0;
								animations[skeleton.activeAnimation].isActive = false;
							}

							ImGui::SliderFloat("Current time", &animations[skeleton.activeAnimation].current_time, animations[skeleton.activeAnimation].start, animations[skeleton.activeAnimation].end, "%.3f", ImGuiInputTextFlags_ReadOnly);
							ImGui::InputFloat("Duration", &animations[skeleton.activeAnimation].duration, 0.0, 0.0, "%.3f", ImGuiInputTextFlags_ReadOnly);
							ImGui::InputFloat("Start", &animations[skeleton.activeAnimation].start, 0.0, 0.0, "%.3f", ImGuiInputTextFlags_ReadOnly);
							ImGui::InputFloat("End", &animations[skeleton.activeAnimation].end, 0.0, 0.0, "%.3f", ImGuiInputTextFlags_ReadOnly);


						}
					}
				}
			}
			ImGui::End();
		}

		void ModelPopUp() {

			engine::RenderModel* renderModel = reinterpret_cast<engine::RenderModel*>(selected_node->node_ref);
			ImVec2 buttonsize = ImVec2(ImGui::GetContentRegionAvail().x, 0);
			if (ImGui::Button("Model", buttonsize))
				ImGui::OpenPopup("Model Popup");

			static std::string selected = renderModel->name;
			if (ImGui::BeginPopupModal("Model Popup", NULL)) {
				if (m_pScenePtr != nullptr) {
					auto& models = m_pScenePtr->getAssetManager()->getModels();

					if (ImGui::BeginListBox("##models_listbox", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
					{
						for (auto& model : models) {

							const bool is_selected = (selected == model.first);
							if (ImGui::Selectable(model.first.c_str(), is_selected)) {
								selected = model.first;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndListBox();
					}
				}

				if (ImGui::Button("Accept")) {

					auto parent_node = m_pScenePtr->getNode(selected_node->Index());
					auto child = m_pScenePtr->getChild<engine::RenderModel>(parent_node);
					auto bindGroup = renderModel->bindGroup;
					child->data = m_pScenePtr->getAssetManager()->getModel(selected);
					child->data.bindGroup = bindGroup;
					child->data.bindGroupId = std::hash<std::string>{}(bindGroup);
					ImGui::CloseCurrentPopup();
				} ImGui::SameLine();
				if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		}
	};
}