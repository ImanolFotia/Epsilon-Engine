///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Core.hpp>

#include <pch.hpp>

#include <Epsilon.h>
#include <ProgramData.h>
#include <time.h>
#include <Helpers.hpp>
#include <Types.h>
#include <glm/gtc/quaternion.hpp>
#include <EpsilonMemory.h>
#include <sys/CPUID.h>
#include <Log.h>
#include <sys/Console.hpp>
#include <multithread/ThreadPool.hpp>
#include <Driver/API/OpenGL/GlCache.h>
#include <sys/GPU.hpp>
#include <Scripts/LuaScript.hpp>

#include <Driver/Audio/XAudio2/XAudio2.hpp>

#include <Engine.hpp>
#include <Clock.hpp>
#include <IO/File/Json.hpp>

GLenum glCache::CullFaceMode = 0;
GLuint glCache::ShaderId = 0;
unsigned glCache::DrawCalls = 0;

using namespace std::placeholders;
float mpos = -20.0;
float blend = 1.0f;

double Input::Mouse::XPOS = 500;
double Input::Mouse::YPOS = 500;

namespace Joystick = Input::Joystick;

std::unordered_map<unsigned, Joystick::JoystickManager::Joystick_ptr> Joystick::JoystickManager::JoystickVector;
Joystick::JoystickManager::Joystick_ptr Joystick::JoystickManager::dummyJoystick = std::make_shared<Input::Joystick::Joystick>();

bool Epsilon::CheckBox::_checked = true;
ThreadPool::ThreadPool_ptr ThreadPool::_instance = nullptr;

namespace Epsilon
{
    ResourceManager ResourceManager::instance;

    Epsilon::Epsilon(std::shared_ptr<Platform::WindowBase> win)
    {
        window = win;
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);
        GPU _gpu;

        Engine::Get().ContextType(API::CONTEXT_TYPE::OGL);

        cout << endl
             << endl
             << "Gathering Video Device information..." << endl;
        GL_VEN = _gpu.getVendor();
        GL_REN = _gpu.getDevice();
        GL_VER = _gpu.getDriverVersion();
        int max_tex_units = _gpu.getMaxtextureUnits();

        Console::Log("vendor", GL_VEN);
        std::cout << "OpenGL VENDOR: " << GL_VEN << std::endl;
        std::cout << "Video RENDERER: " << GL_REN << std::endl;
        std::cout << "OpenGL VERSION: " << GL_VER << std::endl;

        Log::WriteToLog(std::string("OpenGL VENDOR: " + GL_VEN));
        Log::WriteToLog(std::string("Video RENDERER: " + GL_REN));
        Log::WriteToLog(std::string("OpenGL VERSION: " + GL_VER));
        Console::Log("OpenGL MaxtextureUnits: ", std::to_string(max_tex_units));

        glEnable(GL_CLIP_PLANE0);
        glEnable(GL_CLIP_DISTANCE0);

        double plane[4] = {0.0, 5.0, 0.0, 15.0};
        glClipPlane(GL_CLIP_PLANE0, plane);

        tex = (std::shared_ptr<eTexture>)new eTexture("smoke.png");

        std::cout << "Clip Plane: " << (glIsEnabled(GL_CLIP_PLANE0) ? "Enabled" : "Disabled") << endl;

        CPUID cpu(0);
        cpu.printHardwareInformation();

        {
            glCache::glEnable(GL_DEPTH_TEST);

            glDepthFunc(GL_LESS);

            glCache::glEnable(GL_MULTISAMPLE);

            glCache::glEnable(GL_CULL_FACE);

            glShadeModel(GL_SMOOTH);

            glCache::glEnable(GL_BLEND);

            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }

        //Driver::Audio::XAudio2Driver xAudio2;
        //xAudio2.Init();

        normal = true;

        g_Running = true;

        flashLight = false;

        hdr = true;

        parallax = false;

        exposure = 3.0;

        this->frametime = 0.0;
        this->lastTime = 0.0;

        srand(time(NULL)); //

        m_CameraMode = NO_CLIP;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        const char *glsl_version = "#version 410 core";
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForOpenGL(window->getHandle()->getHandle(), true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        onMenu = false;
        window->HideCursor();

        mDefaultFrameBuffer = std::make_shared<OpenGL::FrameBuffer<int>>(window->getWindowData().Width, window->getWindowData().Height, false);
        mDefaultFrameBuffer->addRenderTarget(0, GL_RGB, GL_RGB, GL_NEAREST, GL_NEAREST, false);
        mDefaultFrameBuffer->FinishFrameBuffer();

        //Scripts::LuaScript _script;
        //_script.Load("scripts/Program.luac");
        //std::cout << "Sum is: " << _script.Exec<int>("sum", 3, 5) << std::endl;
    }

    void Epsilon::RenderSplashScreen(string text)
    {

        glClear(GL_COLOR_BUFFER_BIT);
        glViewport(0, 0, WIDTH, HEIGHT);
        Shader tmpShader("shaders/splashv.glsl", "shaders/splashf.glsl");
        eTexture texture("Survtech.png", GL_REPEAT, GL_TEXTURE_2D, 1);
        SplashScreen splsh;
        ClearBuffers();
        splsh.Draw(tmpShader.getProgramID(), texture.getTextureID());
        //this->text->RenderText(text,0.01, 0.83, 0.4, glm::vec3(1,1,1));

        //this->text->RenderText(std::string("#include <iostream>\nusing namespace std;\nint main(int argc, char** argv) {\n\tstd::cout << \"Hello, World!\" << std::endl;\n\treturn 0;\n}") ,0.01, 0.79, 0.5, glm::vec3(1,1,1));

        this->SwapBuffers();
        std::cout << "Splash llega" << std::endl;
        texture.Destroy();
    }

    void Epsilon::InitResources(void)
    {
        cout << "Initializing Resources..." << endl;

        ProgramData DATA;

        this->WIDTH = DATA.WINDOW_WIDTH;
        this->HEIGHT = DATA.WINDOW_HEIGHT;
        this->SSAO = DATA.HBAO;
        this->ParallaxOn = DATA.PARALLAX_OCLUSSION_MAPPING;
        /*
    text = (std::shared_ptr<Text>)new Text("resources/Roboto-Regular.ttf", DATA.WINDOW_WIDTH, DATA.WINDOW_HEIGHT);

    m_GUI = (std::shared_ptr<GUI>) new GUI(this->WIDTH, this->HEIGHT);
    std::shared_ptr<Container> t_Container = (std::shared_ptr<Container>) new Container();
    std::shared_ptr<Panel> t_Panel = (std::shared_ptr<Panel>) new Panel(this->WIDTH, this->HEIGHT, 0.3*WIDTH, 1.0*HEIGHT, glm::vec2(-0.7, 0.0));
    t_PanelSettings = (std::shared_ptr<Panel>) new Panel(this->WIDTH, this->HEIGHT, 0.5*WIDTH, 0.5*HEIGHT, glm::vec2(0.3, 0.0));

    std::shared_ptr<Button> t_ButtonQuit = (std::shared_ptr<Button>) new Button(0.15, 0.1, this->WIDTH, this->HEIGHT, "Quit");
    t_ButtonQuit->SizeX = 0.15;
    t_ButtonQuit->SizeY = 0.1;
    t_ButtonQuit->PositionX = -0.75;
    t_ButtonQuit->PositionY = -0.45;
    t_ButtonQuit->OnClickCallback(endgame);
    t_ButtonQuit->m_isHidden = false;
    t_ButtonQuit->m_TextRendererInstance = text;
    t_Container->addWidget(t_ButtonQuit);

    t_CheckBox = (std::shared_ptr<CheckBox>) new CheckBox(0.05, 0.05, this->WIDTH, this->HEIGHT, "Quit");
    t_CheckBox->SizeX = 0.5;
    t_CheckBox->SizeY = 0.5;
    t_CheckBox->PositionX = 0.0;
    t_CheckBox->PositionY = 0.0;
    t_CheckBox->m_isHidden = false;
    t_CheckBox->m_TextRendererInstance = text;
    t_CheckBox->setText("CheckBox");
    t_Container->addWidget(t_CheckBox);

    t_ButtonSettings = (std::shared_ptr<Button>) new Button(0.15, 0.1, this->WIDTH, this->HEIGHT, "Settings");
    t_ButtonSettings->SizeX = 0.15;
    t_ButtonSettings->SizeY = 0.1;
    t_ButtonSettings->PositionX = -0.75;
    t_ButtonSettings->PositionY = -0.2;
    t_PanelSettings->m_isHidden = true;
    std::function<void()> lSettings = [&]()->void {t_PanelSettings->ChangeVisibility();};
    t_ButtonSettings->OnClickCallback(lSettings);
    t_ButtonSettings->m_isHidden = false;
    t_ButtonSettings->m_TextRendererInstance = text;
    t_Container->addWidget(t_ButtonSettings);

    t_ButtonResume = (std::shared_ptr<Button>) new Button(0.15, 0.1, this->WIDTH, this->HEIGHT, "Resume");
    t_ButtonResume->SizeX = 0.15;
    t_ButtonResume->SizeY = 0.1;
    t_ButtonResume->PositionX = -0.75;
    t_ButtonResume->PositionY = 0.05;
    std::function<void()> lResume = [&]()->void {
        this->onMenu = false;
        menuTime = this->etime;
    };
    t_ButtonResume->OnClickCallback(lResume);
    t_ButtonResume->m_isHidden = false;
    t_ButtonResume->m_TextRendererInstance = text;

    t_Container->addWidget(t_ButtonResume);
    t_Container->addWidget(t_Panel);
    t_Container->addWidget(t_PanelSettings);
    m_GUI->AddContainer(t_Container);
*/
        RenderSplashScreen("Initializing Engine...");

        eCamera.push_back(std::make_shared<Camera>(glm::vec3(0.0f, 8.25f, -7.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
        eCamera.push_back(std::make_shared<Camera>(glm::vec3(20.0f, 30.25f, -60.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        this->ComputeCamera(NO_CLIP, glm::vec3(0.0f, 8.25f, -7.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        shadowMap = std::move((shared_ptr<Renderer::ShadowMap>)(new Renderer::ShadowMap(DATA.SHADOWMAP_SIZE, DATA.SHADOWMAP_SIZE, -20.0f, 80.0f)));

        mPointShadow = std::make_shared<Renderer::PointShadow>(glm::vec3(-5, 19.2, -28.74));
        mPointShadow->Setup();

        //RenderSplashScreen("Loading Shaders...");
        this->LoadShaders();

        //RenderSplashScreen("Loading Geometry...");
        this->LoadGeometry();

        this->LoadSound();

        std::vector<string> modelsNames;
        std::vector<glm::vec3> modelsPositions;
        std::vector<float> modelsScales;

        {
            glm::vec3 tPosition = glm::vec3(-18, 4.5, 0);
            glm::vec3 tScale = glm::vec3(1.3 * GOLDEN_RATIO);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/android.eml";

            auto _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            auto _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            _RComp->CastsShadows(true);
            _RComp->setTransparency(false);
            _RComp->setVisibility(true);
            _Entity->addComponent(_RComp);

            EntityList.push_back(_Entity);
        }

        {
            glm::vec3 tPosition = glm::vec3(10, 10, 10);
            glm::vec3 tScale = glm::vec3(2.0f);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/esfera.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            auto _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            _RComp->CastsShadows(true);
            _RComp->setTransparency(false);
            _RComp->setVisibility(true);
            MIN_MAX_POINTS _BoundingBox = ResourceManager::Get().getModelBoundingBox(tModelName);
            auto _PComp = std::make_shared<Component::PhysicComponent>(100, tPosition, tScale, Physics::Type::SPHERE, _BoundingBox);
            _PComp->setID(EntityList.size());
            //Component::Component_ptr _SComp = std::make_shared<Component::ScriptComponent>("scripts/test.lua");
            _Entity->addComponent(_RComp)->addComponent(_PComp) /*->addComponent(_SComp)*/;

            EntityList.push_back(_Entity);
        }

        for (int a = 0; a < 3; a++)
            for (int b = 0; b < 3; b++)
                for (int c = 0; c < 3; c++)
                {
                    glm::vec3 tPosition = glm::vec3(3, -1.0, 21);
                    glm::vec3 positions = tPosition + glm::vec3(a, b, c) * glm::vec3(2.0, 2.0, 2.0);
                    glm::vec3 tScale = glm::vec3(1.0f);
                    glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
                    std::string tModelName = "models/cube.eml";

                    std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(positions, tScale, tRotation);

                    auto _RComp = std::make_shared<Component::RenderComponent>(tModelName, positions, "Main");
                    _RComp->CastsShadows(true);
                    _RComp->setTransparency(false);
                    MIN_MAX_POINTS _BoundingBox = ResourceManager::Get().getModelBoundingBox(tModelName);
                    auto _PComp = std::make_shared<Component::PhysicComponent>(100, positions, tScale, Physics::Type::CUBE, _BoundingBox);
                    _PComp->setID(EntityList.size());
                    _Entity->addComponent(_RComp)->addComponent(_PComp);

                    EntityList.push_back(_Entity);
                }

        /* {
            glm::vec3 tPosition = glm::vec3(-5, 15, -5);
            glm::vec3 tScale = glm::vec3(0.025);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/sponza.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(true);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(true);
                        static_pointer_cast<Component::RenderComponent>(_RComp)->isDoubleFaced(false);

            _Entity->addComponent(_RComp);

            EntityList.push_back(_Entity);
        }*/
        /*  {
            glm::vec3 tPosition = glm::vec3(-5, 5, -5);
            glm::vec3 tScale = glm::vec3(1.85);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/map.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(true);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(true);

            _Entity->addComponent(_RComp);

            EntityList.push_back(_Entity);
    }*/
        {
            glm::vec3 tPosition = glm::vec3(-23, 1, 28);
            float tScale = 5.0f;
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, glm::vec3(tScale), tRotation);

            Component::Component_ptr _CComp = std::make_shared<Component::ClothComponent>(tPosition, tScale, tRotation, eCamera[mCurrentCamera]);

            std::static_pointer_cast<Component::ClothComponent>(_CComp)->setShader("Cloth");

            _Entity->addComponent(_CComp);

            EntityList.push_back(_Entity);
        }

        glm::vec3 initCubemapPosition = glm::vec3(-35.0, 0, -35.0);

        //33 34 14
        //-32 54 -13

        //-32 34 -13
        //33 54 14

        auto MenuAudio = [&]() -> void { m_AudioSystem->PlayByID(2); };

        /*
    t_ButtonQuit->OnEntering(MenuAudio);
    t_ButtonSettings->OnEntering(MenuAudio);
    t_ButtonResume->OnEntering(MenuAudio);

    RenderSplashScreen("Loading Textures...");*/
        ResourceManager::Get().loadQueuedTextures();

        PP = std::move((shared_ptr<PostProcess>)(new PostProcess()));

        m_PlayerCapsule = (std::shared_ptr<Player>)new Player(10, 15.8, -10);
        //m_PlayerCapsule = (std::shared_ptr<Game::Player>) new Game::Player(170.0,5.25,-202.0, this->rM);

        //m_Pick = (std::shared_ptr<Pick>) (new Pick(rM->m_PhysicsWorld->world));

        for (int x = 0; x < 5; ++x)
            for (int z = 0; z < 5; ++z)
                xz[x][z] = (rand() % 30) - 15;

        MINMAX_POINTS limits;
        limits.MAX_X = -36 + (7 * 10);
        limits.MIN_X = -36;
        limits.MAX_Y = -3.0;
        limits.MIN_Y = -12.0;
        limits.MAX_Z = -36 + (7 * 10);
        limits.MIN_Z = -36;

        m_ParticleSystem = (std::shared_ptr<ParticleSystem>)new ParticleSystem();
        m_ParticleSystem->addNewSystem(limits, MIST, 200);

        //3 , 7, 30
        sun->Update();
        shadowMap->setShadowPosition(glm::vec3(6 + 1.0 * 30, 8 + 45.0f, -8 + 0.0 * 30));

        shadowMap->setShadowDirection(sun->Direction);

        shadowMap->SetupShadowMatrices();

        shadowMap->Begin(0);
        this->RenderShadows(Shaders["ShadowMapping"]);
        shadowMap->End();

        mPointShadow->Begin(0);

        RenderShadows(ResourceManager::Get().getShader("PointShadow"));

        mPointShadow->End();

        //this->mCubemap[1] = (std::shared_ptr<CubeMap>)new CubeMap(55, glm::vec3(10, 7, 10));

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glViewport(0, 0, 1024, 1024);
        glEnable(GL_DEPTH_TEST);
        eCamera[mCurrentCamera]->Update(window->getHandle()->getHandle());
        eCamera[mCurrentCamera]->UpdateMatrices(0);
        sun->Update();
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
        glDisable(GL_CULL_FACE);
        glm::mat4 captureViews[6];
        captureViews[0] = glm::lookAt(glm::vec3(0.0), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        captureViews[1] = glm::lookAt(glm::vec3(0.0), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        captureViews[2] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        captureViews[3] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
        captureViews[4] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        captureViews[5] = glm::lookAt(glm::vec3(0.0), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        //33 34 14
        //-32 54 -13
        SphericalHarmonics::SphericalHarmonicsFormat sphStruct[7][7][7];
        auto start = std::chrono::steady_clock::now();
        //Shaders["CubeMap"]->Use();
        int index = 0;

        for (int a = 0; a < 7; a++)
            for (int b = 0; b < 7; b++)
                for (int c = 0; c < 7; c++)
                {

                    this->mCubemap[a][b][c] = (std::shared_ptr<CubeMap>)new CubeMap(index, initCubemapPosition + glm::vec3(a, b, c) * glm::vec3(10.0, 5.0, 10.0));
                    std::shared_ptr<Shader> cubeShader = this->mCubemap[a][b][c]->getShader();
                    float rotation = 0.5 * Clock::TimeSeconds();
                    for (int index = 0; index < 6; ++index)
                    {
                        this->mCubemap[a][b][c]->Begin(index);

                        //Render cubemap begin
                        {
                            glDisable(GL_CULL_FACE);
                            glDisable(GL_DEPTH_TEST);
                            Shaders["SkyBox_Cubemap"]->Use();

                            glm::mat4 view = captureViews[index];
                            glm::mat4 projection = this->mCubemap[a][b][c]->getProjectionMatrix();
                            glm::mat4 model = glm::mat4(1.0);

                            glm::mat4 ScaleMatrix = glm::scale(model, glm::vec3(5.0));
                            glm::mat4 TranslationMatrix = glm::translate(model, glm::vec3(0.0, 0.0, 0.0));
                            model = model * ScaleMatrix * TranslationMatrix;
                            glUniformMatrix4fv(glGetUniformLocation(Shaders["SkyBox_Cubemap"]->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
                            glUniformMatrix4fv(glGetUniformLocation(Shaders["SkyBox_Cubemap"]->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
                            glUniformMatrix4fv(glGetUniformLocation(Shaders["SkyBox_Cubemap"]->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);

                            glUniform3f(glGetUniformLocation(Shaders["SkyBox_Cubemap"]->getProgramID(), "LightDirection"), sun->Direction.x, sun->Direction.y, sun->Direction.z);
                            skybox->Render(this->eCamera[mCurrentCamera], Shaders["SkyBox_Cubemap"], PP->m_exposure, false);

                            Shaders["SkyBox_Cubemap"]->Free();

                            glEnable(GL_DEPTH_TEST);
                            //glDepthMask(GL_TRUE); //makes solid colors appear
                        }
                        //Render cubemap end

                        cubeShader->Use();

                        /****************************************************************************/

                        glEnable(GL_CULL_FACE);
                        for (unsigned int i = 0; i < EntityList.size(); ++i)
                        {
                            EntityList[i]->Update();
                            EntityList[i]->setShader("CubeMap");
                            glm::mat4 Model = glm::mat4(1.0);
                            if (EntityList[i]->HasClothComponent())
                                continue;
                            if (EntityList[i]->HasRenderComponent())
                            {
                                glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0f), EntityList[i]->getScale());
                                glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0f), EntityList[i]->getPosition());
                                glm::mat4 RotationMatrix = glm::mat4(1.0f) * glm::toMat4(glm::normalize(EntityList[i]->getRotation()));
                                Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
                                cubeShader->PushUniform("model", Model);
                                glm::mat4 currentView = mCubemap[a][b][c]->getViewMatrixbyIndex(index);
                                glm::mat4 currentProj = mCubemap[a][b][c]->getProjectionMatrix();
                                cubeShader->PushUniform("projection", currentProj);
                                cubeShader->PushUniform("view", currentView);
                                glUniformMatrix4fv(glGetUniformLocation(cubeShader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);
                                glUniformMatrix4fv(glGetUniformLocation(cubeShader->getProgramID(), "depthBias"), 1, GL_FALSE, &shadowMap->getBiasMatrix()[0][0]);
                                cubeShader->PushUniform("lightDir", sun->Direction);
                                glActiveTexture(GL_TEXTURE5);
                                cubeShader->PushUniform("shadowMap", 5);
                                glBindTexture(GL_TEXTURE_2D, shadowMap->getShadowTextureID());

                                glActiveTexture(GL_TEXTURE6);
                                cubeShader->PushUniform("gPointShadowMap", 6);
                                glBindTexture(GL_TEXTURE_CUBE_MAP, mPointShadow->getTexture());
                                //if(EntityList[i]->getModelPath() == "models/android.eml")
                                //IO::PrintLine(EntityList[i]->getModelPath(), "position is ", EntityList[i]->getPosition().x, EntityList[i]->getPosition().y, EntityList[i]->getPosition().z);
                            }
                            EntityList[i]->Render();
                        }
                        glDisable(GL_CULL_FACE);
                        glCullFace(GL_FRONT);
                        /****************************************************************************/

                        glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(0.1));
                        glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0));
                        glm::mat4 cModel = TranslationMatrix * ScaleMatrix;

                        cubeShader->PushUniform("model", cModel);
                        glCullFace(GL_FRONT);
                        BSPMap->Frustum.CalculateFrustum(glm::mat4(this->mCubemap[a][b][c]->captureProjection * this->mCubemap[a][b][c]->captureViews[index]), cModel);

                        glUniformMatrix4fv(glGetUniformLocation(cubeShader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);
                        glUniformMatrix4fv(glGetUniformLocation(cubeShader->getProgramID(), "depthBias"), 1, GL_FALSE, &shadowMap->getBiasMatrix()[0][0]);
                        cubeShader->PushUniform("lightDir", sun->Direction);
                        glActiveTexture(GL_TEXTURE5);
                        cubeShader->PushUniform("shadowMap", 5);
                        glBindTexture(GL_TEXTURE_2D, shadowMap->getShadowTextureID());
                        BSPMap->RenderLevel(mCubemap[a][b][c]->getPosition(), cubeShader->getProgramID(), true);
                        cubeShader->Free();
                    }
                    mCubemap[a][b][c]->End();
                    mCubemap[a][b][c]->genAmbientConvolution();
                    ResourceManager::Get().addCubemap(this->mCubemap[a][b][c], mCubemap[a][b][c]->getPosition());
                    sph.CalculateCohefficients(this->mCubemap[a][b][c]->getTextureID(), 3);
                    sph.setId(this->mCubemap[a][b][c]->getID());
                    sphStruct[a][b][c] = sph.toStruct();
                    /*



*/
                    //std::cout << "Generating SPH for cubemap " << a << std::endl;
                    /*for (unsigned int i = 0; i < sph.getCohefficients().size(); i++)
                {
                    std::cout << "vec3 " << sph.mCohefficientsNames[i] << " = vec3(" << sph.getCohefficients()[i].x << ", " << sph.getCohefficients()[i].y << ", " << sph.getCohefficients()[i].z << ");" << std::endl;
                }*/
                    index++;
                }
        auto finish = std::chrono::steady_clock::now();
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        std::cout << "Cubemap generation took: " << interval << " milliseconds" << std::endl;
        std::cout << interval / 245 << " per cubemap" << std::endl;

        {
            /*  for (int a = 0; a < 7; a++)
                for (int b = 0; b < 7; b++)
                    for (int c = 0; c < 7; c++)
                    {
                        glm::vec3 positions = initCubemapPosition + glm::vec3(a, b, c) * glm::vec3(10.0, 5.0, 10.0);
                        glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
                        std::string tModelName = "models/probe.eml";

                        std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(positions, glm::vec3(1.0), tRotation);
                        Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, positions, "Main");
                        static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(false);
                        static_pointer_cast<Component::RenderComponent>(_RComp)->isDoubleFaced(false);
                        static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
                        static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(false);

                        _Entity->addComponent(_RComp);
                        EntityList.push_back(_Entity);
                    }*/
        }

        {
            glm::vec3 positions = glm::vec3(4, 10, 22);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/probe.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(positions, glm::vec3(2.0f), tRotation);
            Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, positions, "Main");
            static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->isDoubleFaced(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(false);

            MIN_MAX_POINTS _BoundingBox = ResourceManager::Get().getModelBoundingBox(tModelName);
            Component::Component_ptr _PComp = std::make_shared<Component::PhysicComponent>(0.0f, positions, glm::vec3(2.0f), Physics::Type::SPHERE, _BoundingBox);

            _Entity->addComponent(_RComp)->addComponent(_PComp);
            EntityList.push_back(_Entity);
        }

        ResourceManager::Get().cubemapsLoaded = true;

        glCullFace(GL_BACK);

        ResourceManager::Get().loadQueuedTextures();

        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            EntityList[i]->setShader("Main");
            if (EntityList[i]->HasClothComponent())
                EntityList[i]->setShader("Cloth");
        }

        auto SSBOSize = sizeof(SphericalHarmonics::SphericalHarmonicsFormat) * 7 * 7 * 7;

        AmbientLightSSBO = std::make_shared<Renderer::ShaderStorage>(SSBOSize, 1);
        AmbientLightSSBO->Init(&sphStruct);

        /*
        glGenBuffers(1, &AmbientLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, AmbientLightSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SphericalHarmonics::SphericalHarmonicsFormat) * 7 * 7 * 7, (const void *)&sphStruct, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AmbientLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
*/
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        std::cout << "All Resources Initialized." << std::endl;
        //exit(0);
    }

    void Epsilon::LoadShaders(void)
    {
        cout << "Loading Shaders..." << endl;

        Shaders["grass"] = std::make_shared<Shader>("shaders/grass.vglsl", "shaders/grass.fglsl");

        Shaders["Main"] = std::make_shared<Shader>("shaders/Geometry.vglsl", "shaders/Geometry.fglsl");

        Shaders["Terrain"] = std::make_shared<Shader>("shaders/Geometry.vglsl", "shaders/GeometryTerrain.fglsl");

        //Shaders["Terrain"] = new Shader( "shaders/testshader.vglsl", "shaders/testshader.fglsl" );

        Shaders["Sun"] = std::make_shared<Shader>("shaders/Sun.vglsl", "shaders/Sun.fglsl");

        Shaders["SkyBox"] = std::make_shared<Shader>("shaders/skybox.vglsl", "shaders/_skybox.fglsl");

        Shaders["SkyBox_Cubemap"] = std::make_shared<Shader>("shaders/_skybox_cubemap_vertex.glsl", "shaders/_skybox_cubemap.glsl");

        Shaders["MD5Geometry"] = std::make_shared<Shader>("shaders/MD5Geometryv.glsl", "shaders/MD5Geometryf.glsl");

        Shaders["ShadowMapping"] = std::make_shared<Shader>("shaders/ShadowMappingv.glsl", "shaders/ShadowMappingf.glsl");

        Shaders["MD5ShadowMapping"] = std::make_shared<Shader>("shaders/MD5GeometryShadowv.glsl", "shaders/ShadowMappingf.glsl");

        Shaders["CubeMap"] = std::make_shared<Shader>("shaders/vertex.glsl", "shaders/fragment.frag");

        Shaders["ForwardShader"] = std::make_shared<Shader>("shaders/TransformVertexShader.vglsl", "shaders/TextureFragmentShader.fglsl");

        Shaders["DefaultParticle"] = std::make_shared<Shader>("shaders/defaultParticle.vglsl", "shaders/defaultParticle.fglsl");

        Shaders["Cloth"] = std::make_shared<Shader>("shaders/Cloth.vglsl", "shaders/Cloth.fglsl");

        Shaders["ImGui_gamma"] = std::make_shared<Shader>("shaders/ImGui_gamma_correct_vertex.glsl", "shaders/ImGui_gamma_correct_fragment.glsl");

        ResourceManager::Get().requestShader("shaders/Geometry.vglsl", "shaders/Geometry.fglsl", "Main");
        ResourceManager::Get().requestShader("shaders/Cloth.vglsl", "shaders/Cloth.fglsl", "Cloth");
        ResourceManager::Get().requestShader("shaders/vertex.glsl", "shaders/fragment.glsl", "CubeMap");

        ResourceManager::Get().requestShader("shaders/PointShadow.vert", "shaders/PointShadow.frag", "shaders/PointShadow.geom", "PointShadow");

        //rM->requestShader("shaders/vertex.glsl", "shaders/fragment.glsl", "CubeMap");
    }

    void Epsilon::LoadGeometry(void)
    {
        cout << "Loading World Geometry..." << endl;
        vector<glm::vec3> grasspos2;

        std::cout << "Llega" << std::endl;
        terrain = std::make_shared<Terrain>("materials/terrain_1024_alpine3_height.png",
                                            "Rock_6_d.png",
                                            "Rock_6_n.png",
                                            "Rock_6_s.png",
                                            "Rock_6_ao.png", 0.5, 200, glm::vec3(0, -67.0, 0));

        for (int i = 0; i < terrain->vertices.size(); i += 2)
        {
            //if(terrain->vertices[i].Position.y < 0)
            //    continue;
            int chance = rand() % 2 + 1;
            if (chance == 1)
                grassPos.push_back(terrain->vertices[i].Position + glm::vec3((rand() % 6) - 3, 3.5, (rand() % 6) - 3));
            else if (chance == 2)
                grasspos2.push_back(terrain->vertices[i].Position + glm::vec3((rand() % 6) - 3, 3.5, (rand() % 6) - 3));
        }

        //std::cout << "Resource manager in epsilon address: " << rM.get() << std::endl;

        ResourceManager::Get().requestCubeMap(50000, glm::vec3(4.8, 80000.2, -8));
        ResourceManager::Get().requestModel("models/esfera.eml", glm::vec3(78.0, 5.25, -57), glm::vec3(1), glm::quat(0.0, 0.0, 0.0, 0.0));
        ResourceManager::Get().requestModel("models/sphere.eml", glm::vec3(78.0, 5.25, -57), glm::vec3(1), glm::quat(0.0, 0.0, 0.0, 0.0));
        //rM->requestModel("models/Desk.eml", rM, glm::vec3(-2.0,8.0,10.0), glm::vec3(0.9), glm::quat(1, 0.0, -1.0, 0.0));
        //rM->requestModel("models/sponza.eml", rM, glm::vec3(-16,5.0,-15), glm::vec3(0.025), glm::quat(0, 0.0, 0, 0.0));
        float first, second, delta;
        first = Clock::TimeSeconds();
        std::vector<std::string> paths;
        std::string path;

        paths.push_back(path + "materials/skyboxes/Miramar/right.tga");
        paths.push_back(path + "materials/skyboxes/Miramar/left.tga");
        paths.push_back(path + "materials/skyboxes/Miramar/top.tga");
        paths.push_back(path + "materials/skyboxes/Miramar/bottom.tga");
        paths.push_back(path + "materials/skyboxes/Miramar/back.tga");
        paths.push_back(path + "materials/skyboxes/Miramar/front.tga");

        skybox = std::move((shared_ptr<Skybox>)(new Skybox("plain")));

        grass.push_back(Grass("grass04.png", grassPos));
        grass.push_back(Grass("billboardgrass0002.png", grasspos2));

        waterPlane = (shared_ptr<Water>)(new Water(glm::vec3(0.0, 0.5, 0.0), 1.0f)); ///-11.8
        sun = std::move((shared_ptr<Sun>)(new Sun()));
        BSPMap = std::move((shared_ptr<CQuake3BSP>)(new CQuake3BSP()));

        BSPMap->LoadBSP((string("maps/") + "GI.bsp").c_str());

        m_AnimModel = std::move((shared_ptr<MD5Model>)(new MD5Model()));

        m_AnimModel->LoadModel("models/hellknight/hellknight.md5mesh");

        m_AnimModel->LoadAnim("models/hellknight/walk7.md5anim");
        m_AnimModel->LoadAnim("models/hellknight/idle2.md5anim");
    }

    void Epsilon::LoadSound(void)
    {
        cout << "Loading Sound..." << endl;

        m_AudioSystem = (std::shared_ptr<IO::Audio::Audio>)new IO::Audio::Audio();
        /*
        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement = (std::shared_ptr<IO::Audio::AudioElement>) new IO::Audio::AudioElement("sound/File0279.wav", STATIC_SOUND, glm::vec3(-28, 10, 15), glm::vec3(0,0,0));
        m_AudioSystem->addAudioElement(3, m_AudioElement);
    */

        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement2 = (std::shared_ptr<IO::Audio::AudioElement>)new IO::Audio::AudioElement("sound/ambient.wav", MUSIC, glm::vec3(-28, 10, 15), glm::vec3(0, 0, 0));
        m_AudioSystem->addAudioElement(0, m_AudioElement2);

        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement3 = (std::shared_ptr<IO::Audio::AudioElement>)new IO::Audio::AudioElement("sound/flag.wav", STATIC_SOUND, glm::vec3(96, 16, 6), glm::vec3(0, 0, 0));
        m_AudioSystem->addAudioElement(1, m_AudioElement3);

        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement4 = (std::shared_ptr<IO::Audio::AudioElement>)new IO::Audio::AudioElement("sound/hover.wav", MENU_SOUND, glm::vec3(96, 16, 6), glm::vec3(0, 0, 0));
        m_AudioSystem->addAudioElement(2, m_AudioElement4);

        m_AudioListener = (std::shared_ptr<IO::Audio::AudioListener>)new IO::Audio::AudioListener();
    }

    bool visible = true;
    float acum = -1.0;
    void Epsilon::Render3D(std::shared_ptr<Shader> shader)
    {
        glm::mat4 Model = glm::mat4(1);
        glm::mat4 ScaleMatrix;
        glm::mat4 TranslationMatrix;
        glm::mat4 RotationMatrix;

        shader->Use();

        //Shaders["Terrain"]->Use();
        this->SetUniforms(shader, glm::vec3(0, 0, 0), glm::vec3(1.0), glm::quat(0, 0, 0, 0));
        //terrain->RenderTerrain(shader);
        glCullFace(GL_BACK);

        glDisable(GL_CULL_FACE);

        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            if (EntityList[i]->HasRenderComponent())
            {
                shader->Use();
                shader->PushUniform("parallaxOn", ParallaxOn);
                shader->PushUniform("ambientDivider", ambientDivider);
                shader->PushUniform("FrameNumber", (int)window->FrameNumber());

                ResourceManager::Get().setModelUniforms(EntityList[i]->getModelPath(), shader, EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation(),
                                                        EntityList[i]->getPrevPosition(), EntityList[i]->getPrevScale(), EntityList[i]->getPrevRotation(),
                                                        eCamera[mCurrentCamera]);
            }
            EntityList[i]->Render();
        }
        /*glCache::*/
        glEnable(GL_CULL_FACE);

        /*glCache::*/
        glCullFace(GL_FRONT);

        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glm::mat4 BSPmodel = glm::mat4(1.0);
        //glm::mat4 tmodel = glm::translate(glm::mat4(1.0), glm::vec3(-30.0, 5.0, -120.0));

        glm::mat4 sModel = glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1));
        BSPmodel = sModel;
        BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix()), BSPmodel);
        shader->Use();
        this->SetUniforms(shader, glm::vec3(0.0), glm::vec3(0.1), glm::quat(0, 0, 0, 0));
        BSPMap->RenderLevel(eCamera[mCurrentCamera]->getPosition(), shader->getProgramID(), true);
        /*
    Shaders["MD5Geometry"]->Use();
    glUniformMatrix4fv(glGetUniformLocation(Shaders["MD5Geometry"]->getProgramID(), "mSkinned"), 150, GL_FALSE, &m_AnimModel->m_AnimatedBones[0][0][0]);
    this->SetUniforms(Shaders["MD5Geometry"], glm::vec3(mpos, 0.8, 15), glm::vec3(0.1, 0.1, 0.1), glm::quat(-1.0, 1.0, 0.0, 0.0));

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(Shaders["MD5Geometry"]->getProgramID(), "skybox"), 4); 
    glBindTexture(GL_TEXTURE_CUBE_MAP, ResourceManager::Get().useCubeMap(ResourceManager::Get().getNearestCubemapIndex(glm::vec3(mpos, 0.8, 15))));
    glUniform1i(glGetUniformLocation(Shaders["MD5Geometry"]->getProgramID(), "CubemapID"), ResourceManager::Get().NearestCubeMap(glm::vec3(mpos, 0.8, 15)));
    //std::cout << "Using cubemap id: " << resm->mCubemapIndex.at(resm->NearestCubeMap(pos)) << std::endl;
    glUniform1i(glGetUniformLocation(Shaders["MD5Geometry"]->getProgramID(), "AmbientProbeID"), ResourceManager::Get().NearestCubeMap(glm::vec3(mpos, 0.8, 15)) - 1);

    m_AnimModel->Render(Shaders["MD5Geometry"]->getProgramID());*/
        glCullFace(GL_BACK);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void Epsilon::RenderShadows(std::shared_ptr<Shader> shader)
    {
        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            if (!EntityList[i]->HasRenderComponent())
                continue;

            Component::RenderComponent_ptr RComponent = EntityList[i]->getComponent<Component::RenderComponent_ptr>();

            if (RComponent != nullptr)
                if (!RComponent->CastsShadows())
                    continue;

            //shader->Use();
            shader->PushUniform("FrameNumber", (int)window->FrameNumber());
            this->SetUniforms(shader, EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation());
            ResourceManager::Get().useModel(EntityList[i]->getModelPath(), shader, EntityList[i]->getPosition());
        }

        //mPatch->updateVertexBuffers(mCloth->getVertices());
        //this->SetUniforms(Shaders["ShadowMapping"], glm::vec3(0.0), glm::vec3(1.0), glm::quat(0.0f, 0.0, 0.0, 0.0));
        //mPatch->Render(Shaders["ShadowMapping"], eCamera->getViewMatrix(), eCamera->getProjectionMatrix());

        glDisable(GL_CULL_FACE);

        glm::mat4 BSPmodel = glm::mat4(1.0);
        //glm::mat4 tmodel = glm::translate(glm::mat4(1.0), glm::vec3(-30.0, 5.0, -120.0));
        glm::mat4 sModel = glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1));
        BSPmodel = sModel;
        BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix()), BSPmodel);
        //shader->Use();

        shader->PushUniform("FrameNumber", (int)window->FrameNumber());
        this->SetUniforms(shader, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1, 0.1, 0.1), glm::quat(0.0, 0.0, 0.0, 0.0));
        BSPMap->RenderLevel(eCamera[mCurrentCamera]->getPosition(), shader->getProgramID(), false);

        /*
    Shaders["MD5ShadowMapping"]->Use();
    glUniformMatrix4fv(glGetUniformLocation(Shaders["MD5ShadowMapping"]->getProgramID(), "mSkinned"), 150, GL_FALSE, &m_AnimModel->m_AnimatedBones[0][0][0]);
    this->SetUniforms(Shaders["MD5ShadowMapping"], glm::vec3(mpos, 0.8, 15), glm::vec3(0.1, 0.1, 0.1), glm::quat(-1.0, 1.0, 0.0, 0.0f));
    m_AnimModel->Render(Shaders["MD5ShadowMapping"]->getProgramID());
    Shaders["MD5ShadowMapping"]->Free();*/
        glEnable(GL_CULL_FACE);
    }

    void Epsilon::SetUniforms(std::shared_ptr<Shader> shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation)
    {
        glm::mat4 Model = glm::mat4(1.0);
        glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0), scale);
        glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0), position);
        glm::mat4 RotationMatrix;
        RotationMatrix = glm::mat4(1) * glm::toMat4(glm::normalize(rotation));

        Model = TranslationMatrix * ScaleMatrix * RotationMatrix;

        glm::mat4 MVP = eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix() * Model;
        shader->PushUniform("MVP", MVP);
        shader->PushUniform("viewDir", eCamera[mCurrentCamera]->getDirection());
        shader->PushUniform("viewPos", eCamera[mCurrentCamera]->getPosition());
        //glUniformMatrix4fv(shader->MVP_Location, 1, GL_FALSE, &MVP[0][0]);

        glUniformMatrix4fv(shader->WorldTransform_Location, 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "PrevModel"), 1, GL_FALSE, &Model[0][0]);
        glUniformMatrix4fv(shader->View_Location, 1, GL_FALSE, &eCamera[mCurrentCamera]->getViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "PrevView"), 1, GL_FALSE, &eCamera[mCurrentCamera]->getPrevViewMatrix()[0][0]);
        glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &eCamera[mCurrentCamera]->getProjectionMatrix()[0][0]);
        glUniformMatrix4fv(shader->LightSpaceMatrix_Location, 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);
        glm::mat3 invModelMatrix = glm::transpose(glm::inverse(glm::mat3(Model)));
        glm::mat3 invNormalMatrix = glm::transpose(glm::inverse(glm::mat3(eCamera[mCurrentCamera]->getViewMatrix() * Model)));
        shader->PushUniform("invModelMatrix", invModelMatrix);
        shader->PushUniform("invNormalMatrix", invNormalMatrix);

        glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"), Clock::TimeSeconds());
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "isTransparent"), 0);
    }

    void Epsilon::Render2D(void)
    {
        glDisable(GL_DEPTH_TEST);
        //if (onMenu)
        //m_GUI->Render();
        m_TextAcum += frametime;
        static float acumfps = 0.0;
        static float acumfps2 = 0.0;

        if (m_TextAcum > 1.0)
        {
            m_TextAcum = 0.0;
            acumfps = acumfps2 / FramesSinceLastUIUpdate;
            acumfps2 = 0.0;
            FramesSinceLastUIUpdate = -1;
        }
        else
        {
            FramesSinceLastUIUpdate++;
            acumfps2 += fps;
        }

        GPU _gpu;
        int DEBUG_MODE = 3;

        bool show_app_metrics = false;
        bool show_app_style_editor = false;
        bool show_app_about = false;
        bool p_cross = false;
        static bool show_pipeline_viewer = false;
        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            bool opt_fullscreen = true;
            if (opt_fullscreen)
            {
                ImGuiViewport *viewport = ImGui::GetMainViewport();
                ImGui::SetNextWindowPos(viewport->Pos);
                ImGui::SetNextWindowSize(viewport->Size);
                ImGui::SetNextWindowViewport(viewport->ID);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
                ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
                window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
                window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
            }

            if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;
            ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
            ImGui::Begin("Main Window", NULL, window_flags); // Create a window called "Hello, world!" and append into it.
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();

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
                            std::string tModelName = "models/esfera.eml";

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
            }

            //ImGui::PopStyleVar();
            //ImGui::Begin("Canvas", NULL);

            /*Begin skybox window*/

            ImGui::Begin("Skybox"); // Create a window called "Hello, world!" and append into it.

            ImGui::Checkbox("Clouds", &skybox->clouds); // Edit bools storing our window open/close state

            ImGui::SliderFloat("Horizon Height", &skybox->Horizon_Height, 0.0f, 1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("Upper Sky Color", (float *)&skybox->UpperSky_color);    // Edit 3 floats representing a color
            ImGui::ColorEdit3("Lower Sky Color", (float *)&skybox->LowerSky_color);    // Edit 3 floats representing a color
            ImGui::ColorEdit3("Tint", (float *)&skybox->Horizon_color);                // Edit 3 floats representing a color

            ImGui::Separator();
            ImGui::SliderFloat3("Ambient divider", &ambientDivider[0], 0, 50);

            ImGui::End();

            /*End Skybox window*/

            /*Begin scene window*/
            if (show_pipeline_viewer)
            {
                static ImVec2 fu = ImVec2(960, 300);
                ImGui::SetNextWindowSize(fu, ImGuiCond_FirstUseEver);
                ImGui::Begin("Debug Framebuffer", &show_pipeline_viewer);

                //get the mouse position
                ImVec2 pos = ImGui::GetCursorScreenPos();

                static const char *items[] = {"gBuffer Normal", "Light Pass", "HBAO", "HBIL", "Screen Space Reflections", "FrameBuffer"};
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

                auto fov = eCamera[mCurrentCamera]->getFoV();

                auto TextureId = 0;

                if (item_current == "Light Pass")
                    TextureId = PP->getSceneTexture();
                else if (item_current == "gBuffer Normal")
                    TextureId = PP->gExpensiveNormal;
                else if (item_current == "Screen Space Reflections")
                    TextureId = PP->ReflectionTexture;
                else if (item_current == "HBAO")
                    TextureId = PP->ssaoColorBufferBlur;
                else if (item_current == "HBIL")
                    TextureId = PP->HBILFramebuffer->getRenderTargetHandler(0);
                else if (item_current == "FrameBuffer")
                    TextureId = mDefaultFrameBuffer->getRenderTargetHandler(0);

                //we are done working with this window
                unsigned int fb_size[3] = {(unsigned int)this->WIDTH, (unsigned int)this->HEIGHT, Shaders["ImGui_gamma"]->getProgramID()};

                //ImGui::GetWindowDrawList()->AddCallback([]() { }, nullptr);
                ImGui::Image((void *)TextureId, ImVec2(640, 360), ImVec2(0, 1), ImVec2(1, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f), ImVec4(1.0f, 1.0f, 1.0f, 0.5f));

                //ImGui::GetWindowDrawList()->AddCallback(ImDrawCallback_ResetRenderState, nullptr);
                ImGui::End();
            }

            ImGui::Begin("Hierarchy");
            if (ImGui::TreeNode("Entities"))
            {
                for (const auto &ent : EntityList)
                {
                    if (ImGui::TreeNode(ent->getHash().c_str()))
                    {
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

            ImGui::SetCursorScreenPos(ImVec2(8, this->HEIGHT - 60));
            ImGui::Text("View position: x=%.2f, y=%.2f, z=%.2f", eCamera[mCurrentCamera]->getPosition().x, eCamera[mCurrentCamera]->getPosition().y, eCamera[mCurrentCamera]->getPosition().z);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            //ImGui::End();

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window->getHandle()->getHandle(), &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glEnable(GL_DEPTH_TEST);
    }

    void Epsilon::ProcessAudio()
    {

        m_AudioListener->setListenerPosition(this->eCamera[mCurrentCamera]->getPosition());
        m_AudioListener->setListenerDirection(this->eCamera[mCurrentCamera]->getDirection(), this->eCamera[mCurrentCamera]->getUp());

        m_AudioListener->UpdateListener();

        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            if (EntityList[i]->HasSoundComponent())
            {
                EntityList[i]->getComponent(Component::SOUNDCOMPONENT)->Render();
            }
        }

        m_AudioSystem->PlayAudio();
    }

    void Epsilon::Clock()
    {
        Clock::Tick();
        etime = Clock::TimeSeconds();
        frametime = etime - lastTime;
        double t = 0.0;

        fps = 1.0 / frametime;
        if (etime > t)
        {
            fpss << fps;
            t = etime + (double)1.000;
        }

        eventtime += 1 * Clock::DeltaSeconds();
        //cout << eventtime << endl;
        sun->Update();

        ResourceManager::Get().timestep = Clock::LastSeconds();
    }

    void Epsilon::RenderSkybox(bool state)
    {
        Shaders["SkyBox"]->Use();

        glm::mat4 view = glm::mat4(glm::mat3(eCamera[mCurrentCamera]->getViewMatrix()));
        float rotation = 0.5 * Clock::TimeSeconds();
        glm::mat4 RotationMatrix = glm::rotate(glm::mat4(1.0), glm::radians(rotation), glm::vec3(0, 1, 0));
        view = view /* * RotationMatrix*/;
        glm::mat4 projection = glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix());
        glm::mat4 model = glm::mat4(1.0);

        glm::mat4 ScaleMatrix = glm::scale(model, glm::vec3(1, 1, 1));
        glm::mat4 TranslationMatrix = glm::translate(model, glm::vec3(0, 0, 0));
        model = model * ScaleMatrix * TranslationMatrix;
        glUniformMatrix4fv(glGetUniformLocation(Shaders["SkyBox"]->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(Shaders["SkyBox"]->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(Shaders["SkyBox"]->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);

        glUniform3f(glGetUniformLocation(Shaders["SkyBox"]->getProgramID(), "LightDirection"), sun->Direction.x, sun->Direction.y, sun->Direction.z);
        skybox->Render(this->eCamera[mCurrentCamera], Shaders["SkyBox"], PP->m_exposure, state);

        Shaders["Sun"]->Use();
        sun->SetUniforms(eCamera[mCurrentCamera], Shaders["Sun"]);
        sun->Render(Shaders["Sun"]);
    }

    void Epsilon::PollEvents(void)
    {

        /*if (glfwGetWindowAttrib(window, GLFW_FOCUSED))
    {
        glfwPollEvents();
    }
    else
    {
        glfwWaitEvents();
    }*/

        glfwPollEvents();
        Input::Joystick::JoystickManager::DetectJoysticks();
        Input::Joystick::JoystickManager::PollJoystick();
        auto _Joystick = Input::Joystick::JoystickManager::PrimaryJoystick();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        if (glm::abs((timeGUI * 60) - (etime * 60)) > 2.0f && onMenu)
        {
            timeGUI = this->etime;
            //m_GUI->PollEvents(window);
        }

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::ESCAPE] || _Joystick->getJoystickButton(_Joystick->getMapping()->START()))
        {
            if (glm::abs((menuTime * 60) - (etime * 60)) > 60.0f)
            {
                //g_Running = false;
                onMenu = !onMenu;
                menuTime = this->etime;
            }
        }
        /* if (onMenu)
        {
            glfwSetInputMode(window->getHandle()->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ComputeCamera(CAMERA_FIXED, eCamera[mCurrentCamera]->getPosition(), eCamera[mCurrentCamera]->getDirection(), eCamera[mCurrentCamera]->getProjectionMatrix(), eCamera[mCurrentCamera]->getViewMatrix());
            this->m_CameraMode = CAMERA_FIXED;
        }
        else
        {
            glfwSetInputMode(window->getHandle()->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            this->m_CameraMode = NO_CLIP;
        }*/

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::N])
            normal = !normal;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::F])
            flashLight = !flashLight;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::SPACE])
        {
            PP->HBAOOn = !PP->HBAOOn;
            SSAO = !SSAO;
        }

        auto resolveTime = [kt = KeyTime, t = etime](int key) -> bool { return glm::abs((kt[key] * 60) - (t * 60)) > 60.0f; };

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::C] && resolveTime(Input::GLFW::Key::C))
        {
            mCurrentCamera = (int)!mCurrentCamera;
            eCamera[mCurrentCamera]->isMoving(true);
            eCamera[!mCurrentCamera]->isMoving(true);
            KeyTime[Input::GLFW::Key::C] = etime;
        }

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::M] && resolveTime(Input::GLFW::Key::M))
        {
            auto &XPOS = Input::Mouse::XPOS;
            auto &YPOS = Input::Mouse::YPOS;
            if (this->m_CameraMode == CAMERA_FIXED)
            {
                XPOS = CursorBeforeMenu.x; //mod(CursorBeforeMenu.x, this->WIDTH);
                YPOS = CursorBeforeMenu.y; //mod(CursorBeforeMenu.y, this->HEIGHT);
                glfwSetCursorPos(window->getHandle()->getHandle(), CursorBeforeMenu.x, CursorBeforeMenu.y);
                window->HideCursor();
                this->m_CameraMode = NO_CLIP;
                onMenu = false;
            }
            else
            {
                CursorBeforeMenu.x = XPOS;
                CursorBeforeMenu.y = YPOS;

                XPOS = this->WIDTH / 2;
                YPOS = this->HEIGHT / 2;
                window->ShowCursor();
                glfwSetCursorPos(window->getHandle()->getHandle(), this->WIDTH / 2, this->HEIGHT / 2);
                this->m_CameraMode = CAMERA_FIXED;
                onMenu = true;
            }

            KeyTime[Input::GLFW::Key::M] = etime;
        }

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::P])
            parallax = !parallax;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::ADD])
        {
            windSpeed += frametime;
        }
        //exposure += 0.5 * frametime;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::SUBTRACT])
            exposure -= 0.5 * frametime;

        blend = glm::clamp(blend, 0.0f, 1.0f);

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::ARROW_UP])
        {
            mpos += 17 * frametime * 0.5;
            m_AnimModel->Update(frametime, glm::clamp(blend, 0.0f, 1.0f));
            blend -= 3.5 * frametime;
        }
        else
        {
            m_AnimModel->Update(frametime, glm::clamp(blend, 0.0f, 1.0f));
            blend += 0.6 * frametime;
            mpos += 17 * frametime * 0.5 * (1 - glm::clamp(blend, 0.0f, 1.0f));
        }

        /*
        m_Pick->CheckforPicking(btVector3(eCamera->getPosition().x, eCamera->getPosition().y, eCamera->getPosition().z),
                                btVector3(eCamera->getDirection().x*1000, eCamera->getDirection().y*1000, eCamera->getDirection().z*1000), 1000, this->rM->m_PhysicsWorld->world);
        */
        m_PlayerCapsule->CheckforPicking(btVector3(eCamera[mCurrentCamera]->getPosition().x, eCamera[mCurrentCamera]->getPosition().y, eCamera[mCurrentCamera]->getPosition().z),
                                         btVector3(eCamera[mCurrentCamera]->getDirection().x * 1000, eCamera[mCurrentCamera]->getDirection().y * 1000, eCamera[mCurrentCamera]->getDirection().z * 1000));

        //m_ParticleSystem->Simulate(this->frametime, eCamera[mCurrentCamera]->getPosition());
    }

    void Epsilon::MainLoop(void)
    {
        // 0 8 6
        while (g_Running && !window->WantsToClose())
        {
            this->ClearBuffers();

            this->Clock();

            this->PollEvents();

            if (!onMenu)
                this->ComputeCamera(m_CameraMode, glm::vec3(48.4247, 8.1507, -12.9128), glm::vec3(-0.785454, 0.0299956, 0.618193));

            this->ProcessAudio();

            static float FixedTimestep = 1.f / 120.f;

            timeBehind += Clock::DeltaSeconds();

            while (timeBehind >= FixedTimestep)
            {
                ResourceManager::Get().getPhysicsWorld()->Update(FixedTimestep);
                timeBehind -= FixedTimestep;
            }

            this->ComputeShadow();

            this->CalculateVisibility();

            this->ProcessFrame();

            this->RenderFrame();

            this->Render2D();

            this->SwapBuffers();

            glCache::DrawCalls = 0;
        }
    }

    void Epsilon::CalculateVisibility()
    {
        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            EntityList[i]->Update();

            if (!EntityList[i]->HasRenderComponent())
                continue;

            Component::RenderComponent_ptr rComponent = EntityList[i]->getComponent<Component::RenderComponent_ptr>();

            glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1), EntityList[i]->getScale());
            glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1), EntityList[i]->getPosition());
            glm::mat4 RotationMatrix = glm::toMat4(EntityList[i]->getRotation());
            glm::mat4 Model = TranslationMatrix * ScaleMatrix * RotationMatrix;

            BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix()), Model);
            bool visible = BSPMap->Frustum.BoxInFrustum(EntityList[i]->getBoundingBox());

            rComponent->setVisibility(visible);
        }

        auto prev_pos = EntityList[1]->getPosition();
        EntityList[1]->setPosition(glm::vec3(prev_pos.x, prev_pos.y + glm::sin(Clock::TimeSeconds()), prev_pos.z));
    }

    void Epsilon::SwapBuffers(void)
    {
        //glfwSwapBuffers(this->window->getHandle()->getHandle());
        this->window->IncrementFrame();
        this->window->SwapBuffers();
        lastTime = etime;
    }

    void Epsilon::ClearBuffers(void)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Epsilon::ComputeCamera(CAMERA_MODE mode, glm::vec3 position, glm::vec3 direction, glm::mat4 proj, glm::mat4 view)
    {
        this->eCamera[mCurrentCamera]->Update(this->window->getHandle()->getHandle());

        if (mode == PLAYER_CONTROLLED)
        {
            m_PlayerCapsule->preStep();
            m_PlayerCapsule->Movement(this->eCamera[mCurrentCamera], frametime);
            eCamera[mCurrentCamera]->setPosition(m_PlayerCapsule->getPosition());
        }
        else if (mode == CAMERA_FIXED)
        {
            eCamera[mCurrentCamera]->setPosition(position);
            eCamera[mCurrentCamera]->setDirection(direction);
        }
        else if (mode == CAMERA_OVERRIDE)
        {
            eCamera[mCurrentCamera]->setPosition(position);
            eCamera[mCurrentCamera]->setDirection(direction);
            eCamera[mCurrentCamera]->setViewMatrix(view);
            eCamera[mCurrentCamera]->setProjection(proj);
        }
        else if (mode == NO_CLIP)
        {
            /*default mode: everything is calculated automatically*/
        }
        else
        {
        }

        this->eCamera[mCurrentCamera]->UpdateMatrices(window->FrameNumber());
    }

    void Epsilon::ComputeShadow()
    {

        for (auto &ent : EntityList)
        {
            if (ent->HasPhysicComponent())
            {
                auto comp = ent->getComponent<Component::PhysicComponent_ptr>();
                comp->addForce(glm::vec3(4, 10, 22));
            }
        }

        glm::vec3 camPos = eCamera[mCurrentCamera]->getPosition();
        glm::vec3 camDir = eCamera[mCurrentCamera]->getDirection();

        shadowMap->setShadowPosition(glm::vec3(camPos.x + camDir.x * 30, camPos.y + 45.0f, camPos.z + camDir.z * 30));

        shadowMap->setShadowDirection(sun->Direction);

        shadowMap->SetupShadowMatrices();

        shadowMap->Begin(0);
        Shaders["ShadowMapping"]->Use();

        //glEnable(GL_DEPTH_CLAMP);
        if (sun->height > 0.0)
            this->RenderShadows(Shaders["ShadowMapping"]);
        //glDisable(GL_DEPTH_CLAMP);
        shadowMap->End();

        Shaders["ShadowMapping"]->Free();

        //------------- Point Shadow ---------------
        mPointShadow->Begin(0);

        RenderShadows(ResourceManager::Get().getShader("PointShadow"));

        mPointShadow->End();
    }

    void Epsilon::ProcessFrame(void)
    {
        PP->beginOffScreenrendering();

        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, this->AmbientLightSSBO);
        //glEnable(GL_DEPTH_CLAMP);
        this->RenderSkybox(true);
        glClearDepth(1.0f);
        /*
    Shaders["grass"]->Use();
    this->SetUniforms(Shaders["grass"], glm::vec3(0, 0, 0), glm::vec3(0.05), glm::quat(-1, 0, -1, 0));
    grass.at(0).Render(Shaders["grass"]);

    Shaders["grass"]->Use();
    this->SetUniforms(Shaders["grass"], glm::vec3(0, 0, 0), glm::vec3(0.05), glm::quat(-1, 0, -1, 0));
    grass.at(1).Render(Shaders["grass"]);
*/

        this->Render3D(Shaders["Main"]);

        //glDisable(GL_DEPTH_CLAMP);
        PP->endOffScreenRendering();

        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        if (this->SSAO)
            PP->applySSAO(this->eCamera[mCurrentCamera]);

        PP->applyHBIL(this->eCamera[mCurrentCamera]);
    }

    void Epsilon::RenderParticles(void)
    {
        Shaders["DefaultParticle"]->Use();
        glUniform3f(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "cameraRight"), eCamera[mCurrentCamera]->getRight().x, eCamera[mCurrentCamera]->getRight().y, eCamera[mCurrentCamera]->getRight().z);
        glUniform3f(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "cameraUp"), eCamera[mCurrentCamera]->getUp().x, eCamera[mCurrentCamera]->getUp().y, eCamera[mCurrentCamera]->getUp().z);
        glUniform2f(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "resolution"), this->WIDTH, this->HEIGHT);
        glUniformMatrix4fv(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "view"), 1, GL_FALSE, &eCamera[mCurrentCamera]->getViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "PrevView"), 1, GL_FALSE, &eCamera[mCurrentCamera]->getPrevViewMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "depthBias"), 1, GL_FALSE, &shadowMap->getBiasMatrix()[0][0]);
        this->SetUniforms(Shaders["DefaultParticle"], glm::vec3(0.0f), glm::vec3(4.0f), glm::quat(1.0, sin(Clock::TimeSeconds() * frametime), 0.0f, cos(Clock::TimeSeconds() * frametime)));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "texture0"), 0);
        glBindTexture(GL_TEXTURE_2D, tex->getTextureID());
        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "bufferDepth"), 1);
        glBindTexture(GL_TEXTURE_2D, PP->gDepth);
        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(Shaders["DefaultParticle"]->getProgramID(), "shadowMap"), 2);
        glBindTexture(GL_TEXTURE_2D, this->shadowMap->getShadowTextureID());
        m_ParticleSystem->Render();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE3);
        Shaders["DefaultParticle"]->Free();
        glEnable(GL_CULL_FACE);
    }

    void Epsilon::RenderFrame(void)
    {
        glViewport(0, 0, this->WIDTH, this->HEIGHT);

        glEnable(GL_BLEND);
        PP->ShowFrame(sun->Direction, SSAO, this->eCamera[mCurrentCamera], exposure, this->shadowMap, mPointShadow);
        glEnable(GL_DEPTH_CLAMP);
        this->RenderSkybox(false);
        glDisable(GL_DEPTH_CLAMP);
        /*
    glDisable(GL_BLEND);
    this->waterPlane->RenderWater(eCamera, PP->CopyTextureFBO->getRenderTargetHandler(0), glm::normalize(glm::vec3(83, 6, -3) - glm::vec3(0, 6, -3)), PP->gDepth, rM->useCubeMap(54) );
    glEnable(GL_BLEND);
*/
        //this->RenderParticles();
        PP->ShowPostProcessImage(this->frametime, (int)this->onMenu, this->sun->Direction, this->eCamera[mCurrentCamera]);
        glEnable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Blit to named default framebuffer

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        mDefaultFrameBuffer->setToDraw();

        glBlitFramebuffer(0, 0, this->WIDTH, this->HEIGHT, 0, 0, this->WIDTH, this->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
} // namespace Epsilon