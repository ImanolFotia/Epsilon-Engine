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
#include <System/CPU.hpp>
#include <Log.h>
#include <System/Console.hpp>
#include <System/GPU.hpp>

#include <Driver/Audio/XAudio2/XAudio2.hpp>

#include <Engine.hpp>
#include <Clock.hpp>
#include <IO/File/Json.hpp>

#include <Editor/Editor.hpp>

float mpos = -20.0;
float blend = 1.0f;

namespace Epsilon
{
    ResourceManager ResourceManager::instance;

    Epsilon::Epsilon(const char *name) : App(name)
    {
        //window = win;
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

        //cpu.printHardwareInformation();

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

        m_CameraMode = CAMERA_FIXED;

        Editor::GUI::Init(mWindow);

        Editor::GUI::WIDTH = &WIDTH;
        Editor::GUI::HEIGHT = &HEIGHT;
        Editor::GUI::mRenderGIFunction = std::bind(&Epsilon::RenderGlobalIllumination, this);
        //window->HideCursor();

        mDefaultFrameBuffer = std::make_shared<OpenGL::FrameBuffer<int>>(mWindow->getWindowData().Width, mWindow->getWindowData().Height, false);
        mDefaultFrameBuffer->addRenderTarget(0, GL_RGB, GL_RGB, GL_NEAREST, GL_NEAREST, false);
        mDefaultFrameBuffer->FinishFrameBuffer();

        mCameraData = std::make_shared<API::OpenGL::UniformBuffer>(sizeof(Camera::CameraData_t), 1, 0, GL_DYNAMIC_DRAW);
        mCameraData->Bind();

        //Scripts::LuaS cript _script;
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

    void Epsilon::onCreate()
    {
        cout << "Initializing Resources..." << endl;

        ProgramData DATA;

        this->WIDTH = DATA.WINDOW_WIDTH;
        this->HEIGHT = DATA.WINDOW_HEIGHT;
        this->SSAO = DATA.HBAO;
        this->ParallaxOn = DATA.PARALLAX_OCLUSSION_MAPPING;
        RenderSplashScreen("Initializing Engine...");

        eCamera.push_back(std::make_shared<Camera>(glm::vec3(5.0f, 5.0f, -5.0f), glm::vec3(-0.694f, -0.312f, 0.648f)));
        eCamera.push_back(std::make_shared<Camera>(glm::vec3(20.0f, 30.25f, -60.0f), glm::vec3(0.0f, 1.0f, 0.0f)));

        Input::Mouse::MouseEventHandler += ([](auto *sender, beacon::args *args)
                                            {
                                                if (args == nullptr)
                                                    return;

                                                auto obj = args->to<Input::MouseArgs>();
                                                if (obj.Left().State == Input::PRESSED)
                                                {
                                                    std::cout << "Left button pressed" << std::endl;
                                                }
                                            });

        this->ComputeCamera(NO_CLIP, glm::vec3(0.0f, 8.25f, -7.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        shadowMap = std::move((shared_ptr<Renderer::ShadowMap>)(new Renderer::ShadowMap(DATA.SHADOWMAP_SIZE, DATA.SHADOWMAP_SIZE, -20.0f, 80.0f)));
        shadowMap->setShadowPosition(glm::vec3(0.0, -1.0, 0.0));

        //RenderSplashScreen("Loading Shaders...");
        this->LoadShaders();

        //RenderSplashScreen("Loading Geometry...");
        this->LoadGeometry();

        mSphere = std::make_shared<Renderer::Sphere>(20);

        auto SphereMaterial = mSphere->getMaterial();

        using Tex2D_ptr = std::shared_ptr<Renderer::Texture2D>;
        auto &ref = ResourceManager::Get();

        ref.addTextureToQueue("textures/epsilon/industrial-tile1-albedo.png");
        ref.addTextureToQueue("textures/epsilon/industrial-tile1-metallic.png");
        ref.addTextureToQueue("textures/epsilon/industrial-tile1-normal-dx.png");
        ref.addTextureToQueue("textures/epsilon/industrial-tile1-roughness.png");

        ref.ForceLoading();

        SphereMaterial->setMaterial(Renderer::Material::MaterialParameter::Albedo, ref.Get<Tex2D_ptr>("textures/epsilon/industrial-tile1-albedo.png"));
        SphereMaterial->setMaterial(Renderer::Material::MaterialParameter::Metallic, ref.Get<Tex2D_ptr>("textures/epsilon/industrial-tile1-metallic.png"));
        SphereMaterial->setMaterial(Renderer::Material::MaterialParameter::Normal, ref.Get<Tex2D_ptr>("textures/epsilon/industrial-tile1-normal-dx.png"));
        SphereMaterial->setMaterial(Renderer::Material::MaterialParameter::Roughness, ref.Get<Tex2D_ptr>("textures/epsilon/industrial-tile1-roughness.png"));

        {
            glm::vec3 tPosition = glm::vec3(0.0f, 3.0f, 0.0f);
            glm::vec3 tScale = glm::vec3(1.0);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);
 
            Component::RenderComponent_ptr _RComp = std::make_shared<Component::RenderComponent>(mSphere, "Main");
            _RComp->CastsShadows(true);
            _RComp->setTransparency(false);
            _RComp->setVisibility(true);
            _RComp->setRenderId(EntityList.size());

            _Entity->addComponent(_RComp);
            _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));

            EntityList.push_back(_Entity);
        }

        //this->LoadSound();

        std::vector<string> modelsNames;
        std::vector<glm::vec3> modelsPositions;
        std::vector<float> modelsScales;
        /*
        {
            glm::vec3 tPosition = glm::vec3(-5, 10.5, 6.0);
            glm::vec3 tScale = glm::vec3(0.007 * GOLDEN_RATIO);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/sponza.eml";
   
            auto _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);
    
            auto _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            _RComp->CastsShadows(true);
            _RComp->setVisibility(true);
            _RComp->setTransparency(false);
            _RComp->isDoubleFaced(false);
            _RComp->setRenderId(EntityList.size());
            _Entity->addComponent(_RComp);

            /*_Entity->mFunction = [cam = eCamera[mCurrentCamera]](EntityBase *ent) {
                Component::TransformComponent_ptr t = std::static_pointer_cast<Component::TransformComponent>(ent->getComponentList()[Component::TRANSFORMCOMPONENT]);
                if (glm::length(cam->getPosition() - t->Position()) < 10.0f)
                { 
                    t->PrevRotation(t->Rotation());
                    t->Rotation(glm::quat(glm::rotate(glm::mat4(1.0), (float)glm::radians(Clock::Time() * .2), glm::vec3(0.0, 1.0, 0.0))));
                } else {
                    t->PrevRotation(t->Rotation());
                }
            };*/

        /*EntityList.push_back(_Entity);
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
            _Entity->addComponent(_RComp)->addComponent(_PComp) /*->addComponent(_SComp)*/
        /* ;

            _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));
            EntityList.push_back(_Entity);
        }*/

        auto random_double = []() -> double
        {
            std::uniform_real_distribution<double> distribution(0.0, 2.0);
            std::mt19937 generator;
            //generator.seed(time(NULL));
            return distribution(generator);
        };
        /*
        {
            glm::vec3 tPosition = glm::vec3(-5, 15, -5);
            glm::vec3 tScale = glm::vec3(0.025);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/sponza.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            Component::RenderComponent_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            _RComp->CastsShadows(true);
            _RComp->setTransparency(false);
            _RComp->setVisibility(true);
            _RComp->isDoubleFaced(false);
            _RComp->setRenderId(EntityList.size());

            _Entity->addComponent(_RComp);
            _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));
 
            EntityList.push_back(_Entity);
        }
*/

        {
            glm::vec3 tPosition = glm::vec3(0.0f);
            glm::vec3 tScale = glm::vec3(1.0);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/cube.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            Component::RenderComponent_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            _RComp->CastsShadows(true);
            _RComp->setTransparency(false);
            _RComp->setVisibility(true);
            _RComp->setRenderId(EntityList.size());

            _Entity->addComponent(_RComp);
            _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));

            EntityList.push_back(_Entity);
        }
        /*{
            glm::vec3 tPosition = glm::vec3(-23, 1, 28);
            float tScale = 5.0f;
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, glm::vec3(tScale), tRotation);
 
            Component::Component_ptr _CComp = std::make_shared<Component::ClothComponent>(tPosition, tScale, tRotation, eCamera[mCurrentCamera]);
 
            std::static_pointer_cast<Component::ClothComponent>(_CComp)->setShader("Cloth");
  
            _Entity->addComponent(_CComp);

            _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));
            EntityList.push_back(_Entity);
        }
  
        {
            glm::vec3 positions = glm::vec3(-31.0, 11.2, 5.8);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/probe.eml";
        
            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(positions, glm::vec3(1.0f), tRotation);
            Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, positions, "Main");
            static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->isDoubleFaced(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(true);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setRenderId(EntityList.size());
 
            MIN_MAX_POINTS _BoundingBox = ResourceManager::Get().getModelBoundingBox(tModelName);
            Component::Component_ptr _PComp = std::make_shared<Component::PhysicComponent>(0.0f, positions, glm::vec3(2.0f), Physics::Type::SPHERE, _BoundingBox);

            _Entity->addComponent(_RComp)->addComponent(_PComp);
            _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));
            EntityList.push_back(_Entity);
        }*/

        //33 34 14
        //-32 54 -13

        //-32 34 -13
        //33 54 14

        /*auto MenuAudio = [&]() -> void
        { m_AudioSystem->PlayByID(2); };*/

        /*
    t_ButtonQuit->OnEntering(MenuAudio);
    t_ButtonSettings->OnEntering(MenuAudio);
    t_ButtonResume->OnEntering(MenuAudio);

    RenderSplashScreen("Loading Textures...");*/
        ResourceManager::Get().loadQueuedModels();
        ResourceManager::Get().loadQueuedTextures();

        mGizmo = std::make_shared<Renderer::Gizmo>();
        ResourceManager::Get().ForceLoading();

        PP = std::move((shared_ptr<PostProcess>)(new PostProcess()));
        Editor::GUI::PostProcess_ref = PP;

        mPointShadow = std::make_shared<Renderer::PointShadow>(PP->getLight(0).position);
        mPointShadow->Setup();

        m_PlayerCapsule = (std::shared_ptr<Player>)new Player(10, 15.8, -10);
        //m_PlayerCapsule = (std::shared_ptr<Game::Player>) new Game::Player(170.0,5.25,-202.0, this->rM);

        //m_Pick = (std::shared_ptr<Pick>) (new Pick(rM->m_PhysicsWorld->world));

        for (int x = 0; x < 5; ++x)
            for (int z = 0; z < 5; ++z)
                xz[x][z] = (rand() % 30) - 15;

        MINMAX_POINTS limits;
        limits.MAX_X = -36 + (7 * 10);
        limits.MIN_X = -36;
        limits.MAX_Y = 40.0;
        limits.MIN_Y = 0.0;
        limits.MAX_Z = -36 + (7 * 10);
        limits.MIN_Z = -36;

        m_ParticleSystem = (std::shared_ptr<ParticleSystem>)new ParticleSystem();
        m_ParticleSystem->addNewSystem(limits, MIST, 50);

        //3 , 7, 30
        sun->Update(); /*glm::vec3(6 + 1.0 * 30, 8 + 45.0f, -8 + 0.0 * 30)*/

        shadowMap->setShadowPosition(sun->Position);

        shadowMap->setShadowDirection(sun->Direction);

        shadowMap->SetupShadowMatrices();

        shadowMap->Begin(0);

        this->RenderShadows(Shaders["ShadowMapping"]);

        shadowMap->End();

        mPointShadow->Begin(0);

        RenderShadows(ResourceManager::Get().getShader("PointShadow"));

        mPointShadow->End();

        //this->mCubemap[1] = (std::shared_ptr<CubeMap>)new CubeMap(55, glm::vec3(10, 7, 10));

        glClearColor(0.0, 0.0, 0.0, 1.0);
        glViewport(0, 0, 1024, 1024);
        glEnable(GL_DEPTH_TEST);
        //eCamera[mCurrentCamera]->Update(mWindow->getHandle()->getHandle());
        //eCamera[mCurrentCamera]->UpdateMatrices(0, WIDTH, HEIGHT);
        mCameraData->Init(&eCamera[mCurrentCamera]->CameraData);
        sun->Update();

        RenderGlobalIllumination();

        for (int a = 0; a < 3; a++)
            for (int b = 0; b < 3; b++)
                for (int c = 0; c < 3; c++)
                {
                    glm::vec3 tPosition = glm::vec3(0, 10.0, 0);
                    glm::vec3 positions = tPosition + glm::vec3(a, b, c) * glm::vec3(2.0, 2.0, 2.0);
                    glm::vec3 tScale = glm::vec3((random_double() * 2.0) + 0.1);
                    glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
                    std::string tModelName = "models/cube.eml";

                    std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(positions, tScale, tRotation);

                    auto _RComp = std::make_shared<Component::RenderComponent>(tModelName, positions, "Main");
                    _RComp->CastsShadows(true);
                    _RComp->setTransparency(false);
                    _RComp->setRenderId(EntityList.size());
                    MIN_MAX_POINTS _BoundingBox = ResourceManager::Get().getModelBoundingBox(tModelName);
                    auto _PComp = std::make_shared<Component::PhysicComponent>(100, positions, tScale, Physics::Type::CUBE, _BoundingBox);
                    _PComp->setID(EntityList.size());
                    //_PComp->setState(true);
                    _Entity->addComponent(_RComp)->addComponent(_PComp);

                    _Entity->setName(std::string("Entity") + std::to_string(EntityList.size()));
                    EntityList.push_back(_Entity);
                }

        { 
            /* glm::vec3 initCubemapPosition = glm::vec3(-35.0, 0, -32.0);
            for (int a = 0; a < 7; a++)
                for (int b = 0; b < 7; b++)
                    for (int c = 0; c < 7; c++)
                    {
                        glm::vec3 positions = initCubemapPosition + glm::vec3(a, b, c) * glm::vec3(10.0, 5.0, 10.0);
                        glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
                        glm::vec3 tScale = glm::vec3(0.5);
                        std::string tModelName = "models/probe.eml";

                        std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(positions, tScale, tRotation);
                        Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, positions, "Main");
                        static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(false);
                        static_pointer_cast<Component::RenderComponent>(_RComp)->isDoubleFaced(false);
                        static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
                        static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(false);

                        _Entity->addComponent(_RComp);
                        EntityList.push_back(_Entity);
                    }*/
        }

        /* {
            glm::vec3 positions = glm::vec3(-20, 10, 2);
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
        }*/

        //ResourceManager::Get().loadQueuedModels();
        //ResourceManager::Get().loadQueuedTextures();

        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            EntityList[i]->setShader("Main");
            if (EntityList[i]->HasClothComponent())
                EntityList[i]->setShader("Cloth");
        }

        Editor::GUI::sSelectedEntity = EntityList.at(0);
        /* 
        glGenBuffers(1, &AmbientLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, AmbientLightSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SphericalHarmonics::SphericalHarmonicsFormat) * 7 * 7 * 7, (const void *)&sphStruct, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AmbientLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
*/
        glEnable(GL_CULL_FACE);
        glDepthFunc(GL_LESS);
        ResourceManager::Get().LaunchIntervals();
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

        Shaders["HBIL"] = std::make_shared<Shader>("shaders/SSAO.vglsl", "shaders/HBIL.glsl");

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

        /*std::cout << "Llega" << std::endl;
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
        }*/

        //std::cout << "Resource manager in epsilon address: " << rM.get() << std::endl;

        ResourceManager::Get().requestCubeMap(50000, glm::vec3(4.8, 80000.2, -8));
        ResourceManager::Get().requestModel("models/esfera.eml" /*, glm::vec3(78.0, 5.25, -57), glm::vec3(1), glm::quat(0.0, 0.0, 0.0, 0.0)*/);
        ResourceManager::Get().requestModel("models/sphere.eml" /*, glm::vec3(78.0, 5.25, -57), glm::vec3(1), glm::quat(0.0, 0.0, 0.0, 0.0)*/);
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
        BSPMap = std::make_shared<CQuake3BSP>();

        BSPMap->LoadBSP((string("maps/") + "GI.bsp").c_str());

        m_AnimModel = std::move((shared_ptr<MD5Model>)(new MD5Model()));

        m_AnimModel->LoadModel("models/hellknight/hellknight.md5mesh");

        m_AnimModel->LoadAnim("models/hellknight/walk7.md5anim");
        m_AnimModel->LoadAnim("models/hellknight/idle2.md5anim");
    }

    void Epsilon::LoadSound(void)
    {
        cout << "Loading Sound..." << endl;
        /*
        m_AudioSystem = (std::shared_ptr<IO::Audio::Audio>)new IO::Audio::Audio();
        /*
        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement = (std::shared_ptr<IO::Audio::AudioElement>) new IO::Audio::AudioElement("sound/File0279.wav", STATIC_SOUND, glm::vec3(-28, 10, 15), glm::vec3(0,0,0));
        m_AudioSystem->addAudioElement(3, m_AudioElement);
    */
        /*
        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement2 = (std::shared_ptr<IO::Audio::AudioElement>)new IO::Audio::AudioElement("sound/ambient.wav", MUSIC, glm::vec3(-28, 10, 15), glm::vec3(0, 0, 0));
        m_AudioSystem->addAudioElement(0, m_AudioElement2);

        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement3 = (std::shared_ptr<IO::Audio::AudioElement>)new IO::Audio::AudioElement("sound/flag.wav", STATIC_SOUND, glm::vec3(96, 16, 6), glm::vec3(0, 0, 0));
        m_AudioSystem->addAudioElement(1, m_AudioElement3);

        std::shared_ptr<IO::Audio::AudioElement> m_AudioElement4 = (std::shared_ptr<IO::Audio::AudioElement>)new IO::Audio::AudioElement("sound/hover.wav", MENU_SOUND, glm::vec3(96, 16, 6), glm::vec3(0, 0, 0));
        m_AudioSystem->addAudioElement(2, m_AudioElement4);

        m_AudioListener = (std::shared_ptr<IO::Audio::AudioListener>)new IO::Audio::AudioListener();*/
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
                shader->PushUniform("ambientDivider", Editor::GUI::ambientDivider);
                shader->PushUniform("FrameNumber", (int)mWindow->FrameNumber());

                auto c = EntityList[i]->getComponent<Component::RenderComponent_ptr>();

                if (c->hasModel)
                    ResourceManager::Get().setModelUniforms(EntityList[i]->getModelPath(), shader, EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation(),
                                                            EntityList[i]->getPrevPosition(), EntityList[i]->getPrevScale(), EntityList[i]->getPrevRotation(),
                                                            eCamera[mCurrentCamera]);
                else
                    this->SetUniforms(shader, EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation());
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
        Frustum.CalculateFrustum(glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix()), BSPmodel);
        shader->Use();
        this->SetUniforms(shader, glm::vec3(0.0), glm::vec3(0.1), glm::quat(0, 0, 0, 0));
        BSPMap->RenderLevel(eCamera[mCurrentCamera]->getPosition(), shader, true, &Frustum);

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

            shader->Use();
            shader->PushUniform("FrameNumber", (int)mWindow->FrameNumber());
            this->SetUniforms(shader, EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation());
            
            if(RComponent->hasModel) {
                auto tModel = ResourceManager::Get().getModel(EntityList[i]->getModelPath() /*, shader, EntityList[i]->getPosition()*/);
                tModel->Draw(shader);
            } else {
                RComponent->mDrawable->Draw(shader);
            }
        }

        //mPatch->updateVertexBuffers(mCloth->getVertices());
        //this->SetUniforms(Shaders["ShadowMapping"], glm::vec3(0.0), glm::vec3(1.0), glm::quat(0.0f, 0.0, 0.0, 0.0));
        //mPatch->Render(Shaders["ShadowMapping"], eCamera->getViewMatrix(), eCamera->getProjectionMatrix());

        glDisable(GL_CULL_FACE);

        glm::mat4 BSPmodel = glm::mat4(1.0);
        //glm::mat4 tmodel = glm::translate(glm::mat4(1.0), glm::vec3(-30.0, 5.0, -120.0));
        glm::mat4 sModel = glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1));
        BSPmodel = sModel;
        Frustum.CalculateFrustum(glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix()), BSPmodel);
        //shader->Use();

        shader->PushUniform("FrameNumber", (int)mWindow->FrameNumber());
        this->SetUniforms(shader, glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1, 0.1, 0.1), glm::quat(0.0, 0.0, 0.0, 0.0));
        BSPMap->RenderLevel(eCamera[mCurrentCamera]->getPosition(), shader, false, &Frustum);

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

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            Editor::GUI::MainWindow(EntityList);

            /*Begin skybox window*/
            Editor::GUI::SkySettings(skybox, sun);
            /*End Skybox window*/

            /*Begin scene window*/

            Editor::GUI::DebugView(PP, mDefaultFrameBuffer, eCamera[mCurrentCamera], Shaders["ImGui_gamma"]);
            Editor::GUI::MainViewport(mDefaultFrameBuffer, eCamera[mCurrentCamera]);

            Editor::GUI::SceneHierarchy(EntityList);

            Editor::GUI::TextureList();

            Editor::GUI::ShaderView(Shaders);

            //Editor::GUI::AudioControl(m_AudioSystem);

            Editor::GUI::FileSystemWindow();

            Editor::GUI::PostprocessSettings(PP);

            Editor::GUI::EntityProperties(eCamera[mCurrentCamera]);

            Editor::GUI::Render(mWindow);

            if (Editor::GUI::sSelectedEntityIndex != -1)
            {
                Editor::GUI::sSelectedEntity = EntityList[Editor::GUI::sSelectedEntityIndex];
            }
        }
    }

    void Epsilon::ProcessAudio()
    {
        /*
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

        m_AudioSystem->PlayAudio();*/
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

        glfwPollEvents();
        Editor::GUI::PollEvents(mDefaultFrameBuffer, PP);

        if (Editor::GUI::mShouldRenewGI)
        {
            RenderGlobalIllumination();
            Editor::GUI::mShouldRenewGI = false;
        }

        Input::Joystick::JoystickManager::DetectJoysticks();
        Input::Joystick::JoystickManager::PollJoystick();
        auto _Joystick = Input::Joystick::JoystickManager::PrimaryJoystick();

        if (glm::abs((timeGUI * 60) - (etime * 60)) > 2.0f && onMenu)
        {
            timeGUI = this->etime;
        }

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::ESCAPE] || _Joystick->getJoystickButton(_Joystick->getMapping()->START()))
        {
            if (glm::abs((menuTime * 60) - (etime * 60)) > 60.0f)
            {
                onMenu = !onMenu;
                menuTime = this->etime;
            }
        }

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::N])
            normal = !normal;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::F])
            flashLight = !flashLight;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::SPACE])
        {
            PP->HBAOOn = !PP->HBAOOn;
            SSAO = !SSAO;
        }

        auto resolveTime = [kt = KeyTime, t = etime](int key) -> bool
        { return glm::abs((kt[key] * 60) - (t * 60)) > 60.0f; };

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::C] && resolveTime(Input::GLFW::Key::C))
        {
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
                glfwSetCursorPos(mWindow->getHandle()->getHandle(), CursorBeforeMenu.x, CursorBeforeMenu.y);
                mWindow->HideCursor();
                this->m_CameraMode = NO_CLIP;
                onMenu = false;
            }
            else
            {
                CursorBeforeMenu.x = XPOS;
                CursorBeforeMenu.y = YPOS;

                XPOS = this->WIDTH / 2;
                YPOS = this->HEIGHT / 2;
                mWindow->ShowCursor();
                glfwSetCursorPos(mWindow->getHandle()->getHandle(), this->WIDTH / 2, this->HEIGHT / 2);
                this->m_CameraMode = CAMERA_FIXED;
                mLastCameraDirection = eCamera[mCurrentCamera]->getDirection();
                mLastCameraPosition = eCamera[mCurrentCamera]->getPosition();
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
    }

    void Epsilon::onRender()
    {
        this->ClearBuffers();

        this->Clock();

        this->PollEvents();

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

            Frustum.CalculateFrustum(glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix()), Model);
            bool visible = Frustum.BoxInFrustum(EntityList[i]->getBoundingBox());

            if (visible)
            {
                auto &ref = ResourceManager::Get();
                auto model = ref.getModel(rComponent->getModelPath());

                if (model != nullptr)
                {
                    Frustum.CalculateFrustum(glm::mat4(eCamera[mCurrentCamera]->getProjectionMatrix() * eCamera[mCurrentCamera]->getViewMatrix()), Model);
                    for (unsigned int j = 0; j < model->Meshes().size(); j++)
                    {
                        auto BB = model->getMeshBoundingBox(j, EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation());
                        auto vis = Frustum.BoxInFrustum(BB);

                        model->setMeshVisibility(j, vis);
                    }
                }
            }

            rComponent->setVisibility(visible);
        }

        //auto prev_pos = EntityList[1]->getPosition();
        //EntityList[1]->setPosition(glm::vec3(prev_pos.x, prev_pos.y + glm::sin(Clock::TimeSeconds()), prev_pos.z));
    }

    void Epsilon::SwapBuffers(void)
    {
        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            EntityList[i]->PostUpdate();
        }
        //glfwSwapBuffers(this->mWindow->getHandle()->getHandle());
        this->mWindow->IncrementFrame();
        this->mWindow->SwapBuffers();
        lastTime = etime;

        auto &ref = ResourceManager::Get();

        if (ref.ShouldLoadQueuedTextures())
        {
            ref.loadQueuedTextures();
        }

        if (ref.ShouldLoadQueuedModels())
        {
            ref.loadQueuedModels();
        }
    }

    void Epsilon::ClearBuffers(void)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Epsilon::ComputeCamera(CAMERA_MODE mode, glm::vec3 position, glm::vec3 direction, glm::mat4 proj, glm::mat4 view)
    {

        if (mode == PLAYER_CONTROLLED)
        {
            this->eCamera[mCurrentCamera]->Update(this->mWindow->getHandle()->getHandle());
            m_PlayerCapsule->preStep();
            m_PlayerCapsule->Movement(this->eCamera[mCurrentCamera], frametime);
            eCamera[mCurrentCamera]->setPosition(m_PlayerCapsule->getPosition());
        }
        else if (mode == CAMERA_FIXED)
        {
            //eCamera[mCurrentCamera]->getDirection()
            //eCamera[mCurrentCamera]->setPosition(mLastCameraPosition);
            //eCamera[mCurrentCamera]->setDirection(mLastCameraDirection);
        }
        else if (mode == CAMERA_OVERRIDE)
        {

            this->eCamera[mCurrentCamera]->Update(this->mWindow->getHandle()->getHandle());
            eCamera[mCurrentCamera]->setPosition(position);
            eCamera[mCurrentCamera]->setDirection(direction);
            eCamera[mCurrentCamera]->setViewMatrix(view);
            eCamera[mCurrentCamera]->setProjection(proj);
        }
        else if (mode == NO_CLIP)
        {
            this->eCamera[mCurrentCamera]->Update(this->mWindow->getHandle()->getHandle());
            /*default mode: everything is calculated automatically*/
        }
        else
        {
        }

        if (PP)
        {

            eCamera[mCurrentCamera]->jitter_multiplier = PP->getPostProcessData().AntiAliasingSettings.JitterMultiplier;
            this->eCamera[mCurrentCamera]->UpdateMatrices(mWindow->FrameNumber(), WIDTH, HEIGHT, PP->getPostProcessData().AntiAliasingSettings.Active);
        }
        else
            this->eCamera[mCurrentCamera]->UpdateMatrices(mWindow->FrameNumber(), WIDTH, HEIGHT);

        eCamera[mCurrentCamera]->CameraData.CurrentSelectedEntity = Editor::GUI::sSelectedEntityIndex;
        mCameraData->Update(sizeof(Camera::CameraData_t), 0, (const void *)&eCamera[mCurrentCamera]->CameraData);
        mCameraData->Bind();
    }

    void Epsilon::ComputeShadow()
    {

        for (auto &ent : EntityList)
        {
            if (ent->HasPhysicComponent())
            {
                auto comp = ent->getComponent<Component::PhysicComponent_ptr>();
                comp->addForce(glm::vec3(glm::sin(Clock::Time()) * 10, 10, 2));
            }
        }

        glm::vec3 camPos = eCamera[mCurrentCamera]->getPosition();
        glm::vec3 camDir = eCamera[mCurrentCamera]->getDirection();

        shadowMap->setShadowPosition(glm::vec3(camPos.x + camDir.x * 30, camPos.y + 45.0f, camPos.z + camDir.z * 30));

        shadowMap->setShadowDirection(sun->Direction);

        shadowMap->SetupShadowMatrices();

        shadowMap->Begin(0);
        Shaders["ShadowMapping"]->Use();

        if (sun->height > 0.0)
            this->RenderShadows(Shaders["ShadowMapping"]);
        shadowMap->End();

        Shaders["ShadowMapping"]->Free();

        //------------- Point Shadow ---------------
        mPointShadow->updateMatrices(PP->getLight(0).position);
        mPointShadow->Begin(0);

        RenderShadows(ResourceManager::Get().getShader("PointShadow"));

        mPointShadow->End();
    }

    void Epsilon::ProcessFrame(void)
    {
        PP->beginOffScreenrendering();

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

        PP->endOffScreenRendering();

        if (this->SSAO)
            PP->applySSAO(this->eCamera[mCurrentCamera]);

        PP->applyHBIL(this->eCamera[mCurrentCamera], shadowMap, Shaders["HBIL"], mPointShadow);
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
        glBindTexture(GL_TEXTURE_2D, PP->gBufferFramebuffer->getRenderTargetHandler(PostProcess::GBUFFER_TARGETS::GBUFFER_IBL_DEPTH));
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
        glDisable(GL_BLEND);
        /*
        
        auto &ref = ResourceManager::Get();
        this->waterPlane->RenderWater(  eCamera[0], 
                                        PP->CopyTextureFBO->getRenderTargetHandler(0), 
                                        glm::normalize(glm::vec3(83, 6, -3) - glm::vec3(0, 6, -3)), 
                                        PP->gBufferFramebuffer->getRenderTargetHandler(PostProcess::GBUFFER_TARGETS::GBUFFER_IBL_DEPTH), 
                                        ref.useCubeMap(0));
        */

        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        if (Editor::GUI::sSelectedEntityIndex >= 0)
            mGizmo->Render(Editor::GUI::gizmo_status, EntityList[Editor::GUI::sSelectedEntityIndex]->getPosition(), this->eCamera[mCurrentCamera]->getPosition());
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);

        //this->RenderParticles();
        PP->ShowPostProcessImage(this->frametime, (int)this->onMenu, this->sun->Direction, this->eCamera[mCurrentCamera], mDefaultFrameBuffer);
        glEnable(GL_BLEND);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //Blit to named default framebuffer

        //glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        //mDefaultFrameBuffer->setToDraw();

        //glBlitFramebuffer(0, 0, this->WIDTH, this->HEIGHT, 0, 0, this->WIDTH, this->HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Epsilon::onExit()
    {

        BSPMap->Destroy();

        sun->Destroy();

        PP->Destroy();

        cout << "Epsilon Engine has closed Succesfully." << endl;

        Log::WriteToLog("Epsilon Engine has closed Succesfully.");
    }

    void Epsilon::RenderGlobalIllumination()
    {

        std::cout << "Begining Global Illumination rendering" << std::endl;
        glm::vec3 initCubemapPosition = glm::vec3(-35.0, 0, -32.0);
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
        SphericalHarmonics sph;
        SphericalHarmonics::SphericalHarmonicsFormat sphStruct[7][7][7];
        auto start = std::chrono::steady_clock::now();
        //Shaders["CubeMap"]->Use();
        int index = 0;
        int id = 0;

        try
        {
            for (int a = 0; a < 7; a++)
            {
                for (int b = 0; b < 7; b++)
                {
                    for (int c = 0; c < 7; c++)
                    {
                        if (this->mCubemap[a][b][c] == nullptr)
                        {
                            std::cout << "creating new cubemap" << std::endl;
                            this->mCubemap[a][b][c] = std::make_shared<CubeMap>(id, initCubemapPosition + glm::vec3(a, b, c) * glm::vec3(10.0, 5.0, 10.0));
                        }
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
                                EntityList[i]->setShader("Main");
                            }
                            glDisable(GL_CULL_FACE);
                            glCullFace(GL_FRONT);
                            /****************************************************************************/

                            glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(0.1));
                            glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0));
                            glm::mat4 cModel = TranslationMatrix * ScaleMatrix;

                            cubeShader->PushUniform("model", cModel);
                            glCullFace(GL_FRONT);
                            Frustum.CalculateFrustum(glm::mat4(this->mCubemap[a][b][c]->captureProjection * this->mCubemap[a][b][c]->captureViews[index]), cModel);

                            glUniformMatrix4fv(glGetUniformLocation(cubeShader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);
                            glUniformMatrix4fv(glGetUniformLocation(cubeShader->getProgramID(), "depthBias"), 1, GL_FALSE, &shadowMap->getBiasMatrix()[0][0]);
                            cubeShader->PushUniform("lightDir", sun->Direction);
                            glActiveTexture(GL_TEXTURE5);
                            cubeShader->PushUniform("shadowMap", 5);
                            glBindTexture(GL_TEXTURE_2D, shadowMap->getShadowTextureID());
                            BSPMap->RenderLevel(mCubemap[a][b][c]->getPosition(), cubeShader, true, &Frustum);
                            cubeShader->Free();
                        }

                        mCubemap[a][b][c]->End();
                        //mCubemap[a][b][c]->genAmbientConvolution();
                        if (ResourceManager::Get().cubemapsLoaded == false)
                            ResourceManager::Get().addCubemap(this->mCubemap[a][b][c], this->mCubemap[a][b][c]->getPosition());
                        else
                            ResourceManager::Get().updateCubemap(this->mCubemap[a][b][c]->getID(), this->mCubemap[a][b][c]);
                        sph.CalculateCohefficients(this->mCubemap[a][b][c]->getTextureID(), 3);
                        sph.setId(this->mCubemap[a][b][c]->getID());
                        sphStruct[a][b][c] = sph.toStruct();

                        index++;
                        id++;
                    }
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught!\n what(): " << e.what() << std::endl;
        }
        auto finish = std::chrono::steady_clock::now();
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        std::cout << "Cubemap generation took: " << interval << " milliseconds" << std::endl;
        std::cout << interval / (7 * 7 * 7) << " per cubemap\n"
                  << std::flush;

        auto SSBOSize = sizeof(SphericalHarmonics::SphericalHarmonicsFormat) * 7 * 7 * 7;

        if (AmbientLightSSBO == nullptr)
        {
            AmbientLightSSBO = std::make_shared<Renderer::ShaderStorage>(SSBOSize, 1);
            AmbientLightSSBO->Init(&sphStruct);
        }
        else
        {
            AmbientLightSSBO->Update(SSBOSize, 0, &sphStruct);
        }

        ResourceManager::Get().cubemapsLoaded = true;

        glCullFace(GL_BACK);
    }
} // namespace Epsilon