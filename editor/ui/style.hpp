#pragma once

#include <imgui.h>

namespace Editor {
static inline void setup_style() {
  {
    /*
                            ImGuiStyle* style = &ImGui::GetStyle();
                            style->Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
                            style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
                            style->Colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
                            style->Colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
                            style->Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
                            style->Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.25f);
                            style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                            style->Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
                            style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
                            style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.68f, 0.69f, 1.00f);
                            style->Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
                            style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
                            style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
                            style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
                            style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
                            style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
                            //style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
                            style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
                            //style->Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
                            style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.73f, 0.78f, 1.00f);
                            style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                            style->Colors[ImGuiCol_Button] = ImVec4(0.02f, 0.60f, 0.60f, 0.00f);
                            //style->Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
                            style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
                            style->Colors[ImGuiCol_Header] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
                            style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
                            style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                            style->Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
                            style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
                            style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
                            //style->Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
                            style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
                            style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
                            style->Colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
                            style->Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
                            style->Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.73f, 0.78f, 1.00f);
                            style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
                            style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
                            style->Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
                            style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
                            style->Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
                            //style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
                            style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
                            //style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
                            style->Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
                            style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
                            style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
                            style->Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
                            style->Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
                            style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.73f, 0.78f, 1.00f);
                            style->Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
                            style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
                            style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
                            style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
                            style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

                            style->Colors[ImGuiCol_PlotLines] = ImVec4(225.0 / 255.0, 121.0 / 255.0, 12.0 / 255.0, 1.0);

                            style->WindowPadding = ImVec2(5, 5);
                            style->WindowRounding = 0.0;
                            style->WindowTitleAlign = ImVec2(0.5, 0.5);
                            style->WindowBorderSize = 0.0;*/
  }
}
} // namespace Editor