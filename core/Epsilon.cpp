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
#include "../../version.h"
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

        m_CameraMode = PLAYER_CONTROLLED;

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

        shadowMap = std::move((shared_ptr<ShadowMap>)(new ShadowMap(DATA.SHADOWMAP_SIZE, DATA.SHADOWMAP_SIZE, -20.0f, 80.0f)));

        //mPointShadow = std::make_shared(PointShadow,glm::vec3(0.,.0,.0));
        //mPointShadow->Setup();

        //rM = std::make_shared<ResourceManager>();

        //rM =

        //rM->m_PhysicsWorld = std::make_shared<Physics::Physics>();
        //ResourceManager::Get().getPhysicsWorld() = std::make_shared<Physics::Physics>();
        /*
    std::shared_ptr<EntityBase> tmpEnt;
    std::shared_ptr<Component::RenderComponent> Compmodel;

    {
        std::shared_ptr<EntityBase> tmpEnt;
        tmpEnt = (std::shared_ptr<EntityBase>)(new EntityBase(rM, glm::vec3(10, 3.5, 10), glm::vec3(2.0), glm::quat(-1.0, 0.0, 0.0, 0.0)));
        std::shared_ptr<Component::ClothComponent> ClothComponent = (std::shared_ptr<Component::ClothComponent>)new Component::ClothComponent(eCamera);
        mCloth = (std::shared_ptr<Physics::ClothPhysicObject>)new Physics::ClothPhysicObject();
        rM->m_PhysicsWorld->getSoftDynamicsWorld()->addSoftBody(mCloth->addObject(rM->m_PhysicsWorld->softBodyWorldInfo, glm::vec3(10, 3.5, 10), 5, 15, 15, 1 + 2).get());
        //mCloth->setWind(btVector3(0,0,1), 0.5);
        ClothComponent->Fill(mCloth);
        ClothComponent->setShader("Cloth");
        tmpEnt->addComponent(ClothComponent);
        EntityList.push_back(tmpEnt);
    }
*/
        //RenderSplashScreen("Loading Shaders...");
        this->LoadShaders();

        //RenderSplashScreen("Loading Geometry...");
        this->LoadGeometry();

        this->LoadSound();

        std::vector<string> modelsNames;
        std::vector<glm::vec3> modelsPositions;
        std::vector<float> modelsScales;
        /*
            modelsNames.push_back("models/tree_2.eml");
            modelsPositions.push_back(glm::vec3(77, -20, -7));
            modelsScales.push_back(3.0);

            modelsNames.push_back("models/Tree.eml");
            modelsPositions.push_back(glm::vec3(80, -20, 35));
            modelsScales.push_back(3.0);*/

        /*
        for(int i = 0; i < modelsNames.size(); ++i) {
            std::shared_ptr<EntityBase> tmpEnt;
            tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, modelsPositions.at(i), glm::vec3(modelsScales.at(i)), glm::quat(0.5, 0.0, 0.0, 0.0)));
            std::shared_ptr<Component::RenderComponent> Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
            Compmodel->Fill(modelsNames.at(i), rM, "Main");

            std::shared_ptr<Component::PhysicComponent> CompPhys = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();

            std::shared_ptr<Physics::CubePhysicObject> ph = (std::shared_ptr<Physics::CubePhysicObject>) new Physics::CubePhysicObject();
            rM->m_PhysicsWorld->world->addRigidBody(ph->addObject(modelsPositions.at(i), 10.0, rM->getModelBoundingBox(modelsNames.at(i)), modelsScales.at(i)).get());
            CompPhys->Fill(100.0f, ph);

            tmpEnt->addComponent(Compmodel);
            tmpEnt->addComponent(CompPhys);

            EntityList.push_back(tmpEnt);
        }*/
        /*
    for(int i = 0; i < 5; i++) {
        std::shared_ptr<EntityBase> tmpEnt;
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-5.5+(i*6.4),9.0,-2), glm::vec3(0.25), glm::quat(-1.0, 0.0, 0.0, 0.0)));
        std::shared_ptr<Component::RenderComponent> Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/rock.eml", rM, "Main");

        std::shared_ptr<Component::PhysicComponent> CompPhys = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();

        std::shared_ptr<Physics::CubePhysicObject> ph = (std::shared_ptr<Physics::CubePhysicObject>) new Physics::CubePhysicObject();
        //rM->m_PhysicsWorld->world->addRigidBody(ph->addObject(2.0, glm::vec3(-20.5+(i*6.4),9.0,-8), 50.0f).get());
        rM->m_PhysicsWorld->world->addRigidBody(ph->addObject(glm::vec3(-5.5+(i*6.4),9.0,-2), 100.0f, rM->getModelBoundingBox("models/rock.eml"), 0.25f).get());
        CompPhys->Fill(100.0f, ph);

        //Compmodel->isTransparent = true;
        tmpEnt->addComponent(Compmodel);
        tmpEnt->addComponent(CompPhys);

        EntityList.push_back(tmpEnt);
    /*}

    /*
        std::shared_ptr<EntityBase> tmpEnt;
        std::shared_ptr<Component::RenderComponent> Compmodel;*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(4.0, -1.2, 9.0), glm::vec3(0.6, 0.85, 0.6), glm::quat(1.0, 0.0, 0.6, 0.0)));
    std::shared_ptr<Component::RenderComponent> Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel->Fill("models/rock_wall.eml", rM, "Main");
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);


    tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-1.3, -1.2, -5.0), glm::vec3(0.6, 0.85, 0.6), glm::quat(-0.6, 0.0, 1.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel->Fill("models/rock_wall.eml", rM, "Main");
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);
    */
        /*
    for(int i = 0; i < 2; i++) {
        for(int j = 0; j < 2; j++) {
            tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-30+i*50, 1.3, 15-j*20), glm::vec3(0.125), glm::quat(-1.0, 0.0, 0.0, 0.0)));
            Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
            Compmodel->Fill("models/grass.eml", rM, "Main");
            Compmodel->isDoubleFaced = true;
            tmpEnt->addComponent(Compmodel);
            EntityList.push_back(tmpEnt);
        }
    }*/
        /*
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(14, 10.0, 0.0), glm::vec3(4.0), glm::quat(-1.0, 0.0, 0.0, 0.0)));
        Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/vrdemo.eml", rM, "Main");
        tmpEnt->addComponent(Compmodel);
        EntityList.push_back(tmpEnt);
    /*
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(0,4.3, 3), glm::vec3(4.0), glm::quat(-1.0, 0.0, 1.0, 0.0)));
        Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/cerberus.eml", rM, "Main");
        tmpEnt->addComponent(Compmodel);
        EntityList.push_back(tmpEnt);*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>)(new EntityBase(rM, glm::vec3(-14, 2.5, -17), glm::vec3(2.0), glm::quat(-1.0, 0.0, 1.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>)new Component::RenderComponent();
    Compmodel->Fill("models/esfera.eml", rM, "Main");
    std::shared_ptr<Component::MovementComponent> Compmov;
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);
*/
        /*
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(22, 8.0, 6), glm::vec3(2.0), glm::quat(-1.0, 0.0, -1.0, 0.0)));
        Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/Bigger_tree.eml", rM, "Main");
        Compmodel->isDoubleFaced = true;
        tmpEnt->addComponent(Compmodel);
        EntityList.push_back(tmpEnt);*/
        /*
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(52, 0.0, -10.0), glm::vec3(2.0), glm::quat(1.0, 0.0, 0.0, 0.0)));
        Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/tree_o.eml", rM, "Main");
        tmpEnt->addComponent(Compmodel);
        EntityList.push_back(tmpEnt);
*/
        /*
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(22, 2.0, -20.0), glm::vec3(2.0), glm::quat(1.0, 0.0, 0.0, 0.0)));
        Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/export3dcoat.eml", rM, "Main");
        tmpEnt->addComponent(Compmodel);
        EntityList.push_back(tmpEnt);*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>)(new EntityBase(rM, glm::vec3(-12, 1.65, -3), glm::vec3(0.05), glm::quat(-1.0, 0.0, 1.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>)new Component::RenderComponent();
    Compmodel->Fill("models/full_rock.eml", rM, "Main");
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);*/
        /*

    /*
        	tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(34, 3.8, 3.5), glm::vec3(3.0), glm::quat(-1.0, 0.0, 1.0, 0.0)));
        	Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        	Compmodel->Fill("models/swords.eml", rM, "Main");
        	tmpEnt->addComponent(Compmodel);
        	EntityList.push_back(tmpEnt);

*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>)(new EntityBase(rM, glm::vec3(-12, 3.8, -10), glm::vec3(1.0), glm::quat(1.0, 0.0, 1.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>)new Component::RenderComponent();
    Compmodel->Fill("models/chutulu.eml", rM, "Main");
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);*/
        /*
        	tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(23, 8.25+6.2, 14.5), glm::vec3(1.0), glm::quat(-1.0, 1.0, 1.0, 0.5)));
        	Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        	Compmodel->Fill("models/ember.eml", rM, "Main");
        	tmpEnt->addComponent(Compmodel);
        	EntityList.push_back(tmpEnt);*/
        /*
    	tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(21, 1.5, -7), glm::vec3(2.0), glm::quat(-1.0, 0.0, 1.0, 0.0)));
    	Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    	Compmodel->Fill("models/Tree.eml", rM, "Main");
    	tmpEnt->addComponent(Compmodel);
    	EntityList.push_back(tmpEnt);

    	tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(21, 1.5, 14), glm::vec3(2.0), glm::quat(-1.0, 0.0, -1.0, 0.0)));
    	Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    	Compmodel->Fill("models/Tree.eml", rM, "Main");
    	tmpEnt->addComponent(Compmodel);
    	EntityList.push_back(tmpEnt);*/

        ///godrays tutorial begin
        /*
    	tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-20, 2.0, 0.0), glm::vec3(0.06, 0.1, 0.06), glm::quat(-1.0, 0.0, 0.0, 0.0)));
    	Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    	Compmodel->Fill("models/grass.eml", rM, "Main");
    	tmpEnt->addComponent(Compmodel);
    	EntityList.push_back(tmpEnt);*/
        /**/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(6, 27, 15.0), glm::vec3(2.0), glm::quat(1.0, 0.0, 0.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel->Fill("models/BigBell.eml", rM, "Main");
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);
*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-13, 8.0, -3), glm::vec3(0.5), glm::quat(1.0, 0.0, 0.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel->Fill("models/zweihander.eml", rM, "Main");
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-58, 0.5, -3), glm::vec3(1.5), glm::quat(1.0, 0.0, 0.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel->Fill("models/column.eml", rM, "Main");
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);*/
        /*
        	tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-18, 100.0, 0.0), glm::vec3(0.025), glm::quat(0.0, 0.0, 0.0, 0.0)));
        	Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        	Compmodel->Fill("models/sponza.eml", rM, "Main");
        	tmpEnt->addComponent(Compmodel);
        	EntityList.push_back(tmpEnt);
*/

        /*
        	tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-20, 0.8, -15.0), glm::vec3(4.0), glm::quat(-1.0, 0.0, 1.0, 0.0)));
        	Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        	Compmodel->Fill("models/case.eml", rM, "Main");
        	tmpEnt->addComponent(Compmodel);
        	EntityList.push_back(tmpEnt);
*/
        /*
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-120, -4.5, 13.5), glm::vec3(5.5), glm::quat(0.0, 0.0, 1.0, 0.0)));
        Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/woodentable.eml", rM, "Main");
        tmpEnt->addComponent(Compmodel);
        EntityList.push_back(tmpEnt);*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>)(new EntityBase(rM, glm::vec3(-12, 4.0, 7), glm::vec3(1.5f), glm::quat(0.0, 0.0, 1.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>)new Component::RenderComponent();
    Compmodel->Fill("models/Tree.eml", rM, "Main");
    Compmodel->isDoubleFaced = true;
    tmpEnt->addComponent(Compmodel);
    EntityList.push_back(tmpEnt);*/

        ///godrays tutorial end

        /*
        tmpEnt = (std::shared_ptr<EntityBase>) (new EntityBase(rM, glm::vec3(-114, 1, 11.0), glm::vec3(2), glm::quat(-0.5, 0.0, -1.0, 0.0)));
        Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
        Compmodel->Fill("models/oldtv.eml", rM, "Main");
    //Compmodel->isTransparent = true;

        tmpEnt->addComponent(Compmodel);
        EntityList.push_back(tmpEnt);*/
        /*
    tmpEnt = (std::shared_ptr<EntityBase>)(new EntityBase(rM, glm::vec3(0.0f, 15.0f, -5.0f), glm::vec3(1.0), glm::quat(1.0, 0.0, 0.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>)new Component::RenderComponent();
    Compmodel->Fill("models/shape_mat.eml", rM, "Main");*/
        //std::shared_ptr<Component::PhysicComponent> CompPhys = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();
        //std::shared_ptr<Physics::CubePhysicObject> ph = (std::shared_ptr<Physics::CubePhysicObject>) new Physics::CubePhysicObject();
        //rM->m_PhysicsWorld->world->addRigidBody(ph->addObject(glm::vec3(16, 15, 10), 100.0, rM->getModelBoundingBox("models/shape_mat.eml"), 0.25).get());
        //CompPhys->Fill(100.0f, ph);
        /* tmpEnt->addComponent(Compmodel);
    //tmpEnt->addComponent(CompPhys);
    EntityList.push_back(tmpEnt);*/

        /*
    ph->addObject(2.0, glm::vec3(-20.5+(i*6.4),8.2,-8), 2.0).get()

        EntityList[7]->addComponent(Compmodel2);
        EntityList[7]->addComponent(CompPhys2);*/
        /////////////////////////////////////////////////////////
        /*tmpEnt = (std::shared_ptr<EntityBase>)(new EntityBase(rM, glm::vec3(22, 5.5, -25.5), glm::vec3(5), glm::quat(-1.0, 0.0, -1.0, 0.0)));
    Compmodel = (std::shared_ptr<Component::RenderComponent>)new Component::RenderComponent();
    Compmodel->Fill("models/platform.eml", rM, "Main");

    std::shared_ptr<Component::PhysicComponent> CompPhys = (std::shared_ptr<Component::PhysicComponent>)new Component::PhysicComponent();
    CompPhys = (std::shared_ptr<Component::PhysicComponent>)new Component::PhysicComponent();

    std::shared_ptr<Physics::CubePhysicObject> ph3 = (std::shared_ptr<Physics::CubePhysicObject>)new Physics::CubePhysicObject();
    rM->m_PhysicsWorld->world->addRigidBody(ph3->addObject(glm::vec3(22, 5.5, -25.5), 0.0, rM->getModelBoundingBox("models/platform.eml"), 5.0f).get());
    CompPhys->Fill(0.0f, ph3);

    std::shared_ptr<Component::MovementComponent> MovComp = (std::shared_ptr<Component::MovementComponent>)new Component::MovementComponent();
    MovComp->Fill(ph3, glm::vec3(22, 8.0, -25.5), glm::vec3(22, 21.5, -25.5), 9.0, true);

    tmpEnt->addComponent(Compmodel);
    tmpEnt->addComponent(CompPhys);
    tmpEnt->addComponent(MovComp);
    EntityList.push_back(tmpEnt);*/

        {
            glm::vec3 tPosition = glm::vec3(5, 15, 5);
            glm::vec3 tScale = glm::vec3(2.0f);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/android.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(true);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(true);
            MIN_MAX_POINTS _BoundingBox = ResourceManager::Get().getModelBoundingBox(tModelName);
            Component::Component_ptr _PComp = std::make_shared<Component::PhysicComponent>(100, tPosition, tScale, Physics::Type::CUBE, _BoundingBox);
            //Component::Component_ptr _SComp = std::make_shared<Component::ScriptComponent>("scripts/test.lua");
            _Entity->addComponent(_RComp)->addComponent(_PComp)/*->addComponent(_SComp)*/;

            EntityList.push_back(_Entity);
        }
        
        {
            glm::vec3 tPosition = glm::vec3(10, 15, 10);
            glm::vec3 tScale = glm::vec3(2.0f);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/esfera.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(true);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
            MIN_MAX_POINTS _BoundingBox = ResourceManager::Get().getModelBoundingBox(tModelName);
            Component::Component_ptr _PComp = std::make_shared<Component::PhysicComponent>(100, tPosition, tScale, Physics::Type::SPHERE, _BoundingBox);
            //Component::Component_ptr _SComp = std::make_shared<Component::ScriptComponent>("scripts/test.lua");
            _Entity->addComponent(_RComp)->addComponent(_PComp)/*->addComponent(_SComp)*/;

            EntityList.push_back(_Entity);
        }
        
        {
            glm::vec3 tPosition = glm::vec3(0, 8, 0);
            glm::vec3 tScale = glm::vec3(2.05);
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);
            std::string tModelName = "models/map.eml";

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, tScale, tRotation);

            Component::Component_ptr _RComp = std::make_shared<Component::RenderComponent>(tModelName, tPosition, "Main");
            static_pointer_cast<Component::RenderComponent>(_RComp)->CastsShadows(true);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setTransparency(false);
            static_pointer_cast<Component::RenderComponent>(_RComp)->setVisibility(true);

            _Entity->addComponent(_RComp);

            EntityList.push_back(_Entity);
        }

        {
            glm::vec3 tPosition = glm::vec3(10, 3.5, 10);
            float tScale = 5.0f;
            glm::quat tRotation = glm::quat(1.0, 0.0, 0.0, 0.0);

            std::shared_ptr<EntityBase> _Entity = std::make_shared<EntityBase>(tPosition, glm::vec3(tScale), tRotation);

            Component::Component_ptr _CComp = std::make_shared<Component::ClothComponent>(tPosition, tScale, tRotation, eCamera[mCurrentCamera]);

            std::static_pointer_cast<Component::ClothComponent>(_CComp)->setShader("Cloth");

            _Entity->addComponent(_CComp);

            EntityList.push_back(_Entity);
        }

        
        glm::vec3 initCubemapPosition = glm::vec3(-35.0, 0, -35.0);
        {
            for (int a = 0; a < 8; a++)
                for (int b = 0; b < 5; b++)
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

                        _Entity->addComponent(_RComp);
                        EntityList.push_back(_Entity);
                    }
        }
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
        limits.MAX_X = 500.0;
        limits.MIN_X = -500.0;
        limits.MAX_Y = 5.0;
        limits.MIN_Y = -50.0;
        limits.MAX_Z = 500.0;
        limits.MIN_Z = -500.0;

       // m_ParticleSystem = (std::shared_ptr<ParticleSystem>)new ParticleSystem();
       // m_ParticleSystem->addNewSystem(limits, MIST, 5000);

        //3 , 7, 30
        sun->Update();
        shadowMap->setShadowPosition(glm::vec3(6 + 1.0 * 30, 8 + 45.0f, -8 + 0.0 * 30));

        shadowMap->setShadowDirection(sun->Direction);

        shadowMap->SetupShadowMatrices();

        shadowMap->BindShadowFrameBuffer();
        this->RenderShadows();
        shadowMap->UnbindShadowFrameBuffer();

        //this->mCubemap[1] = (std::shared_ptr<CubeMap>)new CubeMap(55, glm::vec3(10, 7, 10));

        glClearColor(0.1, 0.1, 0.1, 1.0);
        glViewport(0, 0, 1024, 1024);
        glEnable(GL_DEPTH_TEST);
        eCamera[mCurrentCamera]->Update(window->getHandle()->getHandle());
        eCamera[mCurrentCamera]->UpdateMatrices();
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
        SphericalHarmonics::SphericalHarmonicsFormat sphStruct[8][5][7];
        auto start = std::chrono::steady_clock::now();
        //Shaders["CubeMap"]->Use();
        int index = 0;
        for (int a = 0; a < 8; a++)
            for (int b = 0; b < 5; b++)
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
                                glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0), EntityList[i]->getScale());
                                glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0), EntityList[i]->getPosition());
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
                            }
                            EntityList[i]->Render();
                        }
                        glEnable(GL_CULL_FACE);
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
                        //BSPMap->RenderLevel(mCubemap[a][b][c]->getPosition(), cubeShader->getProgramID(), true);
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
        auto finish =  std::chrono::steady_clock::now();
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
        std::cout << "Cubemap generation took: " << interval << " milliseconds" << std::endl;
        std::cout << interval/245 << " per cubemap" << std::endl;

        ResourceManager::Get().cubemapsLoaded = true;

        glCullFace(GL_BACK);
        

        ResourceManager::Get().loadQueuedTextures();

        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            EntityList[i]->setShader("Main");
            if (EntityList[i]->HasClothComponent())
                EntityList[i]->setShader("Cloth");
        }

        glGenBuffers(1, &AmbientLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, AmbientLightSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SphericalHarmonics::SphericalHarmonicsFormat) * 8 * 5 * 7, (const void *)&sphStruct, GL_DYNAMIC_COPY);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, AmbientLightSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

        ResourceManager::Get().requestShader("shaders/Geometry.vglsl", "shaders/Geometry.fglsl", "Main");
        ResourceManager::Get().requestShader("shaders/Cloth.vglsl", "shaders/Cloth.fglsl", "Cloth");
        ResourceManager::Get().requestShader("shaders/vertex.glsl", "shaders/fragment.glsl", "CubeMap");
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

        BSPMap->LoadBSP((string("maps/") + "godrays_tutorial.bsp").c_str());

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
        //BSPMap->RenderLevel(eCamera[mCurrentCamera]->getPosition(), shader->getProgramID(), true);
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

    void Epsilon::RenderShadows()
    {
        for (unsigned int i = 0; i < EntityList.size(); ++i)
        {
            if (!EntityList[i]->HasRenderComponent())
                continue;

            Component::RenderComponent_ptr RComponent = EntityList[i]->getComponent<Component::RenderComponent_ptr>();

            if (RComponent != nullptr)
                if (!RComponent->CastsShadows())
                    continue;

            Shaders["ShadowMapping"]->Use();
            this->SetUniforms(Shaders["ShadowMapping"], EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation());
            ResourceManager::Get().useModel(EntityList[i]->getModelPath(), Shaders["ShadowMapping"], EntityList[i]->getPosition());
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
        Shaders["ShadowMapping"]->Use();
        this->SetUniforms(Shaders["ShadowMapping"], glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1, 0.1, 0.1), glm::quat(0.0, 0.0, 0.0, 0.0));
        //BSPMap->RenderLevel(eCamera[mCurrentCamera]->getPosition(), Shaders["ShadowMapping"]->getProgramID(), false);

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
        /*
        IO::PrintLine("Position: x = ",
                      Helpers::floatTostring(this->eCamera[0]->getPosition().x),
                      " y = ",
                      Helpers::floatTostring(this->eCamera[0]->getPosition().y),
                      " z = ",
                      Helpers::floatTostring(this->eCamera[0]->getPosition().z));
        /*
    if(DEBUG_MODE >= 1) {
        this->text->RenderText("FPS: " + Helpers::intTostring(acumfps), 0.01, 0.95, 0.5, glm::vec3(1,1,1));
        this->text->RenderText("Frame Time: " + Helpers::floatTostring(frametime*1000) + "ms", 0.01, 0.91, 0.3, glm::vec3(1,1,1));
        this->text->RenderText("Draw calls: " + Helpers::intTostring(glCache::DrawCalls), 0.01, 0.69, 0.3, glm::vec3(1,1,1));
    }

    if(DEBUG_MODE >= 2) {
        this->text->RenderText("OpenGL: " + GL_VER,0.01, 0.89, 0.3, glm::vec3(1,1,1));
        this->text->RenderText(GL_REN,0.01, 0.87, 0.3, glm::vec3(1,1,1));
        this->text->RenderText(GL_VEN,0.01, 0.85, 0.3, glm::vec3(1,1,1));
        this->text->RenderText(std::string("Total GPU Memory: " + Helpers::intTostring(_gpu.getTotalMemory()/1024) + "MB"),0.01, 0.83, 0.3, glm::vec3(1,1,1));
        this->text->RenderText(std::string("Current GPU Memory Available: " + Helpers::intTostring(_gpu.getAvailableMemory()/1024) + "MB"),0.01, 0.81, 0.3, glm::vec3(1,1,1));
        //this->text->RenderText(std::string("Current GPU Memory Available: " + Helpers::intTostring(cur_avail_mem_kb/1024) + "MB"),0.01, 0.77, 0.3, glm::vec3(1,1,1));
        this->text->RenderText(std::string("Resolution: " + Helpers::intTostring(this->WIDTH) + "x" + Helpers::intTostring(this->HEIGHT)),0.01, 0.79, 0.3, glm::vec3(1,1,1));
    }

    if(DEBUG_MODE >= 3) {
        std::string obj = rM->m_PhysicsWorld->getCollisionObjectName(btVector3(eCamera->getPosition().x, eCamera->getPosition().y, eCamera->getPosition().z),
                          btVector3(eCamera->getDirection().x * 1000, eCamera->getDirection().y * 1000, eCamera->getDirection().z * 1000));

        this->text->RenderText(std::string("Pointing at: ") + obj, 0.01, 0.71, 0.3, glm::vec3(1,0,0));
        this->text->RenderText("Epsilon Engine Alpha Build. Version: " + std::string(AutoVersion::FULLVERSION_STRING), 0.01, 0.73, 0.3, glm::vec3(1,1,1));
        this->text->RenderText
        ("Position: x = " + Helpers::floatTostring(this->eCamera->getPosition().x) + " y = " + Helpers::floatTostring(this->eCamera->getPosition().y) + " z = " + Helpers::floatTostring(this->eCamera->getPosition().z), 0.01, 0.77, 0.3, glm::vec3(1,1,1));
        this->text->RenderText
        ("Direction: x = " + Helpers::floatTostring(this->eCamera->getDirection().x) + " y = " + Helpers::floatTostring(this->eCamera->getDirection().y) + " z = " + Helpers::floatTostring(this->eCamera->getDirection().z), 0.01, 0.75, 0.3, glm::vec3(1,1,1));
        //this->text->RenderText("Draw calls: " + Helpers::intTostring(glCache::DrawCalls), 0.01, 0.69, 0.3, glm::vec3(1,1,1));
    }
*/
        //this->text->RenderText("On ground: " + std::string(m_PlayerCapsule->isOnGround() ? "YES" : "NO"),0.01, 0.79, 0.3, glm::vec3(1,1,1));/*
        //this->text->RenderText("Parallax Mapping: " + std::string(parallax ? "ON" : "OFF"),0.01, 0.83, 0.3, glm::vec3(1,1,1));*/

        //this->text->RenderText(".", 0.5, 0.5, 1.0, glm::vec3(1,1,1));

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

        eventtime += 1 * frametime;
        //cout << eventtime << endl;
        sun->Update();

        ResourceManager::Get().timestep = frametime;

        timeBehind += etime - lastTime;
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

        if (glm::abs((timeGUI * 60) - (etime * 60)) > 2.0f && onMenu)
        {
            timeGUI = this->etime;
            //m_GUI->PollEvents(window);
        }

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::ESCAPE] || _Joystick->getJoystickButton(_Joystick->getMapping()->START()))
        {
            if (glm::abs((menuTime * 60) - (etime * 60)) > 60.0f)
            {
                g_Running = false;
                onMenu = !onMenu;
                menuTime = this->etime;
            }
        }
        if (onMenu)
        {
            glfwSetInputMode(window->getHandle()->getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            ComputeCamera(CAMERA_FIXED, eCamera[mCurrentCamera]->getPosition(), eCamera[mCurrentCamera]->getDirection(), eCamera[mCurrentCamera]->getProjectionMatrix(), eCamera[mCurrentCamera]->getViewMatrix());
            this->m_CameraMode = CAMERA_FIXED;
        }
        else
        {
            glfwSetInputMode(window->getHandle()->getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            this->m_CameraMode = NO_CLIP;
        }

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::N])
            normal = !normal;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::F])
            flashLight = !flashLight;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::SPACE]){
            PP->HBAOOn = !PP->HBAOOn;
            SSAO = !SSAO;
        }

        if (glm::abs((menuTime * 60) - (etime * 60)) > 60.0f)
        {
            if (Input::KeyBoard::KEYS[Input::GLFW::Key::C])
            {
                mCurrentCamera = (int)!mCurrentCamera;
                eCamera[mCurrentCamera]->isMoving(true);
                eCamera[!mCurrentCamera]->isMoving(true);
                menuTime = etime;
            }
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

        //m_ParticleSystem->Simulate(this->frametime, this->eCamera->getPosition());
    }

    void Epsilon::MainLoop(void)
    {
        // 0 8 6
        while (g_Running)
        {
            this->ClearBuffers();

            this->Clock();

            this->PollEvents();

            if (!onMenu)
                this->ComputeCamera(m_CameraMode, glm::vec3(48.4247, 8.1507, -12.9128), glm::vec3(-0.785454, 0.0299956, 0.618193));

            this->ProcessAudio();

            while (timeBehind >= 0.016)
            {
                ResourceManager::Get().getPhysicsWorld()->Update(0.016);
                timeBehind -= 0.016;
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
    }

    void Epsilon::SwapBuffers(void)
    {
        //glfwSwapBuffers(this->window->getHandle()->getHandle());
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

        this->eCamera[mCurrentCamera]->UpdateMatrices();
    }

    void Epsilon::ComputeShadow()
    {

        glm::vec3 camPos = eCamera[mCurrentCamera]->getPosition();
        glm::vec3 camDir = eCamera[mCurrentCamera]->getDirection();

        shadowMap->setShadowPosition(glm::vec3(camPos.x + camDir.x * 30, camPos.y + 45.0f, camPos.z + camDir.z * 30));

        shadowMap->setShadowDirection(sun->Direction);

        shadowMap->SetupShadowMatrices();

        shadowMap->BindShadowFrameBuffer();

        //glEnable(GL_DEPTH_CLAMP);
        if (sun->height > 0.0)
            this->RenderShadows();
        //glDisable(GL_DEPTH_CLAMP);
        shadowMap->UnbindShadowFrameBuffer();
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
       // m_ParticleSystem->Render();
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
        PP->ShowFrame(sun->Direction, SSAO, this->eCamera[mCurrentCamera], exposure, this->shadowMap);
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
    }
} // namespace Epsilon