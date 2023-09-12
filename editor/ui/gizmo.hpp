#pragma once

#include <imgui.h>
#include <modules/imgui/custom_widgets.hpp>

#include <memory>
#include <ImGuizmo/ImGuizmo.h>
#include "ui_element.hpp"

namespace Editor::UI {
	class Gizmo : public UIElement {
		glm::mat4 view{}, proj{};
		glm::mat4* matrix_ptr = nullptr;
		UI::TransformationMode operation;
	public:
		void prepare(glm::mat4* matrix, UI::TransformationMode op, glm::mat4 view, glm::mat4 proj) {
			this->view = view;
			this->proj = proj;
			matrix_ptr = matrix;
			operation = op;
		}
	
		void draw() override {
			ImGuizmo::OPERATION op;
			switch (operation) {

			case TRANSLATE:
				op = ImGuizmo::TRANSLATE;
				break;
			case SCALE:
				op = ImGuizmo::SCALE;
				break;
			case ROTATE:
				op = ImGuizmo::ROTATE;
				break;
			case BOUND:
				op = ImGuizmo::BOUNDS;
				break;
			case ALL:
				op = ImGuizmo::UNIVERSAL;
				break;

			default:
				op = ImGuizmo::UNIVERSAL;
				break;

			}

			static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
			static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };

			float* viewMatrix = (float*)(&view[0]);
			const float* projMatrix = (const float*)(&proj[0]);
			ImGuizmo::Manipulate(viewMatrix, projMatrix, op, ImGuizmo::WORLD, (float*)(matrix_ptr), NULL, NULL, op == ImGuizmo::BOUNDS ? bounds : NULL, NULL);
		}
	};
}