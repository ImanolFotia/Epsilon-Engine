#pragma once

#include "ui_element.hpp"
#include "native/file_dialog.hpp"

#include <unordered_map>
#include <functional>
#include <string>

namespace Editor::UI {
	class MenuBar : public UIElement {

		std::unordered_map<std::string, std::function<void(void)>> m_pCallbacks;

		UI::NativeFileDialog m_pFileDialog;
	public:

		MenuBar() {
			m_pFileDialog.Setup(".");
		}

		void addCallback(std::string name, std::function<void(void)> func) {
			if (name.size() > 0) {
				m_pCallbacks[name] = func;
			}
		}

		void draw() override {
			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File")) {
				ImGui::MenuItem("New");
				if(ImGui::MenuItem("Open")) {
					m_pFileDialog.Show();
				}
				ImGui::MenuItem("Open recent");
				if (ImGui::MenuItem("Exit")) {
					if (m_pCallbacks.contains("OnExit")) {
						m_pCallbacks.at("OnExit")();
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View")) {

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Project")) {

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Build")) {

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help")) {
				ImGui::MenuItem("Website");
				ImGui::MenuItem("About");
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	};
}