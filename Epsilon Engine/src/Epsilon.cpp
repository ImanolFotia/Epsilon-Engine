///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

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
float mpos = -40.0;
float blend = 1.0f;


double Input::Mouse::XPOS = 500;
double Input::Mouse::YPOS = 500;

Epsilon::Epsilon(GLFWwindow*& win)
{
    window = win;
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    cout << endl << endl << "Gathering Video Device information..." << endl;

    std:: cout << "OpenGL VENDOR: " << glGetString(GL_VENDOR) << std::endl;
    std:: cout << "Video RENDERER: " << glGetString(GL_RENDERER) << std::endl;
    std:: cout << "OpenGL VERSION: " << glGetString(GL_VERSION) << std::endl;

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

void Epsilon::InitResources(void)
{
    cout << "Initializing Resources..." << endl;

    ProgramData DATA;

    this->WIDTH = DATA.WINDOW_WIDTH;
    this->HEIGHT = DATA.WINDOW_HEIGHT;
    this->SSAO = DATA.SSAO;

    eCamera = std::move((unique_ptr<Camera>)(new Camera(glm::vec3(-10.0f,29.25f,-15.0f), glm::vec3(1.0f,1.0f,1.0f))));

    text = std::move((unique_ptr<Text>)(new Text("resources/arial.ttf", DATA.WINDOW_WIDTH, DATA.WINDOW_HEIGHT)));


    lightPositions.push_back(glm::vec3(-29, 4.5, -11));
    lightPositions.push_back(glm::vec3(-29, 4.5, 11));
    lightPositions.push_back(glm::vec3(29, 4.5, 11));
    lightPositions.push_back(glm::vec3(29, 4.5, -11));

    shadowMap = std::move((unique_ptr<ShadowMap>)(new ShadowMap(DATA.SHADOWMAP_SIZE, DATA.SHADOWMAP_SIZE, -20.0f, 80.0f)));

    rM = ((std::shared_ptr<ResourceManager>)(new ResourceManager()));
    rM->m_PhysicsWorld = (std::shared_ptr<Physics::Physics>) new Physics::Physics();


    this->LoadShaders();

    this->LoadGeometry();


    EntityTest[0] = (std::shared_ptr<EntityTemplate>) (new EntityTemplate(rM, glm::vec3(-28.0,17.25,-20), glm::vec3(1), glm::quat(-1.0, 0.0, -1.0, 0.0)));
    std::shared_ptr<Component::RenderComponent> Compmodel = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel->Fill("models/cube.eml", rM, "Main");

    std::shared_ptr<Component::PhysicComponent> CompPhys = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();

    std::shared_ptr<Physics::CubePhysicObject> ph = (std::shared_ptr<Physics::CubePhysicObject>) new Physics::CubePhysicObject();
    rM->m_PhysicsWorld->world->addRigidBody(ph->addObject(glm::vec3(-28.0,70.25,-5), 30, rM->getModelBoundingBox("models/cube.eml"), 1.0f).get());
    CompPhys->Fill(100.0f, ph);

    EntityTest[0]->addComponent(Compmodel);
    EntityTest[0]->addComponent(CompPhys);
    ////////////////////////////////////////

    EntityTest[1] = (std::shared_ptr<EntityTemplate>) (new EntityTemplate(rM, glm::vec3(-27.0,17.25,-24), glm::vec3(3.2), glm::quat(-1.0, 0.0, -1.0, 0.0)));
    std::shared_ptr<Component::RenderComponent> Compmodel2 = (std::shared_ptr<Component::RenderComponent>) new Component::RenderComponent();
    Compmodel2->Fill("models/cube.eml", rM, "Main");

    std::shared_ptr<Component::PhysicComponent> CompPhys2 = (std::shared_ptr<Component::PhysicComponent>) new Component::PhysicComponent();

    std::shared_ptr<Physics::CubePhysicObject> ph2 = (std::shared_ptr<Physics::CubePhysicObject>) new Physics::CubePhysicObject();
    rM->m_PhysicsWorld->world->addRigidBody(ph2->addObject(glm::vec3(-27.0,70.25,-10), 30, rM->getModelBoundingBox("models/cube.eml"), 3.2f).get());
    CompPhys2->Fill(100.0f, ph2);

    EntityTest[1]->addComponent(Compmodel2);
    EntityTest[1]->addComponent(CompPhys2);

    this->LoadSound();

    rM->loadQueuedTextures();

    PP = std::move((unique_ptr<PostProcess>)(new PostProcess()));

    //m_PlayerCapsule = (std::shared_ptr<Game::Player>) new Game::Player(27.0,70.25,-10, this->rM);

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
/*
    for(int i = 0 ; i < 250 ; i++)
    {
        grassPos.push_back(glm::vec3(  50 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(80))) , 4.5, -23 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(70))) ));
        //cout << grassPos.at(i).x << grassPos.at(i).y << grassPos.at(i).z << endl;
    }
*/
    vector<glm::vec3> grasspos2;
    /*
    for(int i = 0 ; i < 250 ; i++)
    {
        grasspos2.push_back(glm::vec3(  50 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(80))), 4.5, -23 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(70))) ));

    }*/




    terrain = (std::shared_ptr<Terrain>)new Terrain("materials/testmontana2.png", "awp_india_texture_4.jpg", "mud-normal.jpg", "awp_india_texture_4s.jpg", 1, 1);


    for(int i = 0 ; i < terrain->vertices.size() ; i+= 5)
    {
         int chance = rand() % 2 + 1;
         if(chance == 1)
            grassPos.push_back(terrain->vertices[i].Position + glm::vec3(-230, -2, -256));
         else if(chance == 2)
            grasspos2.push_back(terrain->vertices[i].Position + glm::vec3(-230, -2, -256));
    }
/*
    for(int i = 5 ; i < terrain->vertices.size() ; i+= 10)
    {
         grasspos2.push_back(terrain->vertices[i].Position + glm::vec3(-230, -2, -256));
    }*/

    std::cout << "Resource manager in epsilon address: " << rM.get() << std::endl;

    rM->requestCubeMap(2, glm::vec3(0,0,0));
    rM->requestModel("models/esfera.eml", rM, glm::vec3(78.0,5.25,-57), glm::vec3(5), glm::quat(0.0, 0.0, 0.0, 0.0));
    rM->requestModel("models/barrel.eml", rM, glm::vec3(98.9,5.25,-6.0), glm::vec3(4), glm::quat(-1.0, 0.0, -1.0, 0.0));
    rM->requestModel("models/Rock_6.eml", rM, glm::vec3(102, 2.0, 19), glm::vec3(5), glm::quat(-1.0, 0.0, -1.0, 0.0));
    rM->requestModel("models/Tree.eml", rM, glm::vec3(61.0,0.75,-12.0), glm::vec3(10), glm::quat(-1.0, 0.0, -1.0, 0.0));
    skybox = std::move((unique_ptr<Skybox>)(new Skybox("plain")));
    grass.push_back(Grass("grass04.png", grassPos));
    grass.push_back(Grass("billboardredflowers.png", grasspos2));

    waterPlane = std::move((unique_ptr<Water>)(new Water(glm::vec3(-29,-0.2,-83), 1000.0f))); ///-11.8

    sun = std::move((shared_ptr<Sun>)(new Sun()));

    BSPMap = std::move((unique_ptr<CQuake3BSP>)(new CQuake3BSP(this->rM)));

    BSPMap->LoadBSP((string("maps/") + "alley.bsp").c_str());

    m_AnimModel = std::move((unique_ptr<MD5Model>)(new MD5Model()));

    m_AnimModel->LoadModel("models/hellknight/hellknight.md5mesh");

    m_AnimModel->LoadAnim("models/hellknight/walk7.md5anim");
    m_AnimModel->LoadAnim("models/hellknight/idle2.md5anim");

    /*
            m_AnimModel->LoadModel("models/dummy.md5mesh");
            m_AnimModel->LoadAnim("models/anim.md5anim");
            m_AnimModel->LoadAnim("models/anim.md5anim");
    */
}

void Epsilon::LoadSound(void)
{
    cout << "Loading Sound..." << endl;
}

bool visible = true;
void Epsilon::Render3D(Shader* shader)
{
    glm::mat4 Model = glm::mat4(1);
    glm::mat4 ScaleMatrix;
    glm::mat4 TranslationMatrix;
    glm::mat4 RotationMatrix;

    shader->Use();
    this->SetUniforms(shader,glm::vec3(-230, -5, -256), glm::vec3(1),  glm::quat(0, 0 ,0, 0));
    terrain->RenderTerrain(shader);

    for(int i =0; i < 2; ++i)
    {
        shader->Use();
        this->SetUniforms(shader, EntityTest[i]->getPosition(), EntityTest[i]->getScale(), EntityTest[i]->getRotation());
        ScaleMatrix = glm::scale(glm::mat4(1), EntityTest[i]->getScale());
        TranslationMatrix = glm::translate(glm::mat4(1), EntityTest[i]->getPosition());
        RotationMatrix = glm::toMat4(EntityTest[i]->getRotation());
        Model = TranslationMatrix * ScaleMatrix * RotationMatrix;

        BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);
        //visible = BSPMap->Frustum.BoxInFrustum(EntityTest[0]->getBoundingBox());

        //if(visible)
        //{
        EntityTest[i]->Update();
        //}
        //else
        //{
        //cout << "no visible" << endl;
        //}
    }
    shader->Use();
    this->SetUniforms(shader, rM->getModelPosition("models/barrel.eml"), rM->getModelScale("models/barrel.eml"), rM->getModelRotation("models/barrel.eml"));
    Model = glm::mat4(1);
    ScaleMatrix = glm::scale(glm::mat4(), rM->getModelScale("models/barrel.eml"));
    TranslationMatrix = glm::translate(glm::mat4(), rM->getModelPosition("models/barrel.eml"));
    RotationMatrix = glm::toMat4(glm::normalize(rM->getModelRotation("models/barrel.eml")));
    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);

    visible = BSPMap->Frustum.BoxInFrustum(rM->getModelBoundingBox("models/barrel.eml"));

    if(visible)
    {
        rM->useModel("models/barrel.eml", shader);
    }
    else
    {

    }

    shader->Use();
    this->SetUniforms(shader,rM->getModelPosition("models/Rock_6.eml"), rM->getModelScale("models/Rock_6.eml"),  rM->getModelRotation("models/Rock_6.eml"));
    Model = glm::mat4();
    ScaleMatrix = glm::scale(glm::mat4(), rM->getModelScale("models/Rock_6.eml"));
    TranslationMatrix = glm::translate(glm::mat4(),rM->getModelPosition("models/Rock_6.eml"));
    RotationMatrix = glm::toMat4(glm::normalize(rM->getModelRotation("models/Rock_6.eml")));
    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);

    visible = BSPMap->Frustum.BoxInFrustum(rM->getModelBoundingBox("models/Rock_6.eml"));
    if(visible)
    {
        rM->useModel("models/Rock_6.eml", shader);
    }
    else
    {

    }

    shader->Use();
    this->SetUniforms(shader,rM->getModelPosition("models/Tree.eml"), rM->getModelScale("models/Tree.eml"),  rM->getModelRotation("models/Tree.eml"));
    Model = glm::mat4();
    ScaleMatrix = glm::scale(glm::mat4(), rM->getModelScale("models/Tree.eml"));
    TranslationMatrix = glm::translate(glm::mat4(),rM->getModelPosition("models/Tree.eml"));
    RotationMatrix = glm::toMat4(glm::normalize(rM->getModelRotation("models/Tree.eml")));
    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);

    visible = BSPMap->Frustum.BoxInFrustum(rM->getModelBoundingBox("models/Tree.eml"));
    if(visible)
    {
        rM->useModel("models/Tree.eml", shader);
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
    this->SetUniforms(Shaders["MD5Geometry"], glm::vec3(mpos,0.0,-21), glm::vec3(0.1, 0.1, 0.1), glm::quat(-1.0, 1.0, 0.0, 0.0));
    m_AnimModel->Render(Shaders["MD5Geometry"]->getProgramID());
    glCullFace(GL_BACK);


}

void Epsilon::Render3D()
{

    for(int i =0; i <2; ++i)
    {
        Shaders["ShadowMapping"]->Use();
        this->SetUniforms(Shaders["ShadowMapping"], EntityTest[i]->getPosition(), EntityTest[i]->getScale(), EntityTest[i]->getRotation());
        rM->useModel("models/cube.eml", Shaders["ShadowMapping"]);
    }
    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"], rM->getModelPosition("models/barrel.eml"), rM->getModelScale("models/barrel.eml"), rM->getModelRotation("models/barrel.eml"));
    rM->useModel("models/barrel.eml", Shaders["ShadowMapping"]);

    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"], rM->getModelPosition("models/Rock_6.eml"), rM->getModelScale("models/Rock_6.eml"), rM->getModelRotation("models/Rock_6.eml"));
    rM->useModel("models/Rock_6.eml", Shaders["ShadowMapping"]);


    Shaders["ShadowMapping"]->Use();
    this->SetUniforms(Shaders["ShadowMapping"], rM->getModelPosition("models/Tree.eml"), rM->getModelScale("models/Tree.eml"), rM->getModelRotation("models/Tree.eml"));
    rM->useModel("models/Tree.eml", Shaders["ShadowMapping"]);


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
    this->SetUniforms(Shaders["MD5ShadowMapping"], glm::vec3(mpos,0.0,-21.5), glm::vec3(0.1, 0.1, 0.1), glm::quat(-1.0, 1.0, 0.0, 0.0f));
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
    glm::mat4 choppedView = glm::mat4(glm::mat3(eCamera->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "choppedView"), 1, GL_FALSE, &choppedView[0][0]);
    glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &eCamera->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(shader->LightSpaceMatrix_Location, 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);

    glm::mat3 ModelView3x3Matrix = glm::mat3(Model * eCamera->getViewMatrix());
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse((Model/* * eCamera->ViewMatrix*/)));

    glUniformMatrix3fv(glGetUniformLocation(shader->getProgramID(), "MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(shader->getProgramID(), "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);

    glUniform3f(glGetUniformLocation(shader->getProgramID(), "LightDirection"), sun->Direction.x, sun->Direction.y, sun->Direction.z);
    glUniform4f(glGetUniformLocation(shader->getProgramID(), "clipplane"), 0.0, this->waterPlane->getActiveClip(), 0.0, this->waterPlane->position.y+30);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "viewPos"),  eCamera->getPosition().x, eCamera->getPosition().y, eCamera->getPosition().z);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "viewDir"),  eCamera->getDirection().x, eCamera->getDirection().y, eCamera->getDirection().z);
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"),  glfwGetTime());
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "flashLight"),  flashLight);

    glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraRight"),  eCamera->getViewMatrix()[0][0], eCamera->getViewMatrix()[1][0], eCamera->getViewMatrix()[2][0]);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraUp"),  eCamera->getViewMatrix()[0][1], eCamera->getViewMatrix()[1][1], eCamera->getViewMatrix()[2][1]);

    glUniform3f(glGetUniformLocation(shader->getProgramID(), "GrassPos"),  position.x, position.y, position.z);
}

void Epsilon::Render2D(void)
{
    std::string obj = rM->m_PhysicsWorld->RayCollision(btVector3(eCamera->getPosition().x, eCamera->getPosition().y, eCamera->getPosition().z),
                      btVector3(eCamera->getDirection().x * 1000, eCamera->getDirection().y * 1000, eCamera->getDirection().z * 1000));

    this->text->RenderText(std::string("Pointing at: ") + obj , 0.01, 0.83, 0.3, glm::vec3(1,0,0));

    //this->text->RenderText("Epsilon Engine Alpha Developer Build. Build: " + Helpers::intTostring(AutoVersion::BUILD), 0.01, 0.97, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("Speed: " + floatTostring(eCamera->MovementSpeed), 0.01, 0.95, 0.3, glm::vec3(1,1,1));
    this->text->RenderText
    ("Position: x = " + Helpers::floatTostring(this->eCamera->getPosition().x) + " y = " + Helpers::floatTostring(this->eCamera->getPosition().y) + " z = " + Helpers::floatTostring(this->eCamera->getPosition().z), 0.01, 0.93, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText(std::string(normal ? "Normal Mapping: ON" : "Normal Mapping: OFF"),0.01, 0.91, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("Frame Time: " + Helpers::floatTostring(frametime*1000) + "ms", 0.01, 0.89, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("FPS: " + Helpers::intTostring(fps), 0.01, 0.87, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("SSAO: " + std::string(SSAO ? "ON" : "OFF"),0.01, 0.85, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("Parallax Mapping: " + std::string(parallax ? "ON" : "OFF"),0.01, 0.83, 0.3, glm::vec3(1,1,1));

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
    this->SetUniforms(Shaders["Sun"], glm::vec3(1,1,1), glm::vec3(1,1,1), glm::quat(0.0, 0.0, 0.0, 0.0) );
    sun->Render(Shaders["Sun"]);
}

void Epsilon::PollEvents(void)
{
    if(glfwGetWindowAttrib(window, GLFW_FOCUSED))
        glfwPollEvents();
    else
        glfwWaitEvents();

    if(Input::KeyBoard::KEYS[Input::GLFW::Key::ESCAPE])
        Running = false;

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
}

void Epsilon::MainLoop(void)
{
    while(Running)
    {
        this->ClearBuffers();

        this->Clock();

        this->PollEvents();

        this->eCamera->Update(this->window);/*
         btTransform characterWorldTrans;

        m_PlayerCapsule->Movement();
        characterWorldTrans = m_PlayerCapsule->m_ghostObject->getWorldTransform();

        btVector3 pos = characterWorldTrans.getOrigin();
        this->eCamera->setPosition(glm::vec3(pos.getX(), pos.getY()+5,pos.getZ()));
*/
        this->eCamera->UpdateMatrices();

        rM->m_PhysicsWorld->Update(frametime);

        ComputeWater();

        this->ComputeShadow();

        this->ProcessFrame();

        this->RenderFrame();

        this->Render2D();

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

void Epsilon::ComputeWater(void)
{
    /**
        Render Reflection to Texture
    */
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_CLIP_DISTANCE0);

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

    /**
        Render Refraction to Texture
    */

    this->waterPlane->GenerateRefraction(this->eCamera);
    this->eCamera->UpdateMatrices();
    this->waterPlane->setActiveClip(-1);
    this->Render3D(Shaders["Main"]);
    this->RenderSkybox(false);
    this->waterPlane->FinishWatercomputation();
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

    this->Render3D();

    shadowMap->UnbindShadowFrameBuffer();
}

void Epsilon::ProcessFrame(void)
{

    PP->beginOffScreenrendering();

    this->RenderSkybox(true);

    Shaders["grass"]->Use();
    this->SetUniforms(Shaders["grass"], glm::vec3(-512, -10, 512), glm::vec3(1,1,1), glm::quat(-1, 0, -1, 0) );
    grass.at(0).Render(Shaders["grass"]);

    Shaders["grass"]->Use();
    this->SetUniforms(Shaders["grass"], glm::vec3(-512, -10, 512), glm::vec3(1,1,1), glm::quat(-1, 0, -1, 0) );
    grass.at(1).Render(Shaders["grass"]);

    this->Render3D(Shaders["Main"]);
    this->waterPlane->RenderWater(eCamera, sun->Direction);


    if(this->SSAO)
        PP->applySSAO(this->eCamera);
}

void Epsilon::RenderFrame(void)
{
    glViewport(0,0, this->WIDTH, this->HEIGHT);

    PP->ShowFrame(sun->Direction, SSAO, this->eCamera, exposure, this->shadowMap);

    this->RenderSkybox(false);

    PP->ShowPostProcessImage(exposure, shadowMap->getShadowTextureID());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
