#pragma once
#include <imgui.h>
#include <glm/glm.hpp>

namespace EpsilonUI {

    //Modified vector ui element from spartan engine
    //https://github.com/PanosK92/SpartanEngine/blob/343a5d88ea61c32a75afe7f1d1fd65e33a3b7b13/editor/ImGui/ImGuiExtension.h
    static bool vector3(const char* label, glm::vec3& vector)
    {
        const float label_indetation = 15.0f * ImGui::GetWindowDpiScale();

        bool used = false;

        const auto show_float = [](glm::vec3 axis, float* value) -> bool
        {
            const float label_float_spacing = 15.0f * ImGui::GetWindowDpiScale();
            const float step = 0.01f;
            bool res = false;

            // Label
            ImGui::TextUnformatted(axis.x == 1.0f ? "x" : axis.y == 1.0f ? "y" : "z");
            ImGui::SameLine(label_float_spacing);
            glm::vec2 pos_post_label = glm::vec2(ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y);

            // Float
            ImGui::PushID(static_cast<int>(ImGui::GetCursorPosX() + ImGui::GetCursorPosY()));
            res = ImGui::DragFloat("##no_label", value, step, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), "%.4f");
            ImGui::PopID();

            // Axis color
            static const ImU32 color_x = IM_COL32(168, 46, 2, 255);
            static const ImU32 color_y = IM_COL32(112, 162, 22, 255);
            static const ImU32 color_z = IM_COL32(51, 122, 210, 255);
            static const glm::vec2 size = glm::vec2(4.0f, 16.0f);
            static const glm::vec2 offset = glm::vec2(0.0f, 1.0);
            pos_post_label += offset;
            ImVec2 min, max;
            min = ImVec2(pos_post_label.x, pos_post_label.y);
            max = ImVec2(pos_post_label.x + size.x, pos_post_label.y + size.y);
            ImGui::GetWindowDrawList()->AddRectFilled(min, max, axis.x == 1.0f ? color_x : axis.y == 1.0f ? color_y : color_z);

            return res;
        };

        ImGui::BeginGroup();
        ImGui::Indent(label_indetation);
        ImGui::TextUnformatted(label);
        ImGui::Unindent(label_indetation);
        used |= show_float(glm::vec3(1.0f, 0.0f, 0.0f), &vector.x);
        used |= show_float(glm::vec3(0.0f, 1.0f, 0.0f), &vector.y);
        used |= show_float(glm::vec3(0.0f, 0.0f, 1.0f), &vector.z);
        ImGui::EndGroup();

        return used;
    };
}