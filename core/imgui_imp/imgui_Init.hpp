#pragma once
#include <imgui_internal.h>
#include <imgui.h>

#include "IconsFontAwesome5.h"

static void Init_ImGui_Custom()
{

    ImGuiStyle *style = &ImGui::GetStyle();

    ImVec4 BackgroundDark = ImVec4(48/255.0, 56/255.0, 65/255.0, 1.00f);
    ImVec4 BackgroundLight = ImVec4(58/255.0, 71/255.0, 80/255.0, 1.00f);
    ImVec4 ActiveColor = ImVec4(0, 173/255.0, 181/255.0, 1.00f);
    ImVec4 TextColor = ImVec4(238.0/255.0, 238/255.0, 238/255.0, 1.00f);
    ImVec4 TextColorSelected = ImVec4(150.0/255.0, 150/255.0, 150/255.0, 1.00f);
    ImVec4 DisabledTextColor = ImVec4(200.0/255.0, 200/255.0, 200/255.0, 1.00f);

    ImVec4 SecondaryActiveColor = ImVec4(0, 130/255.0, 150/255.0, 1.00f);

    //style->WindowPadding = ImVec2(15, 15);
    //style->WindowRounding = 0.0f;
    //style->FramePadding = ImVec2(5, 5);
    //style->FrameRounding = 0.0f;
    //style->ItemSpacing = ImVec2(12, 8);
    //style->ItemInnerSpacing = ImVec2(8, 6);
    //style->IndentSpacing = 25.0f;
    //style->ScrollbarSize = 15.0f;
    style->GrabMinSize = 5.0f;

    style->ScrollbarRounding = 5.0f;
    style->GrabRounding = 5.0f;
    style->FrameRounding = 5.0f;
    style->WindowRounding = 5.0f;
    style->ChildRounding = 5.0f;
    style->PopupRounding = 5.0f;
    style->TabRounding = 5.0f;
 
    style->Colors[ImGuiCol_Text] = TextColor;
    style->Colors[ImGuiCol_TextDisabled] = DisabledTextColor;
    style->Colors[ImGuiCol_WindowBg] = BackgroundDark;
    //style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_PopupBg] = BackgroundLight;
    style->Colors[ImGuiCol_Border] = BackgroundDark;
    style->Colors[ImGuiCol_BorderShadow] = BackgroundLight;
    style->Colors[ImGuiCol_FrameBg] = BackgroundLight;
    style->Colors[ImGuiCol_FrameBgHovered] = BackgroundLight;
    style->Colors[ImGuiCol_FrameBgActive] = BackgroundLight;
    style->Colors[ImGuiCol_TitleBg] = BackgroundDark;
    style->Colors[ImGuiCol_TitleBgCollapsed] = BackgroundLight;
    style->Colors[ImGuiCol_TitleBgActive] = BackgroundLight;
    style->Colors[ImGuiCol_MenuBarBg] = BackgroundDark;
    style->Colors[ImGuiCol_ScrollbarBg] = BackgroundDark;
    style->Colors[ImGuiCol_ScrollbarGrab] = BackgroundLight;
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = SecondaryActiveColor;
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ActiveColor;
    //style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ActiveColor;
    style->Colors[ImGuiCol_SliderGrab] = SecondaryActiveColor;
    style->Colors[ImGuiCol_SliderGrabActive] = ActiveColor;
    style->Colors[ImGuiCol_Button] = BackgroundDark;
    style->Colors[ImGuiCol_ButtonHovered] = BackgroundLight;
    style->Colors[ImGuiCol_ButtonActive] = ActiveColor;
    style->Colors[ImGuiCol_Header] = BackgroundDark;
    style->Colors[ImGuiCol_HeaderHovered] = SecondaryActiveColor;
    style->Colors[ImGuiCol_HeaderActive] = ActiveColor;
    //style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    //style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    //style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    //style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
    //style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
    //style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = TextColorSelected;
    style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

    
    style->Colors[ImGuiCol_Tab] = BackgroundDark;
    style->Colors[ImGuiCol_TabHovered] = SecondaryActiveColor;
    style->Colors[ImGuiCol_TabActive] = ActiveColor;
    style->Colors[ImGuiCol_TabUnfocused] = SecondaryActiveColor;
    style->Colors[ImGuiCol_TabUnfocusedActive] = BackgroundLight;

    style->WindowBorderSize = 1.0;
    style->FrameBorderSize = 1.0;
    style->PopupBorderSize = 1.0;

    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.Fonts->AddFontFromFileTTF("./resources/Roboto-Regular.ttf", 12);
    //io.Fonts->AddFontFromFileTTF("./resources/Roboto-Regular.ttf", 10);
    //io.Fonts->AddFontFromFileTTF("./resources/Roboto-Regular.ttf", 15); 
    ImFontConfig config;
    config.OversampleH = 2; 
    io.Fonts->AddFontFromFileTTF("./resources/chirp-regular-web.ttf", 16, &config);
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
    icons_config.GlyphOffset.y = 0.5;
    io.Fonts->AddFontFromFileTTF("./resources/fa-regular-400.ttf", 15.0f, &icons_config, icons_ranges );
    io.Fonts->AddFontFromFileTTF("./resources/fa-brands-400.ttf", 15.0f, &icons_config, icons_ranges );
    io.Fonts->AddFontFromFileTTF("./resources/fa-solid-900.ttf", 15.0f, &icons_config, icons_ranges );
    
}