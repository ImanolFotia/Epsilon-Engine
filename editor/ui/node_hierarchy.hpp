#pragma once

#include "ui_element.hpp"

namespace Editor::UI {

	class SceneNodes : public UIElement {
	public:
		NodeProperties* selected_node = nullptr;

		void* scene_node_ref = nullptr;
		int selected_index = -1;

		std::function<void(void)> addEntityCallback;

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

		std::list<NodeProperties> m_pNodeProperties;

		void draw() {
			const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
			const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
			ImGui::Begin("Scene Nodes");
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0, 0.0, 0.0, 0.0));
			if (ImGui::Button(ICON_FA_PLUS, ImVec2(25, 25))) {
				addEntityCallback();
				selected_node = nullptr;
			} 
			ImGui::SameLine();
			ImGui::Button(ICON_FA_MINUS, ImVec2(25, 25));
			ImGui::PopStyleColor();
			if (ImGui::TreeNode("Hierarchy"))
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