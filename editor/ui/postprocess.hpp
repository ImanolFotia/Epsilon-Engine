#pragma once

#include <imgui.h>
#include <modules/imgui/custom_widgets.hpp>
#include "../renderpasses/taa.hpp"

#include <memory>

namespace Editor::UI {
	class PostProcess : public UIElement {

		bool m_TAAEnable = true;
		bool m_SSSEnable = true;

	public:

		Renderpasses::TAAUniformData m_TAAData;
		Renderpasses::SSSUniformData m_SSSData;

		bool TaaEnabled() {
			return m_TAAEnable;
		}

		void draw() override {
			ImGui::Begin("Post-Process");
			if (ImGui::CollapsingHeader("Temporal Anti-Aliasing")) {
				ImGui::Checkbox("Enable", &m_TAAEnable);
				ImGui::DragFloat("Lerp", &m_TAAData.lerpAmount, 0.05);
				ImGui::DragInt("Clamping Kernel Size", &m_TAAData.clampingKernelSize);
				ImGui::DragFloat("_FeedbackMin", &m_TAAData._FeedbackMin, 0.05);
				ImGui::DragFloat("_FeedbackMax", &m_TAAData._FeedbackMax, 0.05);
			}

			if (ImGui::CollapsingHeader("Screen Space Shadows")) {
				ImGui::Checkbox("Enable", &m_SSSEnable);
				ImGui::DragInt("Step Count", &m_SSSData.step_count, 1);
				ImGui::DragFloat("Step Size", &m_SSSData.step_size, 0.05);
				ImGui::DragFloat("Depth Threshold", &m_SSSData.depth_threshold, 0.05);
			}
			ImGui::End();
		}

	};
}