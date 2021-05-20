#pragma once

#include <pch.hpp>

#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <Platform/WindowBase.hpp>
#include <imgui_imp/imgui_impl_glfw.h>
#include <imgui_imp/imgui_impl_opengl3.h>
#include <imgui_imp/imgui_menubar.hpp>
#include <imgui_imp/imgui_Init.hpp>

#include <camera.h>
#include <Entity/Entity.h>
#include <Driver/API/OpenGL/FrameBuffer.h>

namespace Epsilon::Editor::GUI
{
    bool onMenu;

    bool show_app_metrics = false;
    bool show_app_style_editor = false;
    bool show_app_about = false;
    bool p_cross = false;
    static bool show_pipeline_viewer = false;

    float menu_bar_height = 0;
    const float toolbarSize = 50;

    glm::ivec2 mImguiRenderWindow;
    glm::ivec2 mLastImguiRenderWindow;

    bool mShouldResize = false;
    bool mWaitingResize = false;

    glm::vec3 ambientDivider;

    short *WIDTH;
    short *HEIGHT;

    std::shared_ptr<EntityBase> sSelectedEntity;

    static void Init(std::shared_ptr<Platform::WindowBase> window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        const char *glsl_version = "#version 410 core";
        // Setup Dear ImGui style
        //ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        Init_ImGui_Custom();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window->getHandle()->getHandle(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        onMenu = false;
    }

    static void MainWindow(std::vector<std::shared_ptr<EntityBase>> &EntityList)
    {

        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        bool opt_fullscreen = true;
        if (opt_fullscreen)
        {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.x + toolbarSize));
            ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - toolbarSize));
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }

        if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;
        //ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Main Window", NULL, window_flags); // Create a window called "Hello, world!" and append into it.
        ImGui::PopStyleVar();
        //ImGui::PopStyleColor();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("RootDockspace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
        }
        else
        {
            //ShowDockingDisabledMessage();
        }

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::MenuItem("(demo menu)", NULL, false, false);
                if (ImGui::MenuItem("New"))
                {
                }
                if (ImGui::MenuItem("Open", "Ctrl+O"))
                {
                }
                if (ImGui::BeginMenu("Open Recent"))
                {
                    ImGui::MenuItem("fish_hat.c");
                    ImGui::MenuItem("fish_hat.inl");
                    ImGui::MenuItem("fish_hat.h");
                    if (ImGui::BeginMenu("More.."))
                    {
                        ImGui::MenuItem("Hello");
                        ImGui::MenuItem("Sailor");
                        if (ImGui::BeginMenu("Recurse.."))
                        {
                            ShowExampleMenuFile();
                            ImGui::EndMenu();
                        }
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Save", "Ctrl+S"))
                {
                }
                if (ImGui::MenuItem("Save As.."))
                {
                }
                if (ImGui::MenuItem("Add dummy object..."))
                {
                    {
                        glm::vec3 tPosition = glm::vec3(0, 0, 0);
                        glm::vec3 tScale = glm::vec3(1.0f);
                        glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
                        std::string tModelName = "models/Tree.eml";

                        std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

                        auto _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
                        _RComp->CastsShadows(true);
                        _RComp->setTransparency(false);
                        _RComp->setVisibility(true);
                        //Component::Component_ptr _SComp = std::make_shared<Component::ScriptComponent>("scripts/test.lua");
                        _Entity->addComponent(_RComp);

                        EntityList.push_back(_Entity);
                    }
                }

                ImGui::Separator();
                if (ImGui::BeginMenu("Options"))
                {
                    static bool enabled = true;
                    ImGui::MenuItem("Enabled", "", &enabled);
                    ImGui::BeginChild("child", ImVec2(0, 60), true);
                    for (int i = 0; i < 10; i++)
                        ImGui::Text("Scrolling Text %d", i);
                    ImGui::EndChild();
                    static float f = 0.5f;
                    static int n = 0;
                    ImGui::SliderFloat("Value", &f, 0.0f, 1.0f);
                    ImGui::InputFloat("Input", &f, 0.1f);
                    ImGui::Combo("Combo", &n, "Yes\0No\0Maybe\0\0");
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Colors"))
                {
                    float sz = ImGui::GetTextLineHeight();
                    for (int i = 0; i < ImGuiCol_COUNT; i++)
                    {
                        const char *name = ImGui::GetStyleColorName((ImGuiCol)i);
                        ImVec2 p = ImGui::GetCursorScreenPos();
                        ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32((ImGuiCol)i));
                        ImGui::Dummy(ImVec2(sz, sz));
                        ImGui::SameLine();
                        ImGui::MenuItem(name);
                    }
                    ImGui::EndMenu();
                }

                // Here we demonstrate appending again to the "Options" menu (which we already created above)
                // Of course in this demo it is a little bit silly that this function calls BeginMenu("Options") twice.
                // In a real code-base using it would make senses to use this feature from very different code locations.
                if (ImGui::BeginMenu("Options")) // <-- Append!
                {
                    static bool b = true;
                    ImGui::Checkbox("SomeOption", &b);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Disabled", false)) // Disabled
                {
                    IM_ASSERT(0);
                }
                if (ImGui::MenuItem("Checked", NULL, true))
                {
                }
                if (ImGui::MenuItem("Quit", "Alt+F4", &g_Running))
                {
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::BeginMenu("Grid Size"))
                {
                    static bool b = true;
                    ImGui::Checkbox("1x1", &b);
                    ImGui::Checkbox("2x2", &b);
                    ImGui::Checkbox("4x4", &b);
                    ImGui::Checkbox("8x8", &b);
                    ImGui::Checkbox("16x16", &b);
                    ImGui::Checkbox("32x32", &b);
                    ImGui::Checkbox("64x64", &b);
                    ImGui::Checkbox("128x128", &b);
                    ImGui::EndMenu();
                }
                ImGui::MenuItem("Settings", NULL, false, false);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Materials"))
            {
                ImGui::MenuItem("Material Selection", NULL, false, false);
                ImGui::MenuItem("Material Editor", NULL, false, false);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Render"))
            {
                ImGui::MenuItem("Global Parameters", NULL, false, false);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Scene"))
            {
                ImGui::MenuItem("Scene Settings", NULL, false, false);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Project"))
            {
                ImGui::MenuItem("Show Metrics", NULL, false, false);
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Debugger"))
            {
                ImGui::MenuItem("(demo menu)", NULL, false, false);
                if (ImGui::MenuItem("Pipeline viewer", NULL, &show_pipeline_viewer))
                {
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Help"))
            {
                ImGui::MenuItem("Metrics", NULL, &show_app_metrics);
                ImGui::MenuItem("Style Editor", NULL, &show_app_style_editor);
                ImGui::MenuItem("About ImGui", NULL, &show_app_about);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
            menu_bar_height = ImGui::GetCurrentWindow()->MenuBarHeight();
        }
    }

    static void DebugView(std::shared_ptr<PostProcess> PP, std::shared_ptr<OpenGL::FrameBuffer<int>> framebuffer, std::shared_ptr<Camera> camera, Shader_ptr GammaShader)
    { 
                if (show_pipeline_viewer)
                {
                    static ImVec2 fu = ImVec2(960, 300);
                    ImGui::SetNextWindowSize(fu, ImGuiCond_FirstUseEver);
                    ImGui::Begin("Debug Framebuffer", &Editor::GUI::show_pipeline_viewer);

                    //get the mouse position
                    ImVec2 pos = ImGui::GetCursorScreenPos();

                    static const char *items[] = {"gBuffer Normal", "Light Pass", "TAA", "HBIL", "Screen Space Reflections", "FrameBuffer"};
                    static const char *item_current = items[0];
                    static ImGuiComboFlags flags = 0;
                    if (ImGui::BeginCombo("Display", item_current, flags))
                    {
                        for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                        {
                            bool is_selected = (item_current == items[n]);
                            if (ImGui::Selectable(items[n], is_selected))
                                item_current = items[n];
                            if (is_selected)
                                ImGui::SetItemDefaultFocus(); // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
                        }
                        ImGui::EndCombo();
                    }

                    auto fov = camera->getFoV();

                    auto TextureId = 0;

                    if (item_current == "Light Pass")
                        TextureId = PP->getSceneTexture();
                    else if (item_current == "gBuffer Normal")
                        TextureId = PP->gBufferFramebuffer->getRenderTargetHandler(PostProcess::GBUFFER_TARGETS::GBUFFER_NORMAL);
                    else if (item_current == "Screen Space Reflections")
                        TextureId = PP->ReflectionTexture;
                    else if (item_current == "TAA")
                        TextureId = PP->mTAAFramebufferCopy->getRenderTargetHandler(0);
                    else if (item_current == "HBIL")
                        TextureId = PP->HBILFramebuffer->getRenderTargetHandler(0);
                    else if (item_current == "FrameBuffer")
                        TextureId = framebuffer->getRenderTargetHandler(0);

                    //we are done working with this window
                    unsigned int fb_size[3] = {(unsigned int)*WIDTH, (unsigned int)*HEIGHT, GammaShader->getProgramID()};

                    //ImGui::GetWindowDrawList()->AddCallback([]() { }, nullptr);
                    ImGui::Image((void *)TextureId, ImVec2(mImguiRenderWindow.x, mImguiRenderWindow.y), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

                    //ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
                    ImGui::End();
                }
    }

    static void SkySettings(std::shared_ptr<Skybox> skybox)
    {
        //if (!ImGui::CollapsingHeader("Skybox")) return;

        ImGui::Begin("Skybox"); // Create a window called "Hello, world!" and append into it.

        ImGui::Checkbox("Clouds", &skybox->clouds); // Edit bools storing our window open/close state

        ImGui::SliderFloat("Horizon Height", &skybox->Horizon_Height, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("Upper Sky Color", (float *)&skybox->UpperSky_color);    // Edit 3 floats representing a color
        ImGui::ColorEdit3("Lower Sky Color", (float *)&skybox->LowerSky_color);    // Edit 3 floats representing a color
        ImGui::ColorEdit3("Tint", (float *)&skybox->Horizon_color);                // Edit 3 floats representing a color

        ImGui::Separator();
        ImGui::SliderFloat3("Ambient divider", &ambientDivider[0], 0, 50);

        ImGui::End();
    }

    
    static void MaterialEditor()
    {
        using namespace Component;
        if(sSelectedEntity->HasRenderComponent()) {
            
            auto RComponent = std::static_pointer_cast<RenderComponent>(sSelectedEntity->getComponent<RenderComponent_ptr>());
            auto& ref = ResourceManager::Get();
            auto model = ref.getModel(RComponent->getModelPath());
            auto & meshes = model->Meshes();

            ImGui::Begin("Material Editor"); 
            
            /*
            static std::string sizes[] = {"64", "128", "256", "512"};
            static std::string current_item = "1";
            if (ImGui::BeginCombo("Selected Mesh", current_item.c_str()))
            {
                for (int n = 0; n < meshes.size(); n++)
                {
                    bool is_selected = (current_item == sizes[n]); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(sizes[n].c_str(), is_selected))
                    {
                        current_item = sizes[n];
                        imgSize = (n + 1) * 64;
                    }
                }
                ImGui::EndCombo();
            }


            if(ImGui::CollapsingHeader("Albedo")) {
                ImGui::Checkbox("Use albedo texture", &); // Edit bools storing our window open/close state
            }

            ImGui::SliderFloat("Horizon Height", &skybox->Horizon_Height, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("Upper Sky Color", (float *)&skybox->UpperSky_color);    // Edit 3 floats representing a color
            ImGui::ColorEdit3("Lower Sky Color", (float *)&skybox->LowerSky_color);    // Edit 3 floats representing a color
            ImGui::ColorEdit3("Tint", (float *)&skybox->Horizon_color);                // Edit 3 floats representing a color

            ImGui::Separator();
            ImGui::SliderFloat3("Ambient divider", &ambientDivider[0], 0, 50);
*/
            ImGui::End();
        }
    }

    static void PostprocessSettings(std::shared_ptr<PostProcess> Postprocess_ref)
    {

        auto &PostprocessData = Postprocess_ref->getPostProcessData();

        ImGui::Begin("Postprocess Settings"); // Create a window called "Hello, world!" and append into it.

        if (ImGui::CollapsingHeader("Image Settings"))
        {
            ImGui::SliderFloat("Brightness", &PostprocessData.ImageSettings.Brightness, -5.0, 5.0);
            ImGui::SliderFloat("Gamma", &PostprocessData.ImageSettings.Gamma, 0.0, 4.4);
            ImGui::SliderFloat("Saturation", &PostprocessData.ImageSettings.Saturation, -5.0, 5.0);
            ImGui::SliderFloat("Contrast", &PostprocessData.ImageSettings.Contrast, -5.0, 5.0);
            ImGui::SliderFloat("Exposure", &PostprocessData.ImageSettings.Exposure, 0.0, 10.0);
        }

        if (ImGui::CollapsingHeader("Screen Space Reflections"))
        {
            ImGui::Checkbox("SSR Active", &PostprocessData.ScreenSpaceReflections.Active);
            ImGui::SliderFloat("Min Ray Step", &PostprocessData.ScreenSpaceReflections.MinRayStep, 0.01, 0.25);
            ImGui::SliderFloat("Max Ray Step", &PostprocessData.ScreenSpaceReflections.MaxRayStep, 0.01, 0.25);
            ImGui::SliderInt("Max Steps", &PostprocessData.ScreenSpaceReflections.MaxSteps, 25, 250);
            ImGui::SliderFloat("Roughness Cutoff", &PostprocessData.ScreenSpaceReflections.RoughnessCutoff, 0.0, 1.0);
            ImGui::SliderFloat("Depth Cutoff", &PostprocessData.ScreenSpaceReflections.DepthCutoff, -10.0, 10.0);
            ImGui::SliderInt("Refinement Steps", &PostprocessData.ScreenSpaceReflections.MaxRefinementSteps, 0, 20);
            ImGui::Separator();
            ImGui::Checkbox("Denoiser", &PostprocessData.DenoiseSettings.Active);
            ImGui::SliderFloat("Sigma", &PostprocessData.DenoiseSettings.Sigma, 0.0, 5.0);
            ImGui::SliderFloat("kSigma", &PostprocessData.DenoiseSettings.kSigma, 0.0, 5.0);
            ImGui::SliderFloat("Threshold", &PostprocessData.DenoiseSettings.Threshold, 0.0, 1.0); 
            ImGui::SliderFloat("RoughnessCutoff", &PostprocessData.DenoiseSettings.RoughnessCutoff, 0.0, 1.0);

        }

        if (ImGui::CollapsingHeader("Screen Space Ambient Oclussion (HBAO)"))
        {
            ImGui::Checkbox("HBAO Active", &PostprocessData.HBAO.Active);
            ImGui::SliderInt("Directions", &PostprocessData.HBAO.NumDirections, 1, 16);
            ImGui::SliderInt("Samples", &PostprocessData.HBAO.Samples, 1, 16);
        }

        if (ImGui::CollapsingHeader("Motion Blur"))
        {
            ImGui::Checkbox("Motion Blur Active", &PostprocessData.MotionBlur.Active);
            ImGui::SliderInt("Velocity Samples", &PostprocessData.MotionBlur.Samples, 2, 32);
            ImGui::SliderInt("Strength", &PostprocessData.MotionBlur.Strength, 1, 5);
            ImGui::SliderFloat("Frametime Target", &PostprocessData.MotionBlur.FrameTimeTarget, 0.004, 0.064);
        }
        
        if (ImGui::CollapsingHeader("Anti Aliasing"))
        {
            ImGui::Checkbox("TAA Active", &PostprocessData.AntiAliasingSettings.Active);
            ImGui::SliderInt("Clamping Kernel Size", &PostprocessData.AntiAliasingSettings.ClampingKernelSize, 1, 6);
            ImGui::SliderFloat("Lerp Amount", &PostprocessData.AntiAliasingSettings.LerpAmount, 0.0, 1.0);
            ImGui::SliderFloat("Jitter Multiplier", &PostprocessData.AntiAliasingSettings.JitterMultiplier, 0.0, 3.0);
        }

        ImGui::End();
    }

    static void EntityProperties()
    {
        if (sSelectedEntity == nullptr)
            return;

        using namespace Component;
        ImGui::Begin("Properties");

        if (!ImGui::CollapsingHeader("Transform"))
        {
            if (sSelectedEntity->HasPhysicComponent())
            {
                auto PComponent = std::static_pointer_cast<PhysicComponent>(sSelectedEntity->getComponent<PhysicComponent_ptr>());
                auto p = PComponent->getPosition();

                //if(PComponent->isActive) {
                float tmpppos[3] = {p.x, p.y, p.z};
                ImGui::DragFloat3("Position", tmpppos, 1.0, -30.0, 30.0);
                p = glm::vec3(tmpppos[0], tmpppos[1], tmpppos[2]);

                if (!PComponent->isActive)
                {
                    auto t = PComponent->getTransform();
                    t.setOrigin(btVector3(p.x, p.y, p.z));
                    PComponent->setTransform(t);
                }

                bool initialState = PComponent->isActive;
                ImGui::Checkbox("Active", &PComponent->isActive);
                PComponent->setState(PComponent->isActive);
            }
            else if (sSelectedEntity->HasClothComponent())
            {
                auto CComponent = std::static_pointer_cast<ClothComponent>(sSelectedEntity->getComponent<ClothComponent_ptr>());

                if (CComponent == nullptr)
                {
                    ImGui::End();
                    return;
                }

                btSoftBody *cloth = (std::dynamic_pointer_cast<btSoftBody>(CComponent->SoftBodyPointer->m_BodyCloth)).get();
                btSoftBody::tNodeArray &nodes(cloth->m_nodes);

                auto &node0 = nodes[0];
                auto &node1 = nodes[29];

                float tmpppos[3] = {node0.m_x.x(), node0.m_x.y(), node0.m_x.z()};
                auto diff = node1.m_x - node0.m_x;

                ImGui::DragFloat3("Position", tmpppos, 1.0, -30.0, 30.0);
                node0.m_x = btVector3(tmpppos[0], tmpppos[1], tmpppos[2]);
                node1.m_x = btVector3(tmpppos[0], tmpppos[1], tmpppos[2]) + diff;
            }
            else if (sSelectedEntity->HasRenderComponent())
            {
                auto RComponent = std::static_pointer_cast<RenderComponent>(sSelectedEntity->getComponent<RenderComponent_ptr>());
                auto p = RComponent->getPosition();
                float tmpppos[3] = {p.x, p.y, p.z};
                ImGui::DragFloat3("Position", tmpppos, 1.0, -30.0, 30.0);
                p = glm::vec3(tmpppos[0], tmpppos[1], tmpppos[2]);
                RComponent->setPosition(p);
            }
        }
        ImGui::End();
    }

    static void ToolbarUI()
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menu_bar_height));
        ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbarSize));
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiWindowFlags window_flags = 0 | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
        ImGui::Begin("TOOLBAR", NULL, window_flags);
        ImGui::PopStyleVar();

        ImGui::Button("Toolbar goes here", ImVec2(0, 37));

        ImGui::End();
    }

    static void InfoOverlay(bool *p_open, ImVec2 pos, ImVec2 size, std::shared_ptr<Camera> camera)
    {
        const float PADX = 15.0f;
        const float PADY = 40.0f;
        static int corner = 0;
        ImGuiIO &io = ImGui::GetIO();
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
        if (corner != -1)
        {
            ImVec2 mwindow_size = ImGui::GetContentRegionAvail();
            ImVec2 mwindow_pos = ImGui::GetWindowPos();
            //ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = mwindow_pos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = mwindow_size;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PADX) : (work_pos.x + PADX);
            window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PADY) : (work_pos.y + PADY);
            window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            window_flags |= ImGuiWindowFlags_NoMove;
        }
        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        if (ImGui::Begin("Example: Simple overlay", p_open, window_flags))
        {
            ImGui::Text("Viewport Info");
            ImGui::Separator();
            ImGui::Text("Window size: %.0f x %.0f", size.x, size.y);
            ImGui::Text("View position: x=%.2f, y=%.2f, z=%.2f", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
            ImGui::Text("frametime/fps: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Allocated Texture Space: ~ %d Mb", ResourceManager::Get().getTextureBytesAllocation() / (1000*1000));
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse Position: <invalid>");
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::MenuItem("Custom", NULL, corner == -1))
                    corner = -1;
                if (ImGui::MenuItem("Top-left", NULL, corner == 0))
                    corner = 0;
                if (ImGui::MenuItem("Top-right", NULL, corner == 1))
                    corner = 1;
                if (ImGui::MenuItem("Bottom-left", NULL, corner == 2))
                    corner = 2;
                if (ImGui::MenuItem("Bottom-right", NULL, corner == 3))
                    corner = 3;
                if (p_open && ImGui::MenuItem("Close"))
                    *p_open = false;
                ImGui::EndPopup();
            }
        }
        ImGui::End();
    }

    static void MainViewport(std::shared_ptr<OpenGL::FrameBuffer<int>> framebuffer, std::shared_ptr<Camera> camera)
    {

        ImGui::Begin("Main Render");
        ImVec2 window_size = ImGui::GetContentRegionAvail();
        ImVec2 window_pos = ImGui::GetWindowPos();
        mLastImguiRenderWindow = mImguiRenderWindow;
        mImguiRenderWindow = glm::ivec2(window_size.x, window_size.y);

        mShouldResize = mImguiRenderWindow != mLastImguiRenderWindow;

        auto TextureId = framebuffer->getRenderTargetHandler(0);
        static bool showOverlay = true;
        InfoOverlay(&showOverlay, window_pos, window_size, camera);

        ImGui::Image((void *)TextureId, ImVec2(window_size.x, window_size.y - 5), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
        /*ImGui::SetCursorPos(ImVec2(15, 30));
                ImGui::Text("Window size: %.0f x %.0f", window_size.x, window_size.y);
                ImGui::SetCursorPosX(15);
                ImGui::Text("Image size: %.0f x %.0f", window_size.x, window_size.y);
                ImGui::SetCursorPos(ImVec2(15, window_size.y - 30));
                ImGui::Text("View position: x=%.2f, y=%.2f, z=%.2f", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
                ImGui::SetCursorPosX(10);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);*/
        ImGui::End();
    }

    static void TextureList()
    {
        static int imgSize = 128;

        int numChoices = 4;

        ImGui::Begin("Textures");
        //Image size choise
        {
            static std::string sizes[] = {"64", "128", "256", "512"};
            static std::string current_item = "128";
            if (ImGui::BeginCombo("Image size", current_item.c_str()))
            {
                for (int n = 0; n < numChoices; n++)
                {
                    bool is_selected = (current_item == sizes[n]); // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(sizes[n].c_str(), is_selected))
                    {
                        current_item = sizes[n];
                        imgSize = (n + 1) * 64;
                    }
                }
                ImGui::EndCombo();
            }
        }

        ImGuiStyle *style = &ImGui::GetStyle();
        imgSize = glm::max(imgSize, 64);

        float window_width = ImGui::GetWindowContentRegionWidth();

        int numHorizontalImages = glm::max(((int)window_width / (int)(imgSize + (style->ItemSpacing.x * 2))), 1);

        const auto &TextureArray = ResourceManager::Instance().getTextureArray();

        ImGui::Columns(numHorizontalImages, NULL);
        ImGui::Separator();

        int counter = 0;
        for (auto &texture : TextureArray)
        {

            if (counter < 0 && counter % glm::max(numHorizontalImages, 1) == 0)
                ImGui::Separator();

            auto textureId = texture.second->ID();
            ImGui::Image((void *)(unsigned int)textureId, ImVec2(imgSize, imgSize), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
            ImGui::Text(texture.first.c_str());
            ImGui::NextColumn();
            counter++;
        }

        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::End();
    }

    static void SceneHierarchy(std::vector<std::shared_ptr<EntityBase>> &EntityList)
    {
        ImGui::Begin("Hierarchy");
        if (ImGui::TreeNode("Entities"))
        {
            for (const auto &ent : EntityList)
            {
                if (ImGui::TreeNode(ent->getHash().c_str()))
                {
                    if (ImGui::IsItemClicked(0))
                    {
                        sSelectedEntity = ent;
                    }
                    ImGui::Indent();
                    if (ent->HasRenderComponent())
                    {
                        if (ImGui::TreeNode("Render Component"))
                        {
                            ImGui::Indent();
                            ImGui::Text(ent->getComponent<Component::RenderComponent_ptr>()->getModelPath().c_str());
                            ImGui::TreePop();
                            ImGui::Unindent();
                        }
                    }
                    if (ent->HasClothComponent())
                    {
                        if (ImGui::TreeNode("Cloth Component"))
                        {
                            ImGui::TreePop();
                        }
                    }
                    if (ent->HasSoundComponent())
                    {
                        if (ImGui::TreeNode("Sound Component"))
                        {
                            ImGui::Indent();
                            ImGui::Text(ent->getComponent<Component::SoundComponent_ptr>()->getFilename().c_str());
                            ImGui::Unindent();
                            ImGui::TreePop();
                        }
                    }
                    if (ent->HasPhysicComponent())
                    {
                        if (ImGui::TreeNode("Physic Component"))
                        {
                            auto comp = ent->getComponent<Component::PhysicComponent_ptr>();
                            ImGui::Indent();
                            ImGui::Text((std::string("State: ") + std::string(comp->isStatic() ? "Static" : "Dynamic")).c_str());
                            ImGui::Text((std::string("Type: ") + std::string(comp->getTypeStr())).c_str());
                            ImGui::Unindent();
                            ImGui::TreePop();
                        }
                    }
                    if (ImGui::TreeNode("Transform"))
                    {
                        const glm::vec3 &pos = ent->getPosition();
                        const glm::vec3 &sc = ent->getScale();
                        const glm::quat &rot = ent->getRotation();
                        ImGui::Indent();
                        ImGui::Text(std::string(std::string("Position: x=") + std::to_string(pos.x) + " y=" + std::to_string(pos.y) + " z=" + std::to_string(pos.z)).c_str());
                        ImGui::Text(std::string(std::string("Scale: x=") + std::to_string(sc.x) + " y=" + std::to_string(sc.y) + " z=" + std::to_string(sc.z)).c_str());
                        ImGui::Text(std::string(std::string("Rotation (quat): x=") + std::to_string(rot.x) + " y=" + std::to_string(rot.y) + " z=" + std::to_string(rot.z) + " w=" + std::to_string(rot.w)).c_str());
                        ImGui::Unindent();
                        ImGui::TreePop();
                    }
                    //ImGui::Text("Num Slots");
                    //ImGui::Text("Count");
                    ImGui::Unindent();
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        //ImGui::Indent();
        //ImGui::Text("Previous Modifications");
        //ImGui::Text("Debug Ticks");
        //ImGui::Unindent();
        ImGui::End();
        /*End scene window*/

        //ImGui::End();

        ImGui::End();
    }

    static void Render(std::shared_ptr<Platform::WindowBase> window)
    {
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window->getHandle()->getHandle(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glEnable(GL_DEPTH_TEST);
        glViewport(0, 0, *WIDTH, *HEIGHT);
    }

    static void PollEvents(std::shared_ptr<OpenGL::FrameBuffer<int>> framebuffer, std::shared_ptr<PostProcess> PP)
    {
        if (mShouldResize || mWaitingResize)
        {
            if (Input::Mouse::LEFT == Input::PRESSED)
            {
                mWaitingResize = true;
            }
            else
            {
                //auto &window_data = window->getWindowData();
                //this->WIDTH = window_data.Width;
                //this->HEIGHT = window_data.Height;
                *WIDTH = mImguiRenderWindow.x;
                *HEIGHT = mImguiRenderWindow.y;
                framebuffer->Resize(*WIDTH, *HEIGHT);
                PP->Resize(*WIDTH, *HEIGHT);
                std::cout << "Needs to resize " << *WIDTH << " " << *HEIGHT << std::endl;
                mWaitingResize = false;
            }
        }
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }
}