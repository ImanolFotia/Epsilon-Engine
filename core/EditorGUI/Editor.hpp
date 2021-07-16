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
#include "imgui_imp/IconsFontAwesome5.h"

#include <camera.h>
#include <Entity/Entity.h>
#include <Driver/API/OpenGL/FrameBuffer.h>

#include <Filesystem/Filesystem.hpp>

#include <IO/Mouse.h>
#include <beacon/beacon.hpp>

/*#include <Audio/Audio.h>
#include <Audio/AudioElement.h>
#include <Audio/AudioListener.h>*/

namespace Epsilon::Editor::GUI
{
    enum STATUS
    {
        Playing,
        Paused,
        Stopped
    };

    bool onMenu;

    bool show_app_metrics = false;
    bool show_app_style_editor = false;
    bool show_app_about = false;
    bool p_cross = false;
    STATUS status = Stopped;
    static bool show_pipeline_viewer = false;
    bool camera_selected = false;

    float menu_bar_height = 0;
    const float toolbarSize = 50;

    glm::ivec2 mImguiRenderWindow;
    glm::ivec2 mLastImguiRenderWindow;
    static int32_t sSelectedLight = -1;

    glm::vec4 MousePosOnViewport;

    static bool showOverlay = true;
    float TEXT_BASE_WIDTH;
    float TEXT_BASE_HEIGHT;

    std::shared_ptr<PostProcess> PostProcess_ref;

    bool mShouldResize = false;
    bool mWaitingResize = false;
    bool lightSelected = false;
    bool isMainWindowFocused = false;

    glm::vec3 ambientDivider;

    short *WIDTH;
    short *HEIGHT;

    std::shared_ptr<EntityBase> sSelectedEntity;
    int sSelectedEntityIndex = -1;
    void onLeftClickCallback(beacon::sender *sender, beacon::args *args);
    static void Init(std::shared_ptr<Platform::WindowBase> window)
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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

        using namespace Input;
        Mouse::MouseEventHandler.addListener(&onLeftClickCallback);

        onMenu = false;

        auto &FileSystemReference = Filesystem::Filesystem::Instance();
        FileSystemReference.mount(std::filesystem::current_path().string(), Filesystem::VirtualFilesystem::Type::directory, "Root");
        //FileSystemReference.list();
    }

    void onLeftClickCallback(beacon::sender *sender, beacon::args *args)
    {
        int32_t selectedPixel[3] = {-1, -1, -1};
        if (args == nullptr)
            return;

        auto obj = args->to<Input::MouseArgs>();
        if (obj.Left().State == Input::PRESSED && isMainWindowFocused)
        {

            auto is_valid = [](glm::vec4 vec)
            {
                return vec.x >= 0 && vec.x <= vec.z && vec.y >= 0 && vec.y <= vec.w;
            };

            if (PostProcess_ref != nullptr && is_valid(MousePosOnViewport))
            {
                PostProcess_ref->gBufferFramebuffer->bindFramebuffer();

                glReadBuffer(GL_COLOR_ATTACHMENT0 + PostProcess::GBUFFER_TARGETS::GBUFFER_ENTITY);
                glReadPixels(MousePosOnViewport.x, MousePosOnViewport.y, 1, 1, GL_RGB_INTEGER, GL_INT, &selectedPixel);
                //std::cout << selectedPixel << std::endl;
                sSelectedEntityIndex = selectedPixel[0];
                PostProcess_ref->gBufferFramebuffer->unbindFramebuffer();
            }
        }
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
            }
        }
        else if (status == Paused)
        {
            if (ImGui::Button(ICON_FA_PLAY, ImVec2(37, 37)))
            {

                status = Playing;
            }
        }
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_STOP, ImVec2(37, 37)))
        {
            status = Stopped;
        }
        ImGui::SameLine();

        ImGui::End();
    }

    static void MainWindow(std::vector<std::shared_ptr<EntityBase>> &EntityList)
    {

        static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

        bool opt_fullscreen = true;
        if (opt_fullscreen)
        {
            ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + toolbarSize));
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
                if (ImGui::MenuItem(ICON_FA_FILE " New"))
                {
                }
                if (ImGui::MenuItem(ICON_FA_FOLDER_OPEN " Open", "Ctrl+O"))
                {
                }
                if (ImGui::BeginMenu(ICON_FA_FOLDER_OPEN " Open Recent"))
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
                if (ImGui::MenuItem(ICON_FA_SAVE " Save", "Ctrl+S"))
                {
                }
                if (ImGui::MenuItem(ICON_FA_SAVE " Save As.."))
                {
                }
                if (ImGui::MenuItem("Add dummy object..."))
                {
                    {
                        glm::vec3 tPosition = glm::vec3(0, 0, 0);
                        glm::vec3 tScale = glm::vec3(1.0f);
                        glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
                        std::string tModelName = "models/artorias.eml";

                        std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

                        auto _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
                        _RComp->CastsShadows(true);
                        _RComp->setTransparency(false);
                        _RComp->setVisibility(true);
                        _RComp->setRenderId(EntityList.size());
                        //Component::Component_ptr _SComp = std::make_shared<Component::ScriptComponent>("scripts/test.lua");
                        _Entity->addComponent(_RComp);

                        _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));
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

                if (ImGui::MenuItem(ICON_FA_SIGN_OUT_ALT "Quit", "Alt+F4", &g_Running))
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
                ImGui::Checkbox("Show overlay", &showOverlay);
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

        ToolbarUI();
    }

    enum ShaderAction
    {
        ShaderAction_Id,
        ShaderAction_Name,
        ShaderAction_Reload
    };

    void ShaderView(const std::map<std::string, Shader_ptr> &shaders)
    {

        enum ContentsType
        {
            CT_Text,
            CT_Button,
            CT_SmallButton,
            CT_FillButton,
            CT_Selectable,
            CT_SelectableSpanRow
        };
        static int contents_type = CT_SelectableSpanRow;
        ImGui::Begin("Shaders");

        // Update item list if we changed the number of items
        static ImVector<std::string> selection;
        static bool items_need_sort = false;

        static ImGuiTableFlags flags =
            ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti | ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody;

        const ImDrawList *parent_draw_list = ImGui::GetWindowDrawList();
        const int parent_draw_list_draw_cmd_count = parent_draw_list->CmdBuffer.Size;
        ImVec2 table_scroll_cur, table_scroll_max; // For debug display
        const ImDrawList *table_draw_list = NULL;  // "
        const int TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        static ImVec2 outer_size_value = ImVec2(0.0f, TEXT_BASE_HEIGHT * 12);
        static float row_min_height = 20.0f;         // Auto
        static float inner_width_with_scroll = 0.0f; // Auto-extend
        static bool outer_size_enabled = true;
        static bool show_headers = true;
        static bool show_wrapped_text = false;
        // Submit table
        const float inner_width_to_use = (flags & ImGuiTableFlags_ScrollX) ? inner_width_with_scroll : 0.0f;
        if (ImGui::BeginTable("table_advanced", 3, flags, outer_size_enabled ? outer_size_value : ImVec2(0, 0), inner_width_to_use))
        {
            // Declare columns
            // We use the "user_id" parameter of TableSetupColumn() to specify a user id that will be stored in the sort specifications.
            // This is so our sort function can identify a column given our own identifier. We could also identify them based on their index!
            ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoHide, 0.0f, ShaderAction_Id);
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 0.0f, ShaderAction_Name);
            ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_WidthFixed, 0.0f, ShaderAction_Reload);
            ImGui::TableSetupScrollFreeze(0, 0);

            // Sort our data if sort specs have been changed!
            /* ImGuiTableSortSpecs *sorts_specs = ImGui::TableGetSortSpecs();
            if (sorts_specs && sorts_specs->SpecsDirty)
                items_need_sort = true;
            if (sorts_specs && items_need_sort && items.Size > 1)
            {
                MyItem::s_current_sort_specs = sorts_specs; // Store in variable accessible by the sort function.
                qsort(&items[0], (size_t)items.Size, sizeof(items[0]), MyItem::CompareWithSortSpecs);
                MyItem::s_current_sort_specs = NULL;
                sorts_specs->SpecsDirty = false;
            }
            items_need_sort = false;*/

            // Take note of whether we are currently sorting based on the Quantity field,
            // we will use this to trigger sorting when we know the data of this column has been modified.

            // Show headers
            if (show_headers)
                ImGui::TableHeadersRow();

            // Show data
            // FIXME-TABLE FIXME-NAV: How we can get decent up/down even though we have the buttons here?
            ImGui::PushButtonRepeat(true);
            auto iter = shaders.begin();
#if 1
            // Demonstrate using clipper for large vertical lists
            ImGuiListClipper clipper;
            clipper.Begin(shaders.size());
            while (clipper.Step())
            {
                for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++, iter++)
#else
            // Without clipper
            {
                for (int row_n = 0; row_n < items.Size; row_n++, iter++)
#endif
                {
                    Shader_ptr item = iter->second;
                    //if (!filter.PassFilter(item->Name))
                    //    continue;

                    const bool item_is_selected = selection.contains(iter->first);
                    ImGui::PushID(iter->first.c_str());
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, row_min_height);

                    // For the demo purpose we can select among different type of items submitted in the first column
                    ImGui::TableSetColumnIndex(0);
                    char label[32];
                    sprintf(label, "%04d", iter->second->getProgramID());
                    if (contents_type == CT_Text)
                        ImGui::TextUnformatted(label);
                    else if (contents_type == CT_Button)
                        ImGui::Button(label);
                    else if (contents_type == CT_SmallButton)
                        ImGui::SmallButton(label);
                    else if (contents_type == CT_FillButton)
                        ImGui::Button(label, ImVec2(-FLT_MIN, 0.0f));
                    else if (contents_type == CT_Selectable || contents_type == CT_SelectableSpanRow)
                    {
                        ImGuiSelectableFlags selectable_flags = (contents_type == CT_SelectableSpanRow) ? ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap : ImGuiSelectableFlags_None;
                        if (ImGui::Selectable(label, item_is_selected, selectable_flags, ImVec2(0, row_min_height)))
                        {
                            if (ImGui::GetIO().KeyCtrl)
                            {
                                if (item_is_selected)
                                    selection.find_erase_unsorted(iter->first);
                                else
                                    selection.push_back(iter->first);
                            }
                            else
                            {
                                selection.clear();
                                selection.push_back(iter->first);
                            }
                        }
                    }

                    if (ImGui::TableSetColumnIndex(1))
                        ImGui::TextUnformatted(iter->first.c_str());

                    // Here we demonstrate marking our data set as needing to be sorted again if we modified a quantity,
                    // and we are currently sorting on the column showing the Quantity.
                    // To avoid triggering a sort while holding the button, we only trigger it when the button has been released.
                    // You will probably need a more advanced system in your code if you want to automatically sort when a specific entry changes.
                    if (ImGui::TableSetColumnIndex(2))
                    {
                        if (ImGui::Button(ICON_FA_SYNC ""))
                        {
                            item->Reload();
                        }
                        if (ImGui::IsItemDeactivated())
                        {
                            items_need_sort = true;
                        }
                    }

                    ImGui::PopID();
                }
            }
            ImGui::PopButtonRepeat();

            // Store some info to display debug details below
            table_scroll_cur = ImVec2(ImGui::GetScrollX(), ImGui::GetScrollY());
            table_scroll_max = ImVec2(ImGui::GetScrollMaxX(), ImGui::GetScrollMaxY());
            table_draw_list = ImGui::GetWindowDrawList();
            ImGui::EndTable();
        }

        ImGui::End();
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

            static const char *items[] = {"gBuffer Normal", "Light Pass", "TAA", "SSGI", "Screen Space Reflections", "Global Illumination", "Entity"};
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
            else if (item_current == "SSGI")
                TextureId = PP->HBILFramebuffer->getRenderTargetHandler(0);
            else if (item_current == "Global Illumination")
                TextureId = PP->gBufferFramebuffer->getRenderTargetHandler(PostProcess::GBUFFER_TARGETS::GBUFFER_GI);
            else if (item_current == "Entity")
                TextureId = PP->gBufferFramebuffer->getRenderTargetHandler(PostProcess::GBUFFER_TARGETS::GBUFFER_ENTITY);

            //we are done working with this window
            unsigned int fb_size[3] = {(unsigned int)*WIDTH, (unsigned int)*HEIGHT, GammaShader->getProgramID()};

            //ImGui::GetWindowDrawList()->AddCallback([]() { }, nullptr);
            ImGui::Image((void *)TextureId, ImVec2(mImguiRenderWindow.x, mImguiRenderWindow.y), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

            //ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
            ImGui::End();
        }
    }

    static void SkySettings(std::shared_ptr<Skybox> skybox, std::shared_ptr<Sun> sun)
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

        ImGui::Separator();

        ImGui::Text("Sun");
        float pos[3] = {sun->Position.x, sun->Position.y, sun->Position.z};
        ImGui::DragScalarN("Position###sunposition", ImGuiDataType_Float, pos, 3);
        sun->Position = glm::vec3(pos[0], pos[1], pos[2]);
        ImGui::DragFloat("Radius", &sun->radius);
        /*
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

        // Make a local copy to avoid modifying actual backend flags.
        ImGuiBackendFlags backend_flags = io.BackendFlags;
        ImGui::CheckboxFlags("io.BackendFlags: HasGamepad", &backend_flags, ImGuiBackendFlags_HasGamepad);
        ImGui::CheckboxFlags("io.BackendFlags: HasMouseCursors", &backend_flags, ImGuiBackendFlags_HasMouseCursors);
        ImGui::CheckboxFlags("io.BackendFlags: HasSetMousePos", &backend_flags, ImGuiBackendFlags_HasSetMousePos);
        ImGui::CheckboxFlags("io.BackendFlags: PlatformHasViewports", &backend_flags, ImGuiBackendFlags_PlatformHasViewports);
        ImGui::CheckboxFlags("io.BackendFlags: HasMouseHoveredViewport", &backend_flags, ImGuiBackendFlags_HasMouseHoveredViewport);
        ImGui::CheckboxFlags("io.BackendFlags: RendererHasVtxOffset", &backend_flags, ImGuiBackendFlags_RendererHasVtxOffset);
        ImGui::CheckboxFlags("io.BackendFlags: RendererHasViewports", &backend_flags, ImGuiBackendFlags_RendererHasViewports);
*/
        ImGui::End();
    }
    
    /* static void AudioControl(std::shared_ptr<::Epsilon::IO::Audio::Audio> AudioSystem)
    {
        ImGui::Begin("Audio System");
        static int current_device;
        ImGui::Combo("Current device", );

        ImGui::End();
    }*/

    static void MaterialEditor()
    {
        using namespace Component;
        if (sSelectedEntity->HasRenderComponent())
        {

            auto RComponent = std::static_pointer_cast<RenderComponent>(sSelectedEntity->getComponent<RenderComponent_ptr>());
            auto &ref = ResourceManager::Get();
            auto model = ref.getModel(RComponent->getModelPath());
            auto &meshes = model->Meshes();

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
            ImGui::SliderFloat("Feedback Min", &PostprocessData.AntiAliasingSettings.FeedbackMin, 0.0, 1.0);
            ImGui::SliderFloat("Feedback Max", &PostprocessData.AntiAliasingSettings.FeedbackMax, 0.0, 1.0);
        }

        ImGui::End();
    }

    static void EntityProperties(std::shared_ptr<Camera> camera)
    {

        using namespace Component;
        ImGui::Begin(ICON_FA_SLIDERS_H " Properties");
        ImGui::BeginChild("btn_add_component", ImVec2(0,35), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
        ImGui::Button(ICON_FA_PLUS " Add Component", ImVec2(ImGui::GetContentRegionAvail().x, 32));
        ImGui::EndChild();
        ImGui::Separator();

        ImGui::BeginChild("Collection", ImVec2(0,0), false);
        if (sSelectedEntity != nullptr)
        {
            if (ImGui::CollapsingHeader(ICON_FA_ARROWS_ALT " Transform Component", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto position = sSelectedEntity->Transform->Position();
                auto scale = sSelectedEntity->Transform->Scale();
                auto rotation = sSelectedEntity->Transform->Rotation();
                auto vec_rot = glm::eulerAngles(rotation);

                float p[3] = {position.x, position.y, position.z};
                float s[3] = {scale.x, scale.y, scale.z};
                float r[3] = {vec_rot.x, vec_rot.y, vec_rot.z};

                if (ImGui::DragFloat3("Position##1", p))
                {
                    sSelectedEntity->Transform->Position(glm::vec3(p[0], p[1], p[2]), true);
                }
                if (ImGui::DragFloat3("Scale##1", s))
                {
                    sSelectedEntity->Transform->Scale(glm::vec3(s[0], s[1], s[2]), true);
                }
                if (ImGui::DragFloat3("Rotation##1", r, 0.05))
                {
                    sSelectedEntity->Transform->Rotation(glm::quat(glm::vec3(r[0], r[1], r[2])), true);
                }
            }
            if (sSelectedEntity->HasPhysicComponent())
            {
                if (ImGui::CollapsingHeader(ICON_FA_ATOM " Physics Component", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto PComponent = std::static_pointer_cast<PhysicComponent>(sSelectedEntity->getComponent<PhysicComponent_ptr>());
                    auto p = PComponent->getPosition();

                    float tmpppos[3] = {p.x, p.y, p.z};

                    bool initialState = PComponent->isActive;
                    bool is_static = PComponent->isStatic();
                    float &mass = PComponent->mMass;
                    auto type = PComponent->getTypeStr();

                    ImGui::Checkbox("Active", &PComponent->isActive);
                    ImGui::Checkbox("Static", &is_static);
                    ImGui::DragFloat("Mass", &mass);
                    ImGui::Text("Collision Type: %s", type);
                    PComponent->setState(PComponent->isActive);
                }
            }
            if (sSelectedEntity->HasClothComponent())
            {
                if (ImGui::CollapsingHeader(ICON_FA_TSHIRT " Cloth Component", ImGuiTreeNodeFlags_DefaultOpen))
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

                    ImGui::DragFloat3("Position##3", tmpppos, 1.0, -30.0, 30.0);
                    node0.m_x = btVector3(tmpppos[0], tmpppos[1], tmpppos[2]);
                    node1.m_x = btVector3(tmpppos[0], tmpppos[1], tmpppos[2]) + diff;
                }
            }
            if (sSelectedEntity->HasRenderComponent())
            {
                if (ImGui::CollapsingHeader(ICON_FA_VECTOR_SQUARE " Render Component", ImGuiTreeNodeFlags_DefaultOpen))
                {
                    auto RComponent = std::static_pointer_cast<RenderComponent>(sSelectedEntity->getComponent<RenderComponent_ptr>());
                    bool &shadows = RComponent->mCastsShadows;
                    const std::string &modelPath = RComponent->getModelPath();
                    bool &transparent = RComponent->isTransparent;
                    bool &visible = RComponent->isVisible;
                    bool &double_faced = RComponent->mIsDoubleFaced;
                    //auto p = RComponent->getPosition();
                    //float tmpppos[3] = {p.x, p.y, p.z};
                    //ImGui::DragFloat3("Position##4", tmpppos, 1.0, -30.0, 30.0);
                    static int buffer_size = modelPath.size();
                    char *buffer = (char *)modelPath.c_str();

                    ImGui::InputText("###Model path", buffer, buffer_size, ImGuiInputTextFlags_ReadOnly);
                    ImGui::SameLine();
                    ImGui::SmallButton(ICON_FA_FOLDER_OPEN);

                    ImGui::Checkbox("Casts Shadows", &shadows);
                    ImGui::Checkbox("Transparent", &transparent);
                    ImGui::Checkbox("Visible", &visible);
                    ImGui::Checkbox("Double Faced", &double_faced);
                    //p = glm::vec3(tmpppos[0], tmpppos[1], tmpppos[2]);
                    //RComponent->setPosition(p);
                }
            }
        }
        else if (camera_selected)
        {
            auto fov = camera->getFoV();
            auto direction = camera->getDirection();
            auto position = camera->getPosition();

            float p[3] = {position.x, position.y, position.z};
            float d[3] = {direction.x, direction.y, direction.z};

            if (ImGui::CollapsingHeader(ICON_FA_CAMERA " Camera", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::DragFloat3("Position###camerapos", p);
                ImGui::SliderFloat3("Direction###cameradir", d, -1.0f, 1.0f);
                ImGui::SliderFloat("Fov###camerafov", &fov, 10.0f, 100.0f);
            }
            camera->setFoV(fov);
            camera->setDirection(glm::vec3(d[0], d[1], d[2]));
            camera->setPosition(glm::vec3(p[0], p[1], p[2]));
        }
        else if (lightSelected)
        {
            bool any_change = false;

            if (ImGui::CollapsingHeader(ICON_FA_LIGHTBULB " Light", ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto &light = PostProcess_ref->getLight(sSelectedLight);
                float col[3] = {light.color.x, light.color.y, light.color.z};
                if (ImGui::ColorEdit3("Color", col))
                {
                    any_change |= true;
                    light.color = glm::vec4(col[0], col[1], col[2], 1.0);
                }

                float pos[3] = {light.position.x, light.position.y, light.position.z};
                if (ImGui::DragFloat3("Position", pos, 0.05))
                {
                    any_change |= true;
                    light.position = glm::vec4(pos[0], pos[1], pos[2], 1.0);
                }

                float dir[3] = {light.direction.x, light.direction.y, light.direction.z};
                if (ImGui::DragFloat3("Direction", dir, 0.05))
                {
                    any_change |= true;
                    light.direction = glm::vec4(dir[0], dir[1], dir[2], 1.0);
                }

                float radius = light.radius;
                if (ImGui::DragFloat("Radius", &radius, 0.05f))
                {
                    any_change |= true;
                    light.radius = radius;
                }

                float watts = light.watts;
                if (ImGui::DragFloat("Watts", &watts, 0.05f))
                {
                    any_change |= true;
                    light.watts = watts;
                }

                if (any_change)
                {
                    PostProcess_ref->updateLight(sSelectedLight, light);
                }
            }
        }
        ImGui::EndChild();
        ImGui::End();
    }

    static void InfoOverlay(bool *p_open, ImVec2 pos, ImVec2 size, std::shared_ptr<Camera> camera)
    {
        const float PADX = 15.0f;
        const float PADY = 40.0f;
        static int corner = 1;
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
        if (ImGui::Begin("Overlay", p_open, window_flags))
        {
            ImGui::Text("Viewport Info");
            ImGui::Separator();
            ImGui::Text("Window size: %.0f x %.0f", size.x, size.y);
            ImGui::Text("View position: x=%.2f, y=%.2f, z=%.2f", camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
            ImGui::Text("frametime/fps: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Allocated Texture Space: ~ %d Mb", ResourceManager::Get().getTextureBytesAllocation() / (1000 * 1000));
            if (ImGui::IsMousePosValid())
                ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
            else
                ImGui::Text("Mouse Position: <invalid>");
            ImGui::Text("Mouse Position in Viewport: (%.1f,%.1f)", MousePosOnViewport.x, MousePosOnViewport.y);
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

    static void DrawAddCombo()
    {
        static std::string options[] = {ICON_FA_DRAW_POLYGON " Mesh", ICON_FA_SHAPES " Primitive", ICON_FA_LIGHTBULB " Light", ICON_FA_CAMERA " Camera"};

        static std::string options_mesh[] = {ICON_FA_DRAW_POLYGON " Triangle", ICON_FA_CUBES " SDF"};
        static std::string options_primitive[] = {ICON_FA_CUBE " Cube", ICON_FA_GLOBE " Sphere", ICON_FA_DICE_D20 " Ico-Sphere", ICON_FA_RING " Torus", ICON_FA_DATABASE " Cylinder"};
        static std::string options_light[] = {ICON_FA_CERTIFICATE " Point", ICON_FA_SUN " Directional", ICON_FA_CIRCLE " Sphere", "Tube", ICON_FA_SQUARE " Quad"};
        static std::string options_camera[] = {ICON_FA_VIDEO " Perspective", ICON_FA_BORDER_ALL " Orthogonal"};
        if (ImGui::Button(ICON_FA_PLUS " Add..."))
            ImGui::OpenPopup("my_toggle_popup");
        if (ImGui::BeginPopup("my_toggle_popup"))
        {
            for (int i = 0; i < IM_ARRAYSIZE(options); i++)
                if (ImGui::BeginMenu(options[i].c_str()))
                {
                    std::string *tmp_str_array;
                    int num_items = 0;
                    switch (i)
                    {
                    case 0:
                        tmp_str_array = options_mesh;
                        num_items = 2;
                        break;
                    case 1:
                        tmp_str_array = options_primitive;
                        num_items = 5;
                        break;
                    case 2:
                        tmp_str_array = options_light;
                        num_items = 4;
                        break;
                    case 3:
                        tmp_str_array = options_camera;
                        num_items = 2;
                        break;
                    }
                    for (int j = 0; j < num_items; j++)
                    {
                        if (ImGui::MenuItem(tmp_str_array[j].c_str()))
                        {
                            if (tmp_str_array[j] == options_light[0])
                            { //Point light
                                PostProcess_ref->addLight(glm::vec3(0.0), glm::vec3(0.0), 0);
                            }
                            else if (tmp_str_array[j] == options_light[2])
                            {
                                PostProcess_ref->addLight(glm::vec3(0.0), glm::vec3(0.0), 2);
                            }
                            else
                            {
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
            ImGui::EndPopup();
        }
    }

    static void MainViewport(std::shared_ptr<OpenGL::FrameBuffer<int>> framebuffer, std::shared_ptr<Camera> camera)
    {
        ImGui::Begin("Main Render");
        isMainWindowFocused = ImGui::IsWindowFocused();
        ImVec2 window_size = ImGui::GetContentRegionAvail();
        ImVec2 window_pos = ImGui::GetWindowPos();
        mLastImguiRenderWindow = mImguiRenderWindow;
        mImguiRenderWindow = glm::ivec2(window_size.x, window_size.y);

        mShouldResize = mImguiRenderWindow != mLastImguiRenderWindow;

        auto TextureId = framebuffer->getRenderTargetHandler(0);

        auto cursor_pos = ImGui::GetCursorPos();

        ImGuiIO &io = ImGui::GetIO();
        auto mousePos = io.MousePos;
        MousePosOnViewport = glm::vec4((mousePos.x - window_pos.x) - 8.0, (mousePos.y - window_pos.y) - 30.0, window_size.x, window_size.y - 5.0);
        MousePosOnViewport.y = (window_size.y) - MousePosOnViewport.y;
        //MousePosOnViewport = glm::clamp(MousePosOnViewport, glm::vec2(0.0f), glm::vec2(window_size.x, window_size.y));

        if (showOverlay)
            InfoOverlay(&showOverlay, window_pos, window_size, camera);

        ImGui::Image((void *)TextureId, ImVec2(window_size.x, window_size.y), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

        cursor_pos = ImVec2(cursor_pos.x + 20.0f, cursor_pos.y + 20.0f);
        ImGui::SetCursorPos(cursor_pos);
        DrawAddCombo();

        ImGui::End();
    }

    static void TextureList()
    {
        static int imgSize = 64;

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
        ImGui::BeginChild("btn_add_entity", ImVec2(0,35), false, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar);
        ImGui::Button(ICON_FA_PLUS " Add Entity", ImVec2(ImGui::GetContentRegionAvail().x, 32));
        ImGui::EndChild();
        ImGui::Separator();
        ImGui::BeginChild("Collection", ImVec2(0,0), false);
        if (ImGui::TreeNode(ICON_FA_DICE_D6 " Entities"))
        {
            int index = 0;
            for (const auto &ent : EntityList)
            {

                ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                const bool is_selected = ent == sSelectedEntity || sSelectedEntityIndex == index;
                if (is_selected)
                    node_flags |= ImGuiTreeNodeFlags_Selected;
                bool is_open = ImGui::TreeNodeEx((ent->getName() + std::string("###") + ent->getHash() + ent->getName()).c_str(), node_flags);
                if (is_open)
                {
                    if (ImGui::IsItemClicked(0))
                    {
                        sSelectedEntity = ent;
                        sSelectedEntityIndex = index;
                        camera_selected = false;
                        lightSelected = false;
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
                index++;
            }

            if (ImGui::TreeNode("Camera"))
            {
                if (ImGui::IsItemClicked(0))
                {
                    sSelectedEntity = nullptr;
                    camera_selected = true;
                    sSelectedEntityIndex = -1;
                    lightSelected = false;
                }
                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Lights"))
            {
                if (PostProcess_ref != nullptr)
                {
                    int index = 0;
                    for (auto &light : PostProcess_ref->getLights())
                    {
                        if (ImGui::TreeNode(("Light" + std::to_string(index)).c_str()))
                        {
                            if (ImGui::IsItemClicked(0))
                            {
                                sSelectedLight = index;
                                sSelectedEntity = nullptr;
                                sSelectedEntityIndex = -1;
                                camera_selected = false;
                                lightSelected = true;
                            }
                            ImGui::TreePop();
                        }
                        index++;
                    }
                }
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
        ImGui::EndChild();
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
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow *backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

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

    static void DrawDirectoryOpen(const string &dir, const Filesystem::File &f)
    {
        //ImGui::Indent();

        /*for (const auto &file : fs)
        {
            ImGuiTreeNodeFlags node_flags;
            std::string name;
            if (file.type() == Filesystem::File::Type::directory)
            {
                node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
                name = ICON_FA_FOLDER + file.Name();
            }
            else
            {
                node_flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf;
                name = file.Name();
            }
            if (ImGui::TreeNodeEx((name).c_str(), node_flags))
            {
                //std::cout << "Opening: " << file.AbsolutePath() << std::endl;
                if (file.type() == Filesystem::File::Type::directory)
                {
                    auto lst = FileSystemReference.list();
                    auto fs = lst.second;

                    DrawDirectoryOpen(file.AbsolutePath());
                }

                ImGui::TreePop();
            }
        }*/
        /***************************/

        ImGuiTreeNodeFlags node_flags;
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        const bool is_folder = f.type() == Filesystem::File::Type::directory;

        auto &FileSystemReference = Filesystem::Filesystem::Instance();

        if (is_folder)
        {
            //std::cout << f.Name() << "is a dir" << std::endl;
            std::string name = ICON_FA_FOLDER " " + f.Name();
            bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanFullWidth);
            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Folder");
            if (open)
            {
                auto lst = FileSystemReference.list(dir);
                auto fs = lst.second;
                for (const auto &file : fs)
                {
                    //std::cout << file.AbsolutePath() << std::endl;
                    DrawDirectoryOpen(file.AbsolutePath(), file);
                }
                ImGui::TreePop();
            }
        }
        else
        {
            std::string name = f.Name();
            ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth);
            ImGui::TableNextColumn();
            ImGui::Text("%d", f.Size());
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("File");
        }

        /******************************/

        //ImGui::Unindent();
    }

    static void FileSystemWindow()
    {
        TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
        TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
        ImGui::Begin("Filesystem");

        static float w = 400.0f;
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        ImGui::BeginChild("Tree", ImVec2(w, 0), true);

        auto &FileSystemReference = Filesystem::Filesystem::Instance();
        auto fs = FileSystemReference.getFilesystem("Root");

        static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;

        if (ImGui::BeginTable("Mounted Filesystems", 3, flags))
        {
            // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 12.0f);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, TEXT_BASE_WIDTH * 18.0f);
            ImGui::TableHeadersRow();
            Filesystem::File top_dir(std::to_string(fs->Id().get()), Filesystem::File::Type::directory);
            //std::cout << fs->mount_point().AbsolutePath << std::endl;
            DrawDirectoryOpen(fs->mount_point().AbsolutePath, top_dir);

            ImGui::EndTable();
        }
        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, ImGui::GetContentRegionAvail().y));
        if (ImGui::IsItemActive())
            w += ImGui::GetIO().MouseDelta.x;
        ImGui::SameLine();
        ImGui::BeginChild("Directory", ImVec2(0, 0), true);

        static int imgSize = 64;

        int numChoices = 4;

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

        int numHorizontalImages = glm::max(((int)window_width / (int)(imgSize + (style->ItemSpacing.x * 3))), 1);

        const auto &TextureArray = ResourceManager::Instance().getTextureArray();

        ImGui::Columns(numHorizontalImages, NULL);
        ImGui::Separator();

        int counter = 0;
        for (auto &texture : TextureArray)
        {

            if (counter < 0 && counter % glm::max(numHorizontalImages, 1) == 0)
                ImGui::Separator();

            auto textureId = texture.second->ID();
            ImGui::ImageButton((void *)(unsigned int)textureId, ImVec2(imgSize - 8, imgSize - 8), ImVec2(0, 0), ImVec2(1, 1), 8, ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::Text(texture.first.c_str());
            ImGui::NextColumn();
            counter++;
        }

        ImGui::Columns(1);
        ImGui::Separator();

        ImGui::EndChild();
        /*
        auto &lst = FileSystemReference.list(".");
        auto &fs = lst.second;
        for (const auto &file : lst.second)
        {
            ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            //const bool is_selected = ent == sSelectedEntity || sSelectedEntityIndex == index;
            //if (is_selected)
            //    node_flags |= ImGuiTreeNodeFlags_Selected;
            bool is_open = ImGui::TreeNodeEx((file.Name()).c_str(), node_flags);
            if (is_open)
            {
                auto &lst = FileSystemReference.list();
                auto &fs = lst.second;
            }
            ImGui::TreePop();

        }*/
        ImGui::PopStyleVar();
        ImGui::End();
    }
}