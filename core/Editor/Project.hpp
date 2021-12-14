#pragma once

#include <imgui.h>
#include "imgui_imp/IconsFontAwesome5.h"

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#endif

namespace Epsilon::Editor::GUI::Project
{
    void NewProject(bool *status)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("New Project"))
        {
#ifdef _WIN32
            TCHAR user_path[MAX_PATH] = {0};
            auto res = GetEnvironmentVariable(_T("USERPROFILE"), user_path, MAX_PATH);
#endif
#ifdef __linux__
            struct passwd *pw = getpwuid(getuid());
            const char *user_path = pw->pw_dir;
#endif
            static char project_name[64] = "Project1";
            static auto docs_directory = std::string(user_path);
            #ifdef _WIN32
            static std::string project_directory = docs_directory + std::string("\\EpsilonProjects\\");
            #endif
            #ifdef __linux__
            static std::string project_directory = docs_directory + std::string("/EpsilonProjects/");
            #endif
            std::string complete_dir = (std::string(project_directory) + std::string(project_name));
            static char project_description[15000] = {0};

            ImGui::InputText("Name of the project:", project_name, IM_ARRAYSIZE(project_name));
            ImGui::InputText("Project directory:", (char *)complete_dir.c_str(), complete_dir.size(), ImGuiInputTextFlags_ReadOnly);
            ImGui::SameLine();
            ImGui::Button(ICON_FA_FOLDER_OPEN);
            ImGui::InputTextMultiline("Brief description (optional):", project_description, IM_ARRAYSIZE(project_description));
            ImGui::Separator();
            static int option = 0;
            ImGui::Text("Project template (not implemented):");
            ImGui::RadioButton("Empty", &option, 0);
            ImGui::SameLine();
            ImGui::RadioButton("2D", &option, 1);
            ImGui::SameLine();
            ImGui::RadioButton("3D", &option, 2);
            if (ImGui::Button("Create"))
            {
                auto &FileSystemReference = Filesystem::Filesystem::Instance();

                auto dir_bkp = std::filesystem::current_path();
                std::filesystem::current_path(docs_directory);
                FileSystemReference.mount(complete_dir, Filesystem::VirtualFilesystem::Type::directory, "ProjectDirectory");
                FileSystemReference.getFilesystem("ProjectDirectory");
                std::filesystem::current_path(dir_bkp);
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
                *status = false;
            }
            ImGui::EndPopup();
        }
    }
}