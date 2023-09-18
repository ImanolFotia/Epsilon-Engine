#pragma once

#include <imgui.h>

#include <memory>

namespace Editor::UI {
	class MainViewport : public UIElement {
		std::shared_ptr<ImGuiRenderer::ImageInfo> m_pImageInfo{};
		glm::ivec2 m_pSize;
		glm::vec2 m_pPosition;
		glm::ivec2 m_pLastSize;

		bool m_pShouldResize = false;
		bool m_pFirstTime = true;
		bool m_pIsHovered = false;
		TransformationMode m_pTransformMode = TransformationMode::NONE;
	public:
		MainViewport() = default;

		TransformationMode getMode() {
			return m_pTransformMode;
		}

		void draw() override {
			ImGui::Begin("Main Viewport");
			auto windowSize = ImGui::GetWindowSize();
			windowSize.x = glm::min(ImGui::GetWindowSize().x, ImGui::GetContentRegionAvail().x);
			windowSize.y = glm::min(ImGui::GetWindowSize().y, ImGui::GetContentRegionAvail().y);

			m_pSize = glm::ivec2(windowSize.x, windowSize.y);

			auto cursor_pos = ImGui::GetCursorPos();


			ImGuiIO& io = ImGui::GetIO();
			static ImGuiWindowFlags gizmoWindowFlags = 0;

			ImGuizmo::SetOrthographic(false);
			ImGuizmo::BeginFrame();

			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(getPosition().x, getPosition().y, getSize().x, getSize().y);

			bool any_hovered = ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyMouseDown() || ImGui::IsAnyItemFocused();
			if (m_pFirstTime) {
				m_pLastSize = m_pSize;
				m_pFirstTime = false;
			}

			if (m_pLastSize != m_pSize) {
				m_pShouldResize = true;
			}

			ImGui::Image(m_pImageInfo->id, ImGui::GetContentRegionAvail());


			cursor_pos = ImVec2(cursor_pos.x + 20.0f, cursor_pos.y + 20.0f);
			ImGui::SetCursorPos(cursor_pos);

			bool colorSet = false;

			if (m_pTransformMode == TransformationMode::NONE) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 130 / 255.0, 150 / 255.0, 1.00f));
				colorSet = true;
			}	

			if (ImGui::Button(ICON_FA_MOUSE_POINTER, ImVec2(35, 35))) {
				m_pTransformMode = TransformationMode::NONE;
				std::cout << "selection\n";
			} 
			if (colorSet) {
				ImGui::PopStyleColor();
				colorSet = false;
			}
			ImGui::SameLine();

			if (m_pTransformMode == TransformationMode::TRANSLATE) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 130 / 255.0, 150 / 255.0, 1.00f));
				colorSet = true;
			}
			if (ImGui::Button(ICON_FA_ARROWS_ALT, ImVec2(35, 35))) {
				m_pTransformMode = TransformationMode::TRANSLATE;
				std::cout << "move\n";

			} 

			if (colorSet) {
				ImGui::PopStyleColor();
				colorSet = false;
			}

			ImGui::SameLine();

			if (m_pTransformMode == TransformationMode::ROTATE) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 130 / 255.0, 150 / 255.0, 1.00f));
				colorSet = true;
			}

			if (ImGui::Button(ICON_FA_SYNC, ImVec2(35, 35))) {
				m_pTransformMode = TransformationMode::ROTATE;
				std::cout << "rotate\n";

			} 

			if (colorSet) {
				ImGui::PopStyleColor();
				colorSet = false;
			}

			ImGui::SameLine();
			if (m_pTransformMode == TransformationMode::SCALE) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 130 / 255.0, 150 / 255.0, 1.00f));
				colorSet = true;
			}
			if (ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT, ImVec2(35, 35))) {
				m_pTransformMode = TransformationMode::SCALE;
				std::cout << "scale\n";

			}
			if (colorSet) {
				ImGui::PopStyleColor();
				colorSet = false;
			}
			ImGui::SameLine();

			if (m_pTransformMode == TransformationMode::BOUND) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 130 / 255.0, 150 / 255.0, 1.00f));
				colorSet = true;
			}
			if (ImGui::Button(ICON_FA_VECTOR_SQUARE, ImVec2(35, 35))) {
				m_pTransformMode = TransformationMode::BOUND;
				std::cout << "bounds\n";
			}
			if (colorSet) {
				ImGui::PopStyleColor();
				colorSet = false;
			}


			auto mousePos = ImGui::GetMousePos();
			auto windowPos = ImGui::GetWindowPos();
			m_pPosition = glm::vec2(windowPos.x + 7, windowPos.y + 29);
			if (mousePos.x > windowPos.x + 7 && mousePos.y > windowPos.y + 29 &&
				mousePos.x < windowPos.x + 7 + windowSize.x && mousePos.y < windowPos.y + 29 + windowSize.y) {
				if (!any_hovered)
					m_pIsHovered = true;
			}
			else {
				if (ImGui::IsMousePosValid())
					m_pIsHovered = false;
			}


			ImGui::End();


			m_pLastSize = m_pSize;
		}

		bool ShouldResize() {
			return m_pShouldResize;
		}

		void ResetFlags() {
			m_pShouldResize = false;
		}

		bool isHovered() {
			return m_pIsHovered;
		}


		glm::ivec2 getSize() {
			m_pSize = glm::clamp(m_pSize, glm::ivec2(1), glm::ivec2(16000));
			return m_pSize;
		}

		glm::vec2 getPosition() {
			return m_pPosition;
		}

		void setImage(std::shared_ptr<ImGuiRenderer::ImageInfo> imageInfo) {
			m_pImageInfo = imageInfo;
		}
	};
}