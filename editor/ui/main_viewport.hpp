#pragma once

#include <imgui.h>

#include <memory>

namespace Editor::UI {
	class MainViewport : public UIElement {
		std::shared_ptr<ImGuiRenderer::ImageInfo> m_pImageInfo{};
	public:
		MainViewport() = default;

		void draw() override {
			ImGui::Begin("Main Viewport");

			ImGui::Image(m_pImageInfo->id, ImGui::GetContentRegionAvail());

			ImGui::End();
		}

		void setImage(std::shared_ptr<ImGuiRenderer::ImageInfo> imageInfo) {
			m_pImageInfo = imageInfo;
		}
	};
}