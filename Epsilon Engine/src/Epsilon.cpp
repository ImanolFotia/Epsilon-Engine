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

Epsilon::Epsilon(GLFWwindow* win)
{
    window = win;
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

    cout << endl << endl << "Gathering Video Device information..." << endl;

    std:: cout << "OpenGL VENDOR: " << glGetString(GL_VENDOR) << std::endl;
    std:: cout << "Video RENDERER: " << glGetString(GL_RENDERER) << std::endl;
    std:: cout << "OpenGL VERSION: " << glGetString(GL_VERSION) << std::endl;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

    int li = LIGHT_DIRECTIONAL;

}

void Epsilon::InitResources(void)
{
    cout << "Initializing Resources..." << endl;

    ProgramData DATA;

    this->WIDTH = DATA.WINDOW_WIDTH;
    this->HEIGHT = DATA.WINDOW_HEIGHT;
    this->SSAO = DATA.SSAO;

    eCamera = new Camera(glm::vec3(-30.0, 5.0, -120.0), glm::vec3(0.0f,0.0f,0.0f));

    text = new Text("resources/arial.ttf", DATA.WINDOW_WIDTH, DATA.WINDOW_HEIGHT);

    PP = new PostProcess();

    lightPositions.push_back(glm::vec3(-29, 4.5, -11));
    lightPositions.push_back(glm::vec3(-29, 4.5, 11));
    lightPositions.push_back(glm::vec3(29, 4.5, 11));
    lightPositions.push_back(glm::vec3(29, 4.5, -11));

    this->LoadShaders();

    this->LoadGeometry();

    this->LoadSound();

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
}

void Epsilon::LoadGeometry(void)
{
    cout << "Loading World Geometry..." <<endl;

    /*
       for(int i = 0 ; i < 50 ; i++)
       {
           grassPos.push_back(glm::vec3(  0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(30-10)))  ,-11.3, 0 + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(30-0))) ));
           cout << grassPos.at(i).x << grassPos.at(i).y << grassPos.at(i).z << endl;
       }
    */
    skybox = new Skybox("plain");

    //terrain = new Terrain("materials/mountains.jpg", "mud-diffuse.jpg", "mud-normal.jpg", "awp_india_texture_4s.jpg", 1, 1);

    //model.push_back(Model("models/sponza.obj"));

    model.push_back(Model("models/bigboss.obj"));

    grass.push_back(Grass("billboardgrass0002.png"));

    waterPlane = new Water(glm::vec3(0,4.1,0), 100.0f); ///-11.8

    sun = new Sun();

    BSPMap = new CQuake3BSP();

    BSPMap->LoadBSP((string("maps/") + "room.bsp").c_str());

    m_AnimModel = new MD5Model();


    m_AnimModel->LoadModel("models/hellknight/hellknight.md5mesh");
    m_AnimModel->LoadAnim("models/hellknight/idle2.md5anim");
    m_AnimModel->LoadAnim("models/hellknight/idle2.md5anim");
}

void Epsilon::LoadSound(void)
{
    cout << "Loading Sound..." << endl;
}
bool visible = true;
void Epsilon::Render3D(int clip)
{

    bool underWater = ((this->eCamera->getPosition().y < this->waterPlane->position.y) ? true : false);
/*
        Shaders["Main"]->Use();
        this->SetUniforms(Shaders["Main"], glm::vec3(190,-40,10), glm::vec3(1,1,1), glm::vec3(1,1,1) );
        terrain->RenderTerrain(Shaders["Main"]);
*/
/*
    Shaders["Main"]->Use();
    this->SetUniforms(Shaders["Main"], glm::vec3(16,-5,3.5), glm::vec3(0.025,0.025,0.025));
    glUniform1f(glGetUniformLocation(this->Shaders["Main"]->getProgramID(), "plane"), this->waterPlane->position.y);
    glUniform1i(glGetUniformLocation(this->Shaders["Main"]->getProgramID(), "clip_Direction"),  clip);
    if(clip == 0)
        glUniform1i(glGetUniformLocation(this->Shaders["Main"]->getProgramID(), "BelowWater"), underWater);
    model.at(0).Draw(Shaders["Main"]);
*/

    Shaders["Main"]->Use();
    this->SetUniforms(Shaders["Main"], glm::vec3(55.7,0.75,5.5), glm::vec3(3.0,3.0,3.0), glm::vec3(1.0, 1.0, 1.0));
    glUniform1f(glGetUniformLocation(this->Shaders["Main"]->getProgramID(), "plane"), this->waterPlane->position.y);
    glUniform1i(glGetUniformLocation(this->Shaders["Main"]->getProgramID(), "clip_Direction"),  clip);
    if(clip == 0)
        glUniform1i(glGetUniformLocation(this->Shaders["Main"]->getProgramID(), "BelowWater"), underWater);
    glm::mat4 Model = glm::mat4();
    glm::mat4 ScaleMatrix = glm::scale(glm::mat4(), glm::vec3(3.0,3.0,3.0));
    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), glm::vec3(55.7,0.75,5.5));

    Model = TranslationMatrix * ScaleMatrix;
    BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), Model);
/*
    visible = BSPMap->Frustum.BoxInFrustum(
                  model.at(0).MinMaxPoints.MAX_X,
                  model.at(0).MinMaxPoints.MAX_Y,
                  model.at(0).MinMaxPoints.MAX_Z,
                  model.at(0).MinMaxPoints.MIN_X,
                  model.at(0).MinMaxPoints.MIN_Y,
                  model.at(0).MinMaxPoints.MIN_Z
              );*/
    if(visible)
    {
        //cout << "visible" << endl;
        model.at(0).Draw(Shaders["Main"]);
    }
    else
    {
        //cout << "no visible" << endl;
    }


        glCullFace(GL_FRONT);
        glm::mat4 BSPmodel = glm::mat4();
        //glm::mat4 tmodel = glm::translate(glm::mat4(), glm::vec3(-30.0, 5.0, -120.0));
        glm::mat4 sModel = glm::scale(glm::mat4(), glm::vec3(0.1, 0.1, 0.1));
        BSPmodel = sModel;
        BSPMap->Frustum.CalculateFrustum(glm::mat4(eCamera->getProjectionMatrix() * eCamera->getViewMatrix()), BSPmodel);
        Shaders["Main"]->Use();
        this->SetUniforms(Shaders["Main"],  glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1, 0.1, 0.1), glm::vec3(1.0, 1.0, 1.0));
        BSPMap->RenderLevel(eCamera->getPosition(), Shaders["Main"]->getProgramID(), window);



        m_AnimModel->Update(frametime, 0.0);
        Shaders["MD5Geometry"]->Use();
        glUniformMatrix4fv(glGetUniformLocation(Shaders["MD5Geometry"]->getProgramID(), "mSkinned"), 200, GL_FALSE, &m_AnimModel->m_AnimatedBones[0][0][0]);
        this->SetUniforms(Shaders["MD5Geometry"], glm::vec3(55.7 - 55.0 ,0.40 + 5,5.5 - 120), glm::vec3(0.05, 0.05, 0.05), glm::vec3(1.0, 0.0, 0.0), 270.0f);
        m_AnimModel->Render(Shaders["MD5Geometry"]->getProgramID());
        glCullFace(GL_BACK);
    /*
            for(int i = 0 ; i < grassPos.size() ; i++){
            Shaders["grass"]->Use();
            this->SetUniforms(Shaders["grass"], grassPos.at(i), glm::vec3(1,1,1));
            glUniform1f(glGetUniformLocation(this->Shaders["grass"]->ProgramID, "plane"), this->waterPlane->position.y);
            glUniform1i(glGetUniformLocation(this->Shaders["grass"]->ProgramID, "clip_Direction"),  clip);
            if(clip == 0)
            glUniform1i(glGetUniformLocation(this->Shaders["grass"]->ProgramID, "BelowWater"), underWater);
            grass.at(0).Render(Shaders["grass"]);
            }
    */

}

void Epsilon::SetUniforms(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float degree)
{
    glm::mat4 Model = glm::mat4();
    glm::mat4 ScaleMatrix = glm::scale(glm::mat4(), scale);
    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), position);
    glm::mat4 RotationMatrix;
    RotationMatrix = glm::rotate(RotationMatrix, glm::radians(degree), rotation);

    Model = TranslationMatrix * ScaleMatrix * RotationMatrix;

    glm::mat4 MVP = eCamera->getProjectionMatrix() * eCamera->getViewMatrix() * Model;
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "MVP"), 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "model"), 1, GL_FALSE, &Model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "view"), 1, GL_FALSE, &eCamera->getViewMatrix()[0][0]);
    glm::mat4 choppedView = glm::mat4(glm::mat3(eCamera->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "choppedView"), 1, GL_FALSE, &choppedView[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, &eCamera->getProjectionMatrix()[0][0]);

    glm::mat3 ModelView3x3Matrix = glm::mat3(Model * eCamera->getViewMatrix());
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse((Model/* * eCamera->ViewMatrix*/)));

    glUniformMatrix3fv(glGetUniformLocation(shader->getProgramID(), "MV3x3"), 1, GL_FALSE, &ModelView3x3Matrix[0][0]);
    glUniformMatrix3fv(glGetUniformLocation(shader->getProgramID(), "normalMatrix"), 1, GL_FALSE, &NormalMatrix[0][0]);

    glUniform3f(glGetUniformLocation(shader->getProgramID(), "LightDirection"), sun->Direction.x, sun->Direction.y, sun->Direction.z);

    glUniform1i(glGetUniformLocation(shader->getProgramID(), "normal"), normal);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "viewPos"),  eCamera->getPosition().x, eCamera->getPosition().y, eCamera->getPosition().z);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "viewDir"),  eCamera->getDirection().x, eCamera->getDirection().y, eCamera->getDirection().z);
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "cut"),  glm::cos(glm::radians(20.5f)));
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "cutoff"),  glm::cos(glm::radians(25.5f)));
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"),  glfwGetTime());
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "flashLight"),  flashLight);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "parallax"),  parallax);

    glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraRight"),  eCamera->getViewMatrix()[0][0], eCamera->getViewMatrix()[1][0], eCamera->getViewMatrix()[2][0]);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "CameraUp"),  eCamera->getViewMatrix()[0][1], eCamera->getViewMatrix()[1][1], eCamera->getViewMatrix()[2][1]);

    glUniform3f(glGetUniformLocation(shader->getProgramID(), "GrassPos"),  position.x, position.y, position.z);


}

void Epsilon::Render2D(void)
{

    this->text->RenderText("Epsilon Engine Alpha Developer Build.", 0.01, 0.97, 0.3, glm::vec3(1,1,1));
    this->text->RenderText("Speed: " + floatTostring(eCamera->MovementSpeed), 0.01, 0.95, 0.3, glm::vec3(1,1,1));
    this->text->RenderText    /**
        Purpose: Set up buffers for blurring
    */("Position: x = " + floatTostring(this->eCamera->getPosition().x) + " y = " + floatTostring(this->eCamera->getPosition().y) + " z = " + floatTostring(this->eCamera->getPosition().z), 0.01, 0.93, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText(std::string(normal ? "Normal Mapping: ON" : "Normal Mapping: OFF"),0.01, 0.91, 0.3, glm::vec3(1,1,1));
    this->text->RenderText("Frame Time: " + floatTostring(frametime*1000) + "ms", 0.01, 0.89, 0.3, glm::vec3(1,1,1));
    //this->text->RenderText("FPS: " + intTostring(fps), 0.01, 0.87, 0.3, glm::vec3(1,1,1));
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

    sun->Update();
}

void Epsilon::RenderSkybox(void)
{
    Shaders["SkyBox"]->Use();
    glUniform3f(glGetUniformLocation(Shaders["SkyBox"]->getProgramID(), "LightDirection"), sun->Direction.x, sun->Direction.y, sun->Direction.z);
    skybox->Render(this->eCamera, Shaders["SkyBox"]);

    Shaders["Sun"]->Use();
    this->SetUniforms(Shaders["Sun"], glm::vec3(1,1,1), glm::vec3(1,1,1), glm::vec3(0.0, 0.0, 0.0) );
    sun->Render(Shaders["Sun"]);
}

void Epsilon::PollEvents(void)
{
    glfwPollEvents();
    if(eventtime > 0.5)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE ) == GLFW_PRESS)
            Running = false;

        if(glfwGetKey(window, GLFW_KEY_N ) == GLFW_PRESS)
            normal = !normal;

        if(glfwGetKey(window, GLFW_KEY_F ) == GLFW_PRESS)
            flashLight = !flashLight;

        if(glfwGetKey(window, GLFW_KEY_SPACE ) == GLFW_PRESS)
            SSAO = !SSAO;

        if(glfwGetKey(window, GLFW_KEY_P ) == GLFW_PRESS)
            parallax = !parallax;

        eventtime = 0;
    }
    if(glfwGetKey(window, GLFW_KEY_KP_ADD ) == GLFW_PRESS)
        exposure += 0.5 * frametime;

    if(glfwGetKey(window, GLFW_KEY_KP_SUBTRACT ) == GLFW_PRESS)
        exposure -= 0.5 * frametime;
}

void Epsilon::MainLoop(void)
{
    while(Running)
    {
        this->ClearBuffers();

        this->Clock();

        this->PollEvents();

        this->eCamera->Update(this->window);

        this->eCamera->UpdateMatrices();
        /*
                this->ComputeWater();
        */
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

void Epsilon::ComputeWater(void)
{
    /**
        Render Reflection to Texture
    */
    this->waterPlane->GenerateReflection(this->eCamera);
    float distance = 2 * (this->eCamera->getPosition().y - this->waterPlane->position.y);
    this->eCamera->setPosition(glm::vec3(this->eCamera->getPosition().x, this->eCamera->getPosition().y - distance, this->eCamera->getPosition().z));
    this->eCamera->setDirection(glm::vec3(this->eCamera->getDirection().x, this->eCamera->getDirection().y * -1, this->eCamera->getDirection().z));
    this->eCamera->UpdateMatrices();
    this->Render3D(-1);
    this->RenderSkybox();
    this->eCamera->setPosition(glm::vec3(this->eCamera->getPosition().x, this->eCamera->getPosition().y + distance, this->eCamera->getPosition().z));
    this->eCamera->setDirection(glm::vec3(this->eCamera->getDirection().x, this->eCamera->getDirection().y * -1, this->eCamera->getDirection().z));
    this->waterPlane->FinishWatercomputation();

    /**
        Render Refraction to Texture
    */
    this->waterPlane->GenerateRefraction(this->eCamera);
    this->eCamera->UpdateMatrices();
    this->Render3D(1);
    this->RenderSkybox();
    this->waterPlane->FinishWatercomputation();

}

void Epsilon::ProcessFrame(void)
{
    PP->beginOffScreenrendering();

    this->Render3D(0);

    this->RenderSkybox();
    /*
        this->waterPlane->RenderWater(this->eCamera, sun->Direction);
    */
    PP->endOffScreenRendering();

    if(this->SSAO)
        PP->applySSAO(this->eCamera);
}

void Epsilon::RenderFrame(void)
{
    glViewport(0,0, this->WIDTH, this->HEIGHT);

    PP->ShowFrame(sun->Direction, SSAO, this->eCamera, exposure);

    this->RenderSkybox();

    PP->ShowPostProcessImage(exposure);
}
