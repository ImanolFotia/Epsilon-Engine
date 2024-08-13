#pragma once

#include "ui_element.hpp"

namespace Editor::UI {
	class Toolbar : UIElement {


	public:

		enum STATUS {
			Playing,
			Paused,
			Stopped,
			StepPlay,
			StepWait
		};

		enum StepStatus {
			Next,
			Current
		};

		STATUS getStatus() {
			return status;
		}

		void draw() override {
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + 20));
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, 50));
			ImGui::SetNextWindowViewport(viewport->ID);

			ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
			ImGui::Begin("TOOLBAR", NULL, window_flags);
			ImGui::PopStyleVar();

			ImGui::Button(ICON_FA_SAVE, ImVec2(37, 37));
			ImGui::SameLine();
			ImGui::Button(ICON_FA_FILE_EXPORT, ImVec2(37, 37));
			ImGui::SameLine();

			if (status == Stopped)
			{
				if (ImGui::Button(ICON_FA_PLAY, ImVec2(37, 37)))
				{
					status = Playing;
				}
			}
			else if (status == Playing)
			{
				if (ImGui::Button(ICON_FA_PAUSE, ImVec2(37, 37)))
				{
					status = Paused;
					step_status = Current;
				}
			}
			else if (status == Paused)
			{
				if (ImGui::Button(ICON_FA_PLAY, ImVec2(37, 37)))
				{

					status = Playing;
				}
				ImGui::SameLine();
				if (ImGui::Button(ICON_FA_ARROW_RIGHT, ImVec2(37, 37)))
				{

					step_status = StepStatus::Next;
				}
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP, ImVec2(37, 37)))
			{
				status = Stopped;
			}
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_REDO, ImVec2(37, 37)))
			{
				status = Stopped;
			}
			ImGui::SameLine();


			ImGui::End();
		}

		void SetStepStatus(StepStatus st) {
			step_status = st;
		}

		StepStatus GetStepStatus(){
			return step_status;
	}

	private:


		STATUS status = Stopped;
		StepStatus step_status = StepStatus::Next;
	};
}