///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef EPSILON_H_INCLUDED
#define EPSILON_H_INCLUDED

#include <pch.hpp>

#include "App.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Texture.h>
#include <Shader.h>
#include <Skybox.h>
#include <camera.h>
#include <Renderer/Model.h>
#include <Text.h>
#include <Renderer/Terrain.h>
#include <Grass.h>
#include <PostProcess.h>
#include <Water.hpp>
#include <Sun.h>
#include <BSP.h>
#include <MD5_Model.h>
#include <MD5_Anim.h>
#include <Renderer/Shadows/ShadowMapping.h>
#include <Entity/Entity.h>
#include <Resource/ResourceManager.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <Player.h>
//#include <Audio/Audio.h>
//#include <Audio/AudioElement.h>
//#include <Audio/AudioListener.h>
#include <SplashScreen.h>
#include <SphericalHarmonics.h>
#include <Picking.h>
#include <Log.h>
#include <System/filesystem.h>
#include <cubemapRenderer.h>
#include <CubeMap.h>
#include <CommandFunctions.h>
#include <Physics/ClothPhysicObject.h>
#include <Renderer/Patch.h>
#include <Renderer/Shadows/PointShadow.hpp>
#include <Driver/API/BufferObject.hpp>
#include <Driver/API/OpenGL/UniformBuffer.hpp>
#include <Renderer/ShaderStorage.hpp>
#include <Platform/WindowBase.hpp>

#include <ParticleSystem.h>
#include <Driver/API/OpenGL/FrameBuffer.h>

#include <imgui_imp/imgui_impl_glfw.h>
#include <imgui_imp/imgui_impl_opengl3.h>
#include <imgui_imp/imgui_menubar.hpp>
#include <imgui_imp/imgui_Init.hpp>

#include <Renderer/Gizmos.hpp>
#include <Renderer/Primitives/Sphere.hpp>
#include <Renderer/Primitives/Cube.hpp>
#include <Editor/Editor.hpp>
#include <Renderer/Primitives/Bezier.hpp>
#include <Renderer/Primitives/CubicBezier.hpp>

namespace Epsilon
{
    
    class Epsilon : public App
    {
    public:
        Epsilon(const char* name);

        virtual ~Epsilon()
        {
        }

    private:
        void LoadGeometry(void);

        void LoadSound(void);

        void LoadShaders(void);

        void RenderSplashScreen(std::string);

        void PollEvents(void);

        void Render3D(std::shared_ptr<Shader>);

        void RenderShadows(std::shared_ptr<Shader>);

        void Render2D(void);

        void ProcessAudio(void);

        void RenderLegacy(void);

        void RenderSkybox(bool);

        void ComputeWater(void);

        void SwapBuffers(void);

        void ClearBuffers(void);

        void ComputeCamera(CAMERA_MODE, glm::vec3 position = glm::vec3(0.0), glm::vec3 direction = glm::vec3(0.0), glm::mat4 proj = glm::mat4(0), glm::mat4 view = glm::mat4(0));

        void RenderFrame(void);

        void ProcessFrame(void);

        void SetUniforms(std::shared_ptr<Shader>, glm::vec3 position, glm::vec3 scale, glm::quat rotation);

        void Clock(void);

        void ComputeShadow();

        void RenderToCubemaps();

        void RenderParticles();

        void CalculateVisibility();

        void RenderGlobalIllumination();

    public:
        void onRender() override;

        void onCreate() override;

        void onExit() override;

    public:
        std::vector<std::shared_ptr<Camera>> eCamera;
        const int mCurrentCamera = 0;
        std::shared_ptr<Patch> mPatch;
        /**Shaders**/
        GLuint cubemapTex = 0;
        GLuint cubemapDepthTex = 0;
        std::map<string, std::shared_ptr<Shader>> Shaders;
        std::shared_ptr<Skybox> skybox;
        std::shared_ptr<Water> waterPlane;
        std::vector<Grass> grass;
        std::shared_ptr<Platform::WindowBase> window = nullptr;
        std::shared_ptr<Text> text;
        //std::shared_ptr<Text> text2;
        std::shared_ptr<Terrain> terrain;
        std::shared_ptr<Sun> sun;
        std::shared_ptr<CQuake3BSP> BSPMap;
        std::shared_ptr<MD5Model> m_AnimModel;
        std::shared_ptr<Renderer::ShadowMap> shadowMap;
        std::shared_ptr<Renderer::PointShadow> mPointShadow;
        std::shared_ptr<PostProcess> PP;
        std::shared_ptr<Renderer::CubicBezier> mBezierCurve;

        glm::vec3 ambientDivider;
        std::vector<std::shared_ptr<EntityBase>> EntityList;
        //std::shared_ptr<IO::Audio::Audio> m_AudioSystem;
        //std::shared_ptr<IO::Audio::AudioListener> m_AudioListener;
        std::shared_ptr<SphericalHarmonics> sphericalharmonics;
        std::thread t1;
        std::shared_ptr<eTexture> tex;
        // std::shared_ptr<Pick> m_Pick;
        CAMERA_MODE m_CameraMode;
        std::vector<std::shared_ptr<CubeMap>> m_Cubemaps;
        std::shared_ptr<CubeMap> mCubemap[7][7][7];

        std::shared_ptr<Player> m_PlayerCapsule;
        std::shared_ptr<ParticleSystem> m_ParticleSystem;
        //std::shared_ptr<GUI> m_GUI;
        /*std::shared_ptr<Panel> m_Panel;
        std::shared_ptr<Button> t_ButtonSettings;
        std::shared_ptr<Panel> t_PanelSettings;
        std::shared_ptr<Button> t_ButtonResume;
        std::shared_ptr<CheckBox> t_CheckBox;*/
        bool ParallaxOn;
        /** Window Properties **/
        float xz[5][5];
        short WIDTH = 16;
        short HEIGHT = 16;
        bool SSAO = false;
        double timeBehind = 0.0;
        float timeGUI = 0.0;
        Editor::Editor editor;

        std::shared_ptr<Renderer::ShaderStorage> AmbientLightSSBO;
        std::shared_ptr<API::BufferObject> mGlobalUniformBuffer;
        std::shared_ptr<API::BufferObject> mCameraData;

        std::shared_ptr<Renderer::Gizmo> mGizmo;

    private:
        bool normal = 0;
        bool flashLight = 0;
        bool hdr = true;
        double lastTime = 0;
        double frametime = 0;
        double etime = 0;
        float windSpeed = 0.0;
        float exposure = 0;
        bool parallax = false;
        float eventtime = 0;
        short fps = 0;
        int FrameCount = 0;
        int FramesSinceLastUIUpdate = 0;
        bool showtext = false;
        bool onMenu = false;
        float menuTime = 0.0;
        glm::vec3 mLastCameraDirection;
        glm::vec3 mLastCameraPosition;
        //vector<glm::vec3> lightPositions;
        std::vector<glm::vec3> grassPos;
        std::ostringstream fpss;
        std::shared_ptr<Physics::SpherePhysicObject> ph3;
        std::string GL_VER, GL_REN, GL_VEN;
        double m_TextAcum = 0.0;
        CFrustum Frustum;
        std::shared_ptr<OpenGL::FrameBuffer<int>> mDefaultFrameBuffer;
        std::shared_ptr<Renderer::Sphere> mSphere;
        std::shared_ptr<Renderer::Cube> mCube;

        //IMGUI variables

        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        float KeyTime[1024] = {0.0f};

        glm::ivec2 CursorBeforeMenu;
    };
}
#endif /// EPSILON_H_INCLUDED
