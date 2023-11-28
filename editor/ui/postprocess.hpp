#pragma once

#include <imgui.h>
#include <modules/imgui/custom_widgets.hpp>
#include "../renderpasses/taa.hpp"

#include <memory>

namespace Editor::UI {
	class PostProcess : public UIElement {

		bool m_pTAAEnable = true;

	public:

		Renderpasses::TAAUniformData m_pTAAData;

		bool TaaEnabled() {
			return m_pTAAEnable;
		}

		void draw() override {
			ImGui::Begin("Post-Process");
			if (ImGui::CollapsingHeader("TAA")) {
				ImGui::Checkbox("Enable", &m_pTAAEnable);
				ImGui::DragFloat("Lerp", &m_pTAAData.lerpAmount, 0.05);
				ImGui::DragInt("Clamping Kernel Size", &m_pTAAData.clampingKernelSize);
				ImGui::DragFloat("_FeedbackMin", &m_pTAAData._FeedbackMin, 0.05);
				ImGui::DragFloat("_FeedbackMax", &m_pTAAData._FeedbackMax, 0.05);
			}
			ImGui::End();
		}

	};
}