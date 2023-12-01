#pragma once

#include "ui_element.hpp"
#include "../types/editor_node.hpp"
#include "../types/script.hpp"
#include "editor/utils/node_factory.hpp"
#include <core/engine/renderer/drawables/primitives/cube.hpp>

namespace Editor::UI {

	class SceneNodes : public UIElement {
	public:
		NodeProperties* selected_node = nullptr;

		void* scene_node_ref = nullptr;
		int selected_index = -1;

		std::function<void(void)> addEntityCallback;
		std::vector<NodeProperties> m_pNodeProperties;

		std::shared_ptr<engine::Scene> m_pScenePtr = nullptr;

		SceneNodes() {
			m_pNodeProperties.push_back({ nullptr, nullptr, "Root", "", {}, SceneNodeType::Root });
		}

		void Push(NodeProperties node) {
			m_pNodeProperties.push_back(node);
		}

		void PushInRoot(NodeProperties node) {
			m_pNodeProperties.front().children.push_back(node);
		}

		void UpdateNodeProp(std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> node) {

			auto node_props = selected_node;//m_pNodeProperties.front().children.at(selected_index);
			node_props->children.clear();

			auto nodeChildren = m_pScenePtr->getChildren(node);

			for (auto& [type, children] : nodeChildren) {
				for (auto& child : children) {
					if (m_pScenePtr->isOfType<EntityScript>(child))
					{
						auto script = std::static_pointer_cast<engine::Node<EntityScript>>(child);
						UI::NodeProperties script_props;
						script_props.name = "Script";
						script_props.mType = UI::SceneNodeType::Script;
						script_props.node_ref = (void*)&script.get()->data;
						script_props.scene_node_ref = node;
						script_props.index = node->Index();
						node_props->children.push_back(script_props);

					}
					if (m_pScenePtr->isOfType<engine::RenderModel>(child))
					{
						auto renderNode = std::static_pointer_cast<engine::Node<engine::RenderModel>>(child);

						UI::NodeProperties render_props;
						render_props.name = "Render Model" + renderNode.get()->data.name;
						render_props.mType = UI::SceneNodeType::Render;
						render_props.node_ref = (void*)&renderNode.get()->data;
						render_props.scene_node_ref = node;
						render_props.index = node->Index();
						node_props->children.push_back(render_props);
					}
				}
			}

		}

		void RegisterIntoEditor(const std::string& name, std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent) {

			auto nodeChildren = m_pScenePtr->getChildren(parent);

			UI::NodeProperties node_props;
			node_props.name = name;
			node_props.mType = UI::SceneNodeType::Node;
			node_props.node_ref = (void*)&parent.get()->data;
			node_props.scene_node_ref = parent;
			node_props.index = parent->Index();

			for (auto& [type, children] : nodeChildren) {
				for (auto& child : children) {
					if (m_pScenePtr->isOfType<EntityScript>(child))
					{
						auto script = std::static_pointer_cast<engine::Node<EntityScript>>(child);
						UI::NodeProperties script_props;
						script_props.name = "Script";
						script_props.mType = UI::SceneNodeType::Script;
						script_props.node_ref = (void*)&script.get()->data;
						script_props.scene_node_ref = parent;
						script_props.index = parent->Index();
						node_props.children.push_back(script_props);

					}
					if (m_pScenePtr->isOfType<engine::RenderModel>(child))
					{
						auto renderNode = std::static_pointer_cast<engine::Node<engine::RenderModel>>(child);

						UI::NodeProperties render_props;
						render_props.name = "Render Model" + std::to_string(parent->Index());
						render_props.mType = UI::SceneNodeType::Render;
						render_props.node_ref = (void*)&renderNode.get()->data;
						render_props.scene_node_ref = parent;
						render_props.index = parent->Index();
						node_props.children.push_back(render_props);
					}
				}
			}
			PushInRoot(node_props);
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
					scene_node_ref = node.scene_node_ref.get();
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
					scene_node_ref = node.scene_node_ref.get();
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
				static int selected = 0;
				ImGui::SeparatorText("Node Type");
				if (m_pScenePtr != nullptr) {
					auto& models = m_pScenePtr->getAssetManager().getModels();
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
				static char name_buffer[128] = {};
				ImGui::InputText("Node Name", name_buffer, 128); ImGui::SameLine();
				if (ImGui::Button("Accept", ImVec2(0, 30))) {
					std::string node_name = std::string(name_buffer);
					if (node_name.size() <= 0) return;

					std::shared_ptr<engine::Node<engine::Scene::SceneEntity>> parent_node;


					if (selected_node != nullptr && selected_node->Index() != 0) {
						parent_node = selected_node->scene_node_ref;
					}
					else {

						parent_node = Utils::CreateNode(glm::mat4(1.0f), m_pScenePtr);
					}


					switch (selected) {
					case NodeType::RenderModel:

						common::MeshMaterial defaultMaterial;
						defaultMaterial.roughness = 0.8;
						defaultMaterial.metallic = 0;
						defaultMaterial.color = glm::vec4(0.5, 0.5, 0.5, 1.0);
						engine::Cube cube;
						auto m_pDefaultCube = m_pScenePtr->getAssetManager().createModelFromMesh("Model_" + node_name, cube.data(), defaultMaterial);
						Utils::AddModelNode(node_name, m_pScenePtr, m_pDefaultCube, parent_node);
						break;
					}

					if (selected_node == nullptr || selected_node->Index() == 0) {
						RegisterIntoEditor(node_name, parent_node);
					}
					else {
						UpdateNodeProp(parent_node);
					}

					selected_node = nullptr;

					std::memset(name_buffer, 0, 128);
					ImGui::CloseCurrentPopup();
				} ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(0, 30))) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}
		}
	};
}