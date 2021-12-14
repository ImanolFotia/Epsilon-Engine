#pragma once

#include <pch.hpp>

#include <PostProcess.h>
#include <Renderer/Gizmos.hpp>
#include <Skybox.h>
#include <Sun.h>
#include <GLFW/glfw3.h>

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
#include <System/CPU.hpp>
#include <System/GPU.hpp>
#include <System/Memory.hpp>

#include <IO/Mouse.h>
#include <IO/KeyBoard.h>
#include <beacon/beacon.hpp>

#include "ImPlot/implot.h"



/*#include <Audio/Audio.h>
#include <Audio/AudioElement.h>
#include <Audio/AudioListener.h>*/

namespace Epsilon::Editor
{
    class Editor
    {
        public:
        enum STATUS
        {
            Playing,
            Paused,
            Stopped
        };

        template <typename T>
        inline T RandomRange(T min, T max)
        {
            T scale = rand() / (T)RAND_MAX;
            return min + scale * (max - min);
        }

        ImVec4 RandomColor()
        {
            ImVec4 col;
            col.x = RandomRange(0.0f, 1.0f);
            col.y = RandomRange(0.0f, 1.0f);
            col.z = RandomRange(0.0f, 1.0f);
            col.w = 1.0f;
            return col;
        }

        bool onMenu;

        bool show_app_metrics = false;
        bool show_app_style_editor = false;
        bool show_app_about = false;
        bool show_new_project_window = false;
        bool showSystemStatus = false;
        bool show_material_editor = false;
        bool add_new_entity_dialog = false;
        bool add_new_component_dialog = false;
        bool p_cross = false;
        STATUS status = Stopped;
        bool show_pipeline_viewer = false;
        bool camera_selected = false;
        std::shared_ptr<Camera> current_camera_viewport = nullptr;
        std::string filesystem_last_dir_clicked;
        //selection_t current_selection;

        Renderer::Gizmo::gizmo gizmo_status = Renderer::Gizmo::gizmo::translate;

        float menu_bar_height = 0;
        const float toolbarSize = 50;

        std::function<void()> mRenderGIFunction;
        bool mShouldRenewGI = false;
        bool render_gi = false;

        glm::ivec2 mImguiRenderWindow;
        glm::ivec2 mLastImguiRenderWindow;
        int32_t sSelectedLight = -1;

        glm::vec4 MousePosOnViewport;

        bool showOverlay = true;
        float TEXT_BASE_WIDTH;
        float TEXT_BASE_HEIGHT;

        std::shared_ptr<PostProcess> PostProcess_ref;

        System::CPU mCPU;
        System::Memory mMemory;
        std::shared_ptr<GPU> mGPU;

        ImVec4 CPU_Color = RandomColor();
        ImVec4 Framerate_Color = RandomColor();
        ImVec4 Frametime_Color = RandomColor();

        glm::vec3 SelectedEntityPosition;

        bool mShouldResize = false;
        bool mWaitingResize = false;
        bool lightSelected = false;
        bool isMainWindowFocused = false;

        bool textureListVisible = false;

        glm::vec3 ambientDivider;

        short *WIDTH;
        short *HEIGHT;

        std::shared_ptr<EntityBase> sSelectedEntity;
        int sSelectedEntityIndex = -1;
        std::vector<std::shared_ptr<EntityBase>> *localEntityList;
        
        enum ShaderAction
        {
            ShaderAction_Id,
            ShaderAction_Name,
            ShaderAction_Reload
        };

        void onLeftClickCallback(beacon::sender *sender, beacon::args *args);
        void onKeyPress(beacon::sender *sender, beacon::args *args);
        void onMouseWheelCallback(beacon::sender *sender, beacon::args *args);

        void Init(std::shared_ptr<Platform::WindowBase> window);
        void SparkLine(const char *id, const char *overlay, const float *values, int count, float min_v, float max_v, int offset, const ImVec4 &col, const ImVec2 &size);
        void SystemStatus();
        void ToolbarUI();
        void MainWindow(std::vector<std::shared_ptr<EntityBase>> &EntityList);

        void ShaderView(const std::map<std::string, Shader_ptr> &shaders);

        void DebugView(std::shared_ptr<PostProcess> PP, std::shared_ptr<OpenGL::FrameBuffer<int>> framebuffer, std::shared_ptr<Camera> camera, Shader_ptr GammaShader);

        void SkySettings(std::shared_ptr<Skybox> skybox, std::shared_ptr<Sun> sun);


        void MaterialEditor();

        void PostprocessSettings(std::shared_ptr<PostProcess> Postprocess_ref);

        void btn_add_component();

        void EntityProperties(std::shared_ptr<Camera> camera);

        void InfoOverlay(bool *p_open, ImVec2 pos, ImVec2 size, std::shared_ptr<Camera> camera);

        void DrawAddCombo();

        void MainViewport(std::shared_ptr<OpenGL::FrameBuffer<int>> framebuffer, std::shared_ptr<Camera> camera);

        void TextureList(std::pair<std::string, std::shared_ptr<Renderer::Texture2D>> *refTexture = nullptr);

        void add_entity_btn(std::vector<std::shared_ptr<EntityBase>> &EntityList);

        void SceneHierarchy(std::vector<std::shared_ptr<EntityBase>> &EntityList);

        void Render(std::shared_ptr<Platform::WindowBase> window);

        void PollEvents(std::shared_ptr<OpenGL::FrameBuffer<int>> framebuffer, std::shared_ptr<PostProcess> PP);

        void DrawDirectoryOpen(const std::string &dir, const Filesystem::File &f);

        void FileSystemWindow();
    };
}