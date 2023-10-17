#pragma once

#include "ui_element.hpp"
#include "../types/editor_node.hpp"

namespace Editor::UI {

	class SceneNodes : public UIElement {
	public:
		NodeProperties* selected_node = nullptr;

		void* scene_node_ref = nullptr;
		int selected_index = -1;

		std::function<void(void)> addEntityCallback;
		std::list<NodeProperties> m_pNodeProperties;

		engine::Scene* m_pScenePtr = nullptr;

		SceneNodes() {
			m_pNodeProperties.push_back({ nullptr, nullptr, "Root", "", {}, SceneNodeType::Root });
		}

		void Push(NodeProperties node) {
			m_pNodeProperties.push_back(node);
		}

		void PushInRoot(NodeProperties node) {
			m_pNodeProperties.front().children.push_back(node);
		}

		void erase(uint32_t index) {
			std::vector<NodeProperties>::iterator selected_item;
			bool found = false;
			for (auto it = m_pNodeProperties.front().children.begin(); it < m_pNodeProperties.front().children.end(); it++) {
				if (it->Index() == index) {
					if (selected_node != nullptr) {
						selected_node = nullptr;

					}
					it->DestroyChildren();
					m_pNodeProperties.front().children.erase(it);
					break;
				}

			}
		}


		void draw() {
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			ImGui::Begin("Scene Nodes");
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0, 0.0, 0.0, 0.0));
			if (ImGui::Button(ICON_FA_PLUS, ImVec2(25, 25))) {

				ImGui::OpenPopup("New Node");
				//addEntityCallback();
				selected_node = nullptr;
			}
			ModelPopUp();
			ImGui::SameLine();
			ImGui::Button(ICON_FA_MINUS, ImVec2(25, 25));
			ImGui::PopStyleColor();
			if (ImGui::TreeNode(ICON_FA_SITEMAP " Hierarchy"))
			{
				static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;

				if (ImGui::BeginTable("3ways", 2, flags, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y)))
				{
					// The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
					ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
					ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
					ImGui::TableHeadersRow();

					if (m_pNodeProperties.size() > 0)
						DisplayNode(m_pNodeProperties.front());

					for (auto node : m_pNodeProperties) {
						if (node.num_children() > 0) continue;
						DisplayNode(node);
					}

					ImGui::EndTable();
				}
				ImGui::TreePop();
			}
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
				bool open = ImGui::TreeNodeEx(node.name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick);
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

		void ModelPopUp() {


			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImVec2 size = ImGui::GetMainViewport()->Size;
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::SetNextWindowSize(ImVec2((size.x / 4) * 3, (size.y / 4) * 3));
			bool unused_open = true;
			if (ImGui::BeginPopupModal("New Node", &unused_open)) {
				ImGui::SeparatorText("Node Type");
				if (m_pScenePtr != nullptr) {
					auto& models = m_pScenePtr->getAssetManager().getModels();
					static int selected = 0;
					if (ImGui::BeginListBox("##nodes_listbox", ImVec2(ImGui::GetContentRegionAvail().x * 0.25, ImGui::GetContentRegionAvail().y-35)))
					{
						for (int i = 0; i < NodeType::Size; i++) {

							const bool is_selected = (selected == i);
							if (ImGui::Selectable(NodeType::NodeTypesNames[i], is_selected)) {
								selected = i;
							}
							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
						ImGui::EndListBox();
					}
					ImGui::SameLine();
					ImGui::BeginChild("Description", ImVec2(0, ImGui::GetContentRegionAvail().y - 35), true);
					ImGui::SeparatorText("Description");
					ImGui::TextWrapped("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");

					ImGui::BeginChild("Properties", ImVec2(0, 0), true);
					ImGui::SeparatorText("Properties");
					bool dummyBool = true;
					ImGui::Checkbox("Dummy check", &dummyBool);
					ImGui::Button("Dummy button");
					ImGui::EndChild();


					ImGui::EndChild();
				}

				if (ImGui::Button("Accept", ImVec2(0, 30))) {

					/*auto parent_node = m_pScenePtr->getNode(selected_node->Index());
					auto child = m_pScenePtr->getChild<engine::RenderModel>(parent_node);
					auto bindGroup = renderModel->bindGroup;
					child->data = m_pScenePtr->getAssetManager().getModel(selected);
					child->data.bindGroup = bindGroup;
					ImGui::CloseCurrentPopup();*/
				} ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(0, 30))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		}
	};
}