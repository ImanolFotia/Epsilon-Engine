///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
#include <iostream>
#include <Epsilon.h>
#include <ProgramData.h>
#include <cstdlib>
#include <time.h>
#include <Includes.h>
#include <Types.h>
#include "../version.h"
#include <glm/gtc/quaternion.hpp>
#include <cmath>
#include <thread>
#include <EpsilonMemory.h>
#include <sys/CPUID.h>
#include <chrono>

using namespace std::placeholders;
float mpos = 0.0;
float blend = 1.0f;

double Input::Mouse::XPOS = 500;
double Input::Mouse::YPOS = 500;
bool Input::Joystick::JoystickIsPresent = false;
int Input::Joystick::JoystickAxesCount = 0;
int Input::Joystick::JoystickButtonsCount = 0;
const float* Input::Joystick::JoystickAxes = {0};
const unsigned char* Input::Joystick::JoystickButtons = {0};

Epsilon::Epsilon(GLFWwindow*& win)
{
    window = win;
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    cout << endl << endl << "Gathering Video Device information..." << endl;
    GL_VEN = (char*)glGetString(GL_VENDOR);
    GL_REN = (char*)glGetString(GL_RENDERER);
    GL_VER = (char*)glGetString(GL_VERSION);
    std:: cout << "OpenGL VENDOR: " << GL_VEN << std::endl;
    std:: cout << "Video RENDERER: " << GL_REN << std::endl;
    std:: cout << "OpenGL VERSION: " << GL_VER << std::endl;

    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_DISTANCE0);

    double plane[4] = {0.0, 5.0, 0.0, 15.0};
    glClipPlane(GL_CLIP_PLANE0, plane);


    std::cout << "Clip Plane: " << (glIsEnabled(GL_CLIP_PLANE0) ? "Enabled" : "Disabled") << endl;

    CPUID cpu(0);
    cpu.printHardwareInformation();

    {
        glEnable(GL_DEPTH_TEST);

        glDepthFunc(GL_LESS);

        glEnable(GL_MULTISAMPLE);

        glEnable(GL_CULL_FACE);

        glShadeModel(GL_SMOOTH);

        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    normal = true;

    Running = true;

    flashLight = false;

    hdr = true;

    parallax = false;

    exposure = 3.0;

    srand(time(NULL));


}

void Epsilon::RenderSplashScreen(string text)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0,0,WIDTH, HEIGHT);
    Shader tmpShader("shaders/splashv.glsl", "shaders/splashf.glsl");
    eTexture texture("Survtech.png");
    SplashScreen splsh;
    ClearBuffers();
    splsh.Draw(tmpShader.getProgramID(), texture.getTextureID());
    this->text->RenderText(text,0.01, 0.83, 0.3, glm::vec3(1,1,1));
    this->SwapBuffers();
    texture.Destroy();
}

void Epsilon::InitResources(void)
{
    cout << "Initializing Resources..." << endl;

    ProgramData DATA;

    this->WIDTH = DATA.WINDOW_WIDTH;
    this->HEIGHT = DATA.WINDOW_HEIGHT;
    this->SSAO = DATA.SSAO;

    text = std::move((unique_ptr<Text>)(new Text("resources/Prototype.ttf", DATA.WINDOW_WIDTH, DATA.WINDOW_HEIGHT)));

    RenderSplashScreen("Initializing Engine...");

    eCamera = (std::shared_ptr<Camera>)(new Camera(glm::vec3(-10.0f,8.25f,-15.0f), glm::vec3(0.0f,1.0f,0.0f)));

    shadowMap = std::move((unique_ptr<ShadowMap>)(new ShadowMap(DATA.SHADOWMAP_SIZE, DATA.SHADOWMAP_SIZE, -20.0f, 80.0f)));

    rM = ((std::shared_ptr<ResourceManager>)(new ResourceManager()));
    rM->m_PhysicsWorld = (std::shared_ptr<Physics::Physics>) new Physics::Physics();


    RenderSplashScreen("Loading Shaders...");
    this->LoadShaders();

    RenderSplashScreen("Loading Geometry...");
    this->LoadGeometry();

    EntityList.reserve(3);
    EntityList[0] = (std::shared_ptr<EntityTemplate>) (new EntityTemplate(rM, glm::vec3(10.0,9.25,0.0), glm::vec3(2), glm::quat(-1.0, 0.0, -1.0, 0.0)));
    std::shared_ptr<Component::RenderComponent> Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel->Fill("models/cube.eml", rM, "Main");

    std::shared_ptr<Component::PhysicComponent> CompPhys = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();

    std::shared_ptr<Physics::CubePhysicObject> ph = (std::shared_ptr<Physics::CubePhysicObject>) new Physics::CubePhysicObject();
    rM->m_PhysicsWorld->world->addRigidBody(ph->addObject(glm::vec3(10.0,9.25,0.0), 30, rM->getModelBoundingBox("models/cube.eml"), 2.0f).get());
    CompPhys->Fill(100.0f, ph);

    EntityList[0]->addComponent(Compmodel);
    EntityList[0]->addComponent(CompPhys);
    ////////////////////////////////////////

    EntityList[1] = (std::shared_ptr<EntityTemplate>) (new EntityTemplate(rM, glm::vec3(10.0,200.25,0.0), glm::vec3(3.2), glm::quat(-1.0, 0.0, -1.0, 0.0)));
    std::shared_ptr<Component::RenderComponent> Compmodel2 = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel2->Fill("models/esfera.eml", rM, "Main");

    std::shared_ptr<Component::PhysicComponent> CompPhys2 = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();

    std::shared_ptr<Physics::SpherePhysicObject> ph2 = (std::shared_ptr<Physics::SpherePhysicObject>) new Physics::SpherePhysicObject();
    rM->m_PhysicsWorld->world->addRigidBody(ph2->addObject(3.2f, glm::vec3(57.0,150.25,50.0), 30.0f).get());
    CompPhys2->Fill(100.0f, ph2);


    EntityList[1]->addComponent(Compmodel2);
    EntityList[1]->addComponent(CompPhys2);
    /////////////////////////////////////////////////////////
    EntityList[2] = (std::shared_ptr<EntityTemplate>) (new EntityTemplate(rM, glm::vec3(22, 5.5, -25.5), glm::vec3(5), glm::quat(-1.0, 0.0, -1.0, 0.0)));
    std::shared_ptr<Component::RenderComponent> Compmodel3 = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel3->Fill("models/platform.eml", rM, "Main");

    std::shared_ptr<Component::PhysicComponent> CompPhys3 = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();

    std::shared_ptr<Physics::CubePhysicObject> ph3 = (std::shared_ptr<Physics::CubePhysicObject>) new Physics::CubePhysicObject();
    rM->m_PhysicsWorld->world->addRigidBody(ph3->addObject(glm::vec3(22, 5.5, -25.5), 0.0, rM->getModelBoundingBox("models/platform.eml"), 5.0f).get());
    CompPhys3->Fill(0.0f, ph3);

    std::shared_ptr<Component::MovementComponent> MovComp = (std::shared_ptr<Component::MovementComponent>) new Component::MovementComponent();
    MovComp->Fill(ph3, glm::vec3(22, 8.0, -25.5), glm::vec3(22, 21.5, -25.5), 0.08, true);

    EntityList[2]->addComponent(Compmodel3);
    EntityList[2]->addComponent(CompPhys3);
    EntityList[2]->addComponent(MovComp);

    this->LoadSound();

    RenderSplashScreen("Loading Textures...");
    rM->loadQueuedTextures();

    PP = std::move((unique_ptr<PostProcess>)(new PostProcess()));

    m_PlayerCapsule = (std::shared_ptr<Game::Player>) new Game::Player(50.0,10.25,30.0, this->rM);
    //m_PlayerCapsule = (std::shared_ptr<Game::Player>) new Game::Player(170.0,5.25,-202.0, this->rM);

    for(int x=0; x<5; ++x)
        for(int z=0; z<5; ++z)
            xz[x][z] = (rand()%30) - 15;


    std::cout << "All Resources Initialized." << std::endl;
}

void Epsilon::LoadShaders(void)
{
    cout << "Loading Shaders..." <<endl;

    Shaders["grass"] = new Shader( "shaders/grass.vglsl", "shaders/grass.fglsl" );

    Shaders["Main"] = new Shader("shaders/Geometry.vglsl", "shaders/Geometry.fglsl");

    Shaders["Terrain"] = new Shader( "shaders/testshader.vglsl", "shaders/testshader.fglsl" );

    Shaders["Sun"] = new Shader("shaders/Sun.vglsl", "shaders/Sun.fglsl");

    Shaders["SkyBox"] = new Shader("shaders/skybox.vglsl", "shaders/skybox.fglsl");

    Shaders["MD5Geometry"] = new Shader("shaders/MD5Geometryv.glsl", "shaders/MD5Geometryf.glsl");

    Shaders["ShadowMapping"] = new Shader("shaders/ShadowMappingv.glsl", "shaders/ShadowMappingf.glsl");

    Shaders["MD5ShadowMapping"] = new Shader("shaders/MD5GeometryShadowv.glsl", "shaders/ShadowMappingf.glsl");

    Shaders["ForwardShader"] = new Shader("shaders/TransformVertexShader.vglsl", "shaders/TextureFragmentShader.fglsl");

    rM->requestShader("shaders/Geometry.vglsl", "shaders/Geometry.fglsl", "Main");
}

void Epsilon::LoadGeometry(void)
{
    cout << "Loading World Geometry..." <<endl;
    vector<glm::vec3> grasspos2;

   // terrain = (std::shared_ptr<Terrain>)new Terrain("materials/mountains.jpg", "mountains3.jpg", "mud-normal.jpg", "awp_india_texture_4s.jpg", 3, 1, glm::vec3(-850.0, -50.0, -600),rM);
/*


    for(int i = 0 ; i < terrain->vertices.size() ; i+= 3)
    {
        int chance = rand() % 2 + 1;
        if(chance == 1)
            grassPos.push_back(terrain->vertices[i].Position + glm::vec3((rand()%6)-3, 3.5, (rand()%6)-3));
        else if(chance == 2)
            grasspos2.push_back(terrain->vertices[i].Position + glm::vec3((rand()%6)-3, 3.5, (rand()%6)-3));
    }
*/
    std::cout << "Resource manager in epsilon address: " << rM.get() << std::endl;

    rM->requestCubeMap(2, glm::vec3(0,0,0));
    rM->requestModel("models/esfera.eml", rM, glm::vec3(78.0,5.25,-57), glm::vec3(1), glm::quat(0.0, 0.0, 0.0, 0.0));
    rM->requestModel("models/dragon.eml", rM, glm::vec3(79,0.0,-20.0), glm::vec3(1), glm::quat(-1.0, 0.0, -1.0, 0.0));
    rM->requestModel("models/platform.eml", rM, glm::vec3(50,0.0,0), glm::vec3(4), glm::quat(-1.0, 0.0, -1.0, 0.0));
    rM->requestModel("models/bunny.eml", rM, glm::vec3(79.0,0.0,20), glm::vec3(4), glm::quat(1.0, 0.0, -0.5, 0.0));
    rM->requestModel("models/Desk.eml", rM, glm::vec3(-2.0,8.0,10.0), glm::vec3(0.9), glm::quat(1, 0.0, -1.0, 0.0));
    rM->requestModel("models/buddha.eml", rM, glm::vec3(79,0.0,0), glm::vec3(1), glm::quat(-1, 0.0, 1, 0.0));
/*

            std::vector<std::string> paths;
        std::string path;


        paths.push_back(path + "materials/skyboxes/cubemap2/right.tga");
        paths.push_back(path + "materials/skyboxes/cubemap2/left.tga");
        paths.push_back(path + "materials/skyboxes/cubemap2/top.tga");
        paths.push_back(path + "materials/skyboxes/cubemap2/bottom.tga");
        paths.push_back(path + "materials/skyboxes/cubemap2/back.tga");
        paths.push_back(path + "materials/skyboxes/cubemap2/front.tga");

    CubeMap cube(paths, 1, glm::vec3(0,0,0));
    sphericalharmonics = (std::shared_ptr<SphericalHarmonics>) new SphericalHarmonics();
    sphericalharmonics->CalculateCohefficients(cube, 3);
    std::vector<glm::vec3> sph = sphericalharmonics->getCohefficients();

    for(int i = 0; i < sph.size(); ++i)
        cout << sph[i].x << ", " << sph[i].y << ", " << sph[i].z  << endl;
*/
    skybox = std::move((unique_ptr<Skybox>)(new Skybox("plain")));
    grass.push_back(Grass("grass04.png", grassPos));
    grass.push_back(Grass("billboardgrass0002.png", grasspos2));

    //waterPlane = std::move((unique_ptr<Water>)(new Water(glm::vec3(50000,-1,0), 100000.0f))); ///-11.8

    sun = std::move((shared_ptr<Sun>)(new Sun()));

    BSPMap = std::move((unique_ptr<CQuake3BSP>)(new CQuake3BSP(this->rM)));

    BSPMap->LoadBSP((string("maps/") + "democorridor.bsp").c_str());

    m_AnimModel = std::move((unique_ptr<MD5Model>)(new MD5Model()));

    m_AnimModel->LoadModel("models/hellknight/hellknight.md5mesh");

    m_AnimModel->LoadAnim("models/hellknight/walk7.md5anim");
    m_AnimModel->LoadAnim("models/hellknight/idle2.md5anim");

}

void Epsilon::LoadSound(void)
{
    cout << "Loading Sound..." << endl;

    m_AudioSystem = (std::unique_ptr<Audio::Audio>) new Audio::Audio();

    m_AudioElement = (std::unique_ptr<Audio::AudioElement>) new Audio::AudioElement("sound/mgs1.wav", STATIC_SOUND, glm::vec3(88, 10, 15), glm::vec3(0,0,0));

    m_AudioListener = (std::unique_ptr<Audio::AudioListener>) new Audio::AudioListener();

}

bool visible = true;
float acum = -1.0;
void Epsilon::Render3D(Shader* shader)
{
    glm::mat4 Model = glm::mat4(1);
    glm::mat4 ScaleMatrix;
    glm::mat4 TranslationMatrix;
    glm::mat4 RotationMatrix;
/*
    shader->Use();
    this->SetUniforms(shader,glm::vec3(0, 0, 0), glm::vec3(1),  glm::quat(0, 0 ,0, 0));
    terrain->RenderTerrain(shader);
    glCullFace(GL_BACK);
*/
    for(int i =0; i < 3; ++i)
    {
        shader->Use();
        //this->SetUniforms(shader, EntityList[i]->getPosition(), EntityList[i]->getScale(), EntityList[i]->getRotation());
        ScaleMatrix = glm::scale(glm::mat4(1), EntityList[i]->getScale());
        TranslationMatrix = glm::translate(glm::mat4(1), EntityList[i]->getPosition());
        RotationMatrix = glm::toMat4(EntityList[i]->getRotation());
        Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
        this->rM->setModelUniforms(EntityList[i]->modelPath, shader, EntityList[i]->getPosition() , EntityList[i]->getScale(), EntityList[i]->getRotation(), eCamera);
        BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);
        //visible = BSPMap->Frustum.BoxInFrustum(EntityList[0]->getBoundingBox());

        //if(visible)
        //{
        EntityList[i]->Update();
        //}
        //else
        //{
        //cout << "no visible" << endl;
        //}
    }


    shader->Use();
    this->rM->setModelUniforms(std::string("models/dragon.eml"), shader, rM->getModelPosition("models/dragon.eml") , rM->getModelScale("models/dragon.eml"), rM->getModelRotation("models/dragon.eml"), eCamera);
    Model = glm::mat4(1);
    ScaleMatrix = glm::scale(glm::mat4(), rM->getModelScale("models/dragon.eml"));
    TranslationMatrix = glm::translate(glm::mat4(), rM->getModelPosition("models/dragon.eml"));
    RotationMatrix = glm::toMat4(glm::normalize(rM->getModelRotation("models/dragon.eml")));
    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);

    visible = BSPMap->Frustum.BoxInFrustum(rM->getModelBoundingBox("models/dragon.eml"));

    if(visible)
    {
        rM->useModel("models/dragon.eml", shader);
    }
    else
    {

    }
/*
    shader->Use();
    this->SetUniforms(shader,rM->getModelPosition("models/platform.eml"), rM->getModelScale("models/platform.eml"),  rM->getModelRotation("models/platform.eml"));
    Model = glm::mat4();
    ScaleMatrix = glm::scale(glm::mat4(), rM->getModelScale("models/platform.eml"));
    TranslationMatrix = glm::translate(glm::mat4(),rM->getModelPosition("models/platform.eml"));
    RotationMatrix = glm::toMat4(glm::normalize(rM->getModelRotation("models/platform.eml")));
    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);

    visible = BSPMap->Frustum.BoxInFrustum(rM->getModelBoundingBox("models/platform.eml"));
    if(visible)
    {
        rM->useModel("models/platform.eml", shader);
    }
    else
    {

    }*/

    shader->Use();
    this->SetUniforms(shader,rM->getModelPosition("models/bunny.eml") /*+ glm::vec3(0, 0.0, -i * 5)*/, rM->getModelScale("models/bunny.eml"),  rM->getModelRotation("models/bunny.eml"));
    Model = glm::mat4();
    ScaleMatrix = glm::scale(glm::mat4(), rM->getModelScale("models/bunny.eml"));
    TranslationMatrix = glm::translate(glm::mat4(),rM->getModelPosition("models/bunny.eml") /*+ glm::vec3(0, 0.0, -i * 5)*/);
    RotationMatrix = glm::toMat4(glm::normalize(rM->getModelRotation("models/bunny.eml")));
    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);

    visible = BSPMap->Frustum.BoxInFrustum(rM->getModelBoundingBox("models/bunny.eml"));
    if(visible)
    {
        rM->useModel("models/bunny.eml", shader);
    }
    else
    {

    }



    shader->Use();
    this->SetUniforms(shader,rM->getModelPosition("models/buddha.eml") /*+ glm::vec3(0, 0.0, -i * 5)*/, rM->getModelScale("models/buddha.eml"),  rM->getModelRotation("models/buddha.eml"));
    Model = glm::mat4();
    ScaleMatrix = glm::scale(glm::mat4(), rM->getModelScale("models/buddha.eml"));
    TranslationMatrix = glm::translate(glm::mat4(),rM->getModelPosition("models/buddha.eml") /*+ glm::vec3(0, 0.0, -i * 5)*/);
    RotationMatrix = glm::toMat4(glm::normalize(rM->getModelRotation("models/buddha.eml") * glm::quat(0.0, 0.0, 1.0, 0.0)));
    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);

    visible = BSPMap->Frustum.BoxInFrustum(rM->getModelBoundingBox("models/buddha.eml"));
    if(visible)
    {
        rM->useModel("models/buddha.eml", shader);
    }
    else
    {

    }

    glCullFace(GL_FRONT);
    glm::mat4 BSPmodel = glm::mat4();
    //glm::mat4 tmodel = glm::translate(glm::mat4(), glm::vec3(-30.0, 5.0, -120.0));
    glm::mat4 sModel = glm::scale(glm::mat4(), glm::vec3(0.1, 0.1, 0.1));
    BSPmodel = sModel;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), BSPmodel);
    shader->Use();
    this->SetUniforms(shader,  glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1, 0.1, 0.1), glm::quat(0,0,0,0));
    BSPMap->RenderLevel(eCamera->getPosition(), shader->getProgramID(), true);

    Shaders["MD5Geometry"]->Use();
    glUniformMatrix4fv(glGetUniformLocation(Shaders["MD5Geometry"]->getProgramID(), "mSkinned"), 150, GL_FALSE, &m_AnimModel->m_AnimatedBones[0][0][0]);
    this->SetUniforms(Shaders["MD5Geometry"], glm::vec3(mpos,3.0,60), glm::vec3(0.1, 0.1, 0.1), glm::quat(-1.0, 1.0, 0.0, 0.0));
    m_AnimModel->Render(Shaders["MD5Geometry"]->getProgramID());
    glCullFace(GL_BACK);


}

void Epsilon::Render3D()
{
    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"], EntityList[0]->getPosition(), EntityList[0]->getScale(), EntityList[0]->getRotation());
    rM->useModel(EntityList[0]->modelPath, Shaders["ShadowMapping"]);

    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"], EntityList[1]->getPosition(), EntityList[1]->getScale(), EntityList[1]->getRotation());
    rM->useModel(EntityList[1]->modelPath, Shaders["ShadowMapping"]);


    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"], rM->getModelPosition("models/dragon.eml"), rM->getModelScale("models/dragon.eml"), rM->getModelRotation("models/dragon.eml"));
    rM->useModel("models/dragon.eml", Shaders["ShadowMapping"]);

    //for(int i =0; i < 4; ++i)
    //{
    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"], rM->getModelPosition("models/platform.eml") + glm::vec3(0, 0.0, 0.0), rM->getModelScale("models/platform.eml"), rM->getModelRotation("models/platform.eml"));
    rM->useModel("models/platform.eml", Shaders["ShadowMapping"]);
    //}


    glDisable(GL_CULL_FACE);
    glm::mat4 BSPmodel = glm::mat4();
    //glm::mat4 tmodel = glm::translate(glm::mat4(), glm::vec3(-30.0, 5.0, -120.0));
    glm::mat4 sModel = glm::scale(glm::mat4(), glm::vec3(0.1, 0.1, 0.1));
    BSPmodel = sModel;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), BSPmodel);
    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"],  glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1, 0.1, 0.1), glm::quat(0.0, 0.0, 0.0, 0.0));
    BSPMap->RenderLevel(eCamera->getPosition(), Shaders["ShadowMapping"]->getProgramID(), false);

    Shaders["MD5ShadowMapping"]->Use();
    glUniformMatrix4fv(glGetUniformLocation(Shaders["MD5ShadowMapping"]->getProgramID(), "mSkinned"), 150, GL_FALSE, &m_AnimModel->m_AnimatedBones[0][0][0]);
    this->SetUniforms(Shaders["MD5ShadowMapping"], glm::vec3(mpos,3.0,60), glm::vec3(0.1, 0.1, 0.1), glm::quat(-1.0, 1.0, 0.0, 0.0f));
    m_AnimModel->Render(Shaders["MD5ShadowMapping"]->getProgramID());
    Shaders["MD5ShadowMapping"]->Free();
    glEnable(GL_CULL_FACE);
}


void Epsilon::SetUniforms(Shader*& shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation)
{
    glm::mat4 Model = glm::mat4();
    glm::mat4 ScaleMatrix = glm::scale(glm::mat4(), scale);
    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), position);
    glm::mat4 RotationMatrix;
    RotationMatrix = glm::mat4(1) * glm::toMat4(glm::normalize(rotation));

    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;

    glm::mat4 MVP = eCamera->getProjectionMatrix() * eCamera->getViewMatrix() * Model;
    glUniformMatrix4fv(shader->MVP_Location, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(shader->WorldTransform_Location, 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(shader->View_Location, 1, GL_FALSE, &eCamera->getViewMatrix()[0][0]);
    glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &eCamera->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(shader->LightSpaceMatrix_Location, 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);

    glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"),  glfwGetTime());

}

void Epsilon::Render2D(void)
{
    m_TextAcum += frametime;
    static float acumfps = 0.0;
    if(m_TextAcum > 0.5) {
        m_TextAcum = 0.0;
        acumfps = fps;

    }
    std::string obj = rM->m_PhysicsWorld->getCollisionObjectName(btVector3(eCamera->getPosition().x, eCamera->getPosition().y, eCamera->getPosition().z),
                      btVector3(eCamera->getDirection().x * 1000, eCamera->getDirection().y * 1000, eCamera->getDirection().z * 1000));

    //this->text->RenderText(std::string("Pointing at: ") + obj , 0.01, 0.75, 0.3, glm::vec3(1,0,0));

    //this->text->RenderText("Epsilon Engine Alpha Developer Build. Build: " + Helpers::intTostring(AutoVersion::BUILD), 0.01, 0.97, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("Speed: " + floatTostring(eCamera->MovementSpeed), 0.01, 0.95, 0.3, glm::vec3(1,1,1));
    this->text->RenderText
    ("Position: x = " + Helpers::floatTostring(this->eCamera->getPosition().x) + " y = " + Helpers::floatTostring(this->eCamera->getPosition().y) + " z = " + Helpers::floatTostring(this->eCamera->getPosition().z), 0.01, 0.77, 0.3, glm::vec3(1,1,1));

    GLint total_mem_kb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
                  &total_mem_kb);

    GLint cur_avail_mem_kb = 0;
    glGetIntegerv(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
                  &cur_avail_mem_kb);

    this->text->RenderText("FPS: " + Helpers::intTostring(acumfps), 0.01, 0.95, 0.5, glm::vec3(1,1,1));
    this->text->RenderText("Frame Time: " + Helpers::floatTostring(frametime*1000) + "ms", 0.01, 0.91, 0.3, glm::vec3(1,1,1));
    this->text->RenderText("OpenGL: " + GL_VER,0.01, 0.89, 0.3, glm::vec3(1,1,1));
    this->text->RenderText(GL_REN,0.01, 0.87, 0.3, glm::vec3(1,1,1));
    this->text->RenderText(GL_VEN,0.01, 0.85, 0.3, glm::vec3(1,1,1));
    this->text->RenderText(std::string("Total GPU Memory: " + Helpers::intTostring(total_mem_kb/1024) + "MB"),0.01, 0.83, 0.3, glm::vec3(1,1,1));
    this->text->RenderText(std::string("Current GPU Memory Available: " + Helpers::intTostring(cur_avail_mem_kb/1024) + "MB"),0.01, 0.81, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("On ground: " + std::string(m_PlayerCapsule->isOnGround() ? "YES" : "NO"),0.01, 0.79, 0.3, glm::vec3(1,1,1));/*
    //this->text->RenderText("Parallax Mapping: " + std::string(parallax ? "ON" : "OFF"),0.01, 0.83, 0.3, glm::vec3(1,1,1));*/


}

void Epsilon::ProcessAudio()
{

    m_AudioListener->setListenerPosition(this->eCamera->getPosition());
    m_AudioListener->setListenerDirection(this->eCamera->getDirection());

    m_AudioListener->UpdateListener();

    m_AudioElement->Play();

}

void Epsilon::Clock()
{
    etime = glfwGetTime();
    frametime = etime - lastTime;
    double t;

    fps = 1 / frametime;
    if(etime > t)
    {
        fpss << fps;
        t = etime + (double)1.000;
    }

    eventtime += 1 * frametime;
    //cout << eventtime << endl;
    sun->Update();

    rM->timestep = frametime;
}

void Epsilon::RenderSkybox(bool state)
{
    Shaders["SkyBox"]->Use();
    glUniform3f(glGetUniformLocation(Shaders["SkyBox"]->getProgramID(), "LightDirection"), sun->Direction.x, sun->Direction.y, sun->Direction.z);
    skybox->Render(this->eCamera, Shaders["SkyBox"], this->exposure, state);

    Shaders["Sun"]->Use();
    sun->SetUniforms(eCamera, Shaders["Sun"]);
    sun->Render(Shaders["Sun"]);
}

void Epsilon::PollEvents(void)
{

    if(glfwGetWindowAttrib(window, GLFW_FOCUSED))
        glfwPollEvents();
    else
        glfwWaitEvents();

    Input::Joystick::PollJoystick();

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::ESCAPE])
    {

        endgame();

    }

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::N])
        normal = !normal;

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::F])
        flashLight = !flashLight;
    /*
        if(Input::KeyBoard::KEYS[Input::GLFW::Key::SPACE])
            SSAO = !SSAO;
    */
    if(Input::KeyBoard::KEYS[Input::GLFW::Key::P])
        parallax = !parallax;

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::ADD])
        exposure += 0.5 * frametime;

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::SUBTRACT])
        exposure -= 0.5 * frametime;

    blend = glm::clamp(blend, 0.0f, 1.0f);

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::ARROW_UP])
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
    //m_GUI->PollEvents(window);
}

void Epsilon::MainLoop(void)
{
    while(Running)
    {
        this->ClearBuffers();

        this->Clock();

        this->PollEvents();

        ComputeCamera();

        //this->ProcessAudio();

        rM->m_PhysicsWorld->Update(frametime);

        this->ComputeShadow();

        this->ProcessFrame();

        this->RenderFrame();

        //this->Render2D();

        this->SwapBuffers();
    }
}

void Epsilon::SwapBuffers(void)
{
    glfwSwapBuffers(this->window);

    lastTime = etime;
}

void Epsilon::ClearBuffers(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Epsilon::ComputeCamera()
{
    this->eCamera->Update(this->window);
    //this->eCamera->setDirection(glm::vec3(0,0,-1));
    //this->eCamera->setPosition(glm::vec3(20.0,12.4, 0));

    if(0)
    {
        m_PlayerCapsule->preStep();
        m_PlayerCapsule->Movement(this->eCamera, frametime);
        eCamera->setPosition(m_PlayerCapsule->getPosition());
    }

    this->eCamera->UpdateMatrices();
}

void Epsilon::ComputeWater(void)
{
    /**
        Render Reflection to Texture
    */
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_DISTANCE0);
    /*
        this->waterPlane->GenerateReflection(this->eCamera);
        float distance = 2 * (this->eCamera->getPosition().y - this->waterPlane->position.y);
        this->eCamera->setPosition(glm::vec3(this->eCamera->getPosition().x, this->eCamera->getPosition().y - distance, this->eCamera->getPosition().z));
        this->eCamera->setDirection(glm::vec3(this->eCamera->getDirection().x, this->eCamera->getDirection().y * -1, this->eCamera->getDirection().z));
        this->eCamera->UpdateMatrices();
        this->waterPlane->setActiveClip(1);
        this->Render3D(Shaders["Main"]);
        this->RenderSkybox(false);
        this->eCamera->setPosition(glm::vec3(this->eCamera->getPosition().x, this->eCamera->getPosition().y + distance, this->eCamera->getPosition().z));
        this->eCamera->setDirection(glm::vec3(this->eCamera->getDirection().x, this->eCamera->getDirection().y * -1, this->eCamera->getDirection().z));
        this->waterPlane->FinishWatercomputation();
    */
    /**
        Render Refraction to Texture
    */
    /*
        this->waterPlane->GenerateRefraction(this->eCamera);
        this->eCamera->UpdateMatrices();
        this->waterPlane->setActiveClip(-1);
        this->Render3D(Shaders["Main"]);
        this->RenderSkybox(false);
        this->waterPlane->FinishWatercomputation();*/
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_DISTANCE0);

}

void Epsilon::ComputeShadow()
{

    glm::vec3 camPos = eCamera->getPosition();
    glm::vec3 camDir = eCamera->getDirection();


    shadowMap->setShadowPosition(glm::vec3(camPos.x + camDir.x * 30, camPos.y + 45.0f, camPos.z + camDir.z * 30));

    shadowMap->setShadowDirection(sun->Direction);

    shadowMap->SetupShadowMatrices();

    shadowMap->BindShadowFrameBuffer();

    glEnable(GL_DEPTH_CLAMP);
    this->Render3D();
    glDisable(GL_DEPTH_CLAMP);
    shadowMap->UnbindShadowFrameBuffer();
}

void Epsilon::ProcessFrame(void)
{
    PP->beginOffScreenrendering();

    glEnable(GL_DEPTH_CLAMP);
    this->RenderSkybox(true);
/*
    Shaders["grass"]->Use();
    this->SetUniforms(Shaders["grass"], glm::vec3(-512, 0, 512), glm::vec3(3,3,3), glm::quat(-1, 0, -1, 0) );
    grass.at(0).Render(Shaders["grass"]);

    Shaders["grass"]->Use();
    this->SetUniforms(Shaders["grass"], glm::vec3(-512, 0, 512), glm::vec3(1,1,1), glm::quat(-1, 0, -1, 0) );
    grass.at(1).Render(Shaders["grass"]);
*/
    this->Render3D(Shaders["Main"]);
    //this->waterPlane->RenderWater(eCamera, sun->Direction);

    glDisable(GL_DEPTH_CLAMP);
    PP->endOffScreenRendering();

    if(this->SSAO)
        PP->applySSAO(this->eCamera);
}

void Epsilon::RenderFrame(void)
{
    glViewport(0,0, this->WIDTH, this->HEIGHT);

    PP->ShowFrame(sun->Direction, SSAO, this->eCamera, exposure, this->shadowMap);
    glEnable(GL_DEPTH_CLAMP);
    this->RenderSkybox(false);
    glDisable(GL_DEPTH_CLAMP);
    PP->ShowPostProcessImage(exposure, shadowMap->getShadowTextureID(), this->sun->Direction, this->eCamera);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
