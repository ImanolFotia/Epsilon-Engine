#pragma once

#include "editor/ui/catppuccin.hpp"
#include "imgui.h"
#include "ui_element.hpp"
#include <cstring>

namespace Editor::UI {
    class ThemeSelector : public UIElement {
        public:
        void draw() {
            static std::string current_theme;
            auto& style = ImGui::GetStyle();
            ImGui::Begin("Themes", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration);
            ImGui::Text("Current theme: %s", current_theme.c_str());

				static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY;
                ImGui::BeginTable("Theme:", 1, flags);
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
                ImGui::TableHeadersRow();
			        ImGui::TableNextRow();
			        ImGui::TableNextColumn();
                    ImGui::Text("Catppuccin: Latte");
                    if (ImGui::IsItemClicked())
                    {
                        style = Catppuccin::GetPalette<Catppuccin::Latte>();
                        
                        current_theme = "Catppuccin: Latte";
                    }
                    ImGui::TableNextRow();
			        ImGui::TableNextColumn();
                    ImGui::Text("Catppuccin: Frappe");
                    if (ImGui::IsItemClicked())
                    {
                        style = Catppuccin::GetPalette<Catppuccin::Frappe>();
                        current_theme = "Catppuccin: Frappe";
                    }
                    ImGui::TableNextRow();
			        ImGui::TableNextColumn();
                    ImGui::Text("Catppuccin: Machiatto");
                    if (ImGui::IsItemClicked())
                    {
                        style = Catppuccin::GetPalette<Catppuccin::Machiatto>();
                        current_theme = "Catppuccin: Machiatto";
                    }
                    ImGui::TableNextRow();
			        ImGui::TableNextColumn();
                    ImGui::Text("Catppuccin: Mocha");
                    if (ImGui::IsItemClicked())
                    {
                        style = Catppuccin::GetPalette<Catppuccin::Mocha>();
                        current_theme = "Catppuccin: Mocha";
                    }
                    ImGui::TableNextRow();
			        ImGui::TableNextColumn();
                ImGui::EndTable();
            ImGui::End();
        }
    };
}