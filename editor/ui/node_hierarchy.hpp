#pragma once

#include "ui_element.hpp"

namespace Editor::UI {

	enum class SceneNodeType : int {
		Root,
		Node,
		Render,
		Script,
		Transform
	};

	struct NodeProperties {

		bool valid() {
			return node_ref != nullptr;
		}

		size_t num_children() {
			return children.size();
		}

		void DestroyChildren() {
			for (auto& child : children) {
				child.DestroyChildren();
			}

			children.clear();
		}

		const char* type() {
			switch ((int)mType) {
			case (int)SceneNodeType::Render: return "Render";
			case (int)SceneNodeType::Transform: return "Transform";
			default: return "Node"; break;
			}
		}

		void* node_ref = nullptr;
		void* scene_node_ref = nullptr;
		std::string name{};
		std::string model_path{};

		std::vector<NodeProperties> children;

		SceneNodeType mType{};

		uint32_t index = 0;

		uint32_t Index() {
			return index;
		}

	};
	class SceneNodes : public UIElement {
		NodeProperties* selected_node = nullptr;
	public:

		void* scene_node_ref = nullptr;
		int selected_index = -1;

		SceneNodes() {
			m_pNodeProperties.push_back({ nullptr, nullptr, "Root", "", {}, SceneNodeType::Root });
		}

		void Push(NodeProperties node) {
			m_pNodeProperties.push_back(node);
		}

		void PushInRoot(NodeProperties node) {
			m_pNodeProperties.at(0).children.push_back(node);
		}

		void erase(uint32_t index) {
			std::vector<NodeProperties>::iterator selected_item;
			bool found = false;
			for (auto it = m_pNodeProperties.at(0).children.begin(); it < m_pNodeProperties.at(0).children.end(); it++) {
				if (it->Index() == index) {
					if (selected_node != nullptr) {
						selected_node = nullptr;

					}
					it->DestroyChildren();
					m_pNodeProperties.at(0).children.erase(it);
					break;
				}

			}
		}

		std::vector<NodeProperties> m_pNodeProperties;

		void draw() {
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			ImGui::Begin("Scene Nodes");
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0, 0.0, 0.0, 0.0));
			ImGui::Button(ICON_FA_PLUS, ImVec2(25, 25)); ImGui::SameLine();
			ImGui::Button(ICON_FA_MINUS, ImVec2(25, 25));
			ImGui::PopStyleColor();
			if (ImGui::TreeNode("Hierarchy"))
			{
				static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

				if (ImGui::BeginTable("3ways", 2, flags, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5)))
				{
					// The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
					ImGui::TableHeadersRow();

					if (m_pNodeProperties.size() > 0)
						DisplayNode(m_pNodeProperties[0]);

					for (auto node : m_pNodeProperties) {
						if (node.num_children() > 0) continue;
						DisplayNode(node);
					}

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}

			ImGui::SeparatorText("Properties");

			//ImGui::Begin("Inspector");
			if (selected_node != nullptr) {

				char* name_vec = selected_node->name.data();
				char* path_vec = selected_node->model_path.data();
				ImGui::InputText("Name", name_vec, selected_node->name.size(), ImGuiInputTextFlags_ReadOnly);


				if (selected_node->mType == SceneNodeType::Script) {

					EntityScript * script = reinterpret_cast<EntityScript*>(selected_node->node_ref);

					ImGui::InputText("Class Name", script->className.data(), script->className.size(), ImGuiInputTextFlags_ReadOnly);
					for (auto& prop : script->properties) {
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
							ImGui::Checkbox(prop.name.c_str(), &x);
							prop.value = x;
						}
						else if (prop.type == PropertyType::STRING) {
							std::string x = std::any_cast<std::string>(prop.value);
							ImGui::InputText(prop.name.c_str(), x.data(), x.size(), ImGuiInputTextFlags_ReadOnly);
							prop.value = x;
						}
						else {}
					}
					//ImGui::End();
				}

				else if (selected_node->mType == SceneNodeType::Render) {

					engine::RenderModel* renderModel = reinterpret_cast<engine::RenderModel*>(selected_node->node_ref);

					char* model_name_vec = renderModel->name.data();
					ImGui::InputText("Model Name", model_name_vec, renderModel->name.size(), ImGuiInputTextFlags_ReadOnly);
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
			//ImGui::End();
			ImGui::End();
		}

	private:
		void DisplayNode(NodeProperties& node)
		{
			ImGui::TableNextRow();
			ImGui::TableNextColumn();

			bool is_folder = (node.num_children() > 0);
			if (is_folder)
			{
				bool open = ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
				if (ImGui::IsItemClicked()) {
					selected_node = &node;
					std::cout << node.name << " clicked" << std::endl;
					scene_node_ref = node.scene_node_ref;
					selected_index = node.index;
				}
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(node.type());
				if (open)
				{
					for (int child_n = 0; child_n < node.num_children(); child_n++)
						DisplayNode(node.children[child_n]);
					ImGui::TreePop();
				}
			}
			else
			{
				ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
				if (ImGui::IsItemClicked()) {
					selected_node = &node;
					std::cout << node.name << " clicked" << std::endl;
					scene_node_ref = node.scene_node_ref;
					selected_index = node.index;
				}
				ImGui::TableNextColumn();
				ImGui::TextUnformatted(node.type());

			}
		}
	};
}