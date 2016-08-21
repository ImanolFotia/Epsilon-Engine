///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef EPSILON_H_INCLUDED
#define EPSILON_H_INCLUDED

#include <map>
#define GLEW_STATIC
#define GLFW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <Texture.h>
#include <Shader.h>
#include <Skybox.h>
#include <camera.h>
#include <Model.h>
#include <Text.h>
#include <Terrain.h>
#include <Grass.h>
#include <PostProcess.h>
#include <Water.h>
#include <Sun.h>
#include <BSP.h>
#include <MD5_Model.h>
#include <MD5_Anim.h>
#include <ShadowMapping.h>
#include <EntityTemplate.h>
#include <memory>
#include <ResourceManager.h>
#include <Scene.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <sys/KeyBoard.h>
#include <sys/Mouse.h>
#include <sys/Joystick.h>
#include <Player.h>
#include <Audio.h>
#include <AudioElement.h>
#include <AudioListener.h>

class Epsilon
{
public:

    Epsilon(GLFWwindow*&);

    virtual ~Epsilon(void)
    {

        for(std::map<std::string, Shader*>::iterator itr = Shaders.begin(); itr != Shaders.end(); itr++)
        {
            delete itr->second;
            itr->second = nullptr;
        }

        cout << "Epsilon Engine has closed Succesfully." << endl;
    }

    void MainLoop(void);

    void InitResources(void);

private:

    void LoadGeometry(void);

    void LoadSound(void);

    void LoadShaders(void);

    void PollEvents(void);

    void Render3D(Shader*);

    void Render3D(void);

    void Render2D(void);

    void ProcessAudio(void);

    void RenderLegacy(void);

    void RenderSkybox(bool);

    void ComputeWater(void);

    void SwapBuffers(void);

    void ClearBuffers(void);

    void RenderFrame(void);

    void ProcessFrame(void);

    void SetUniforms(Shader*&, glm::vec3 position, glm::vec3 scale, glm::quat rotation);

    void Clock(void);

    void ComputeShadow(void);


public:

    std::unique_ptr<Camera> eCamera;

    /**Shaders**/
    std::map<string, Shader*> Shaders;
    std::unique_ptr<Skybox> skybox;
    std::unique_ptr<Water> waterPlane;
    std::vector<Grass> grass;
    std::vector<Model> model;
    GLuint VertexArrayID;
    GLFWwindow* window = nullptr;
    std::unique_ptr<Text> text;
    std::unique_ptr<Text> text2;
    //std::shared_ptr<Terrain> terrain;
    std::shared_ptr<Sun> sun;
    std::unique_ptr<CQuake3BSP> BSPMap;
    std::unique_ptr<MD5Model> m_AnimModel;
    std::unique_ptr<ShadowMap> shadowMap;
    std::unique_ptr<PostProcess> PP;
    std::shared_ptr<EntityTemplate> EntityTest[2];
    std::unique_ptr<Audio::AudioElement> m_AudioElement;
    std::unique_ptr<Audio::Audio> m_AudioSystem;
    std::unique_ptr<Audio::AudioListener> m_AudioListener;

    //std::shared_ptr<Game::Player> m_PlayerCapsule;
    /** Window Properties **/

    short WIDTH = 16;
    short HEIGHT = 16;
    bool SSAO = false;
private:
    bool Running = true;
    bool normal = 0;
    bool flashLight = 0;
    bool hdr = true;
    double lastTime = 0;
    double frametime = 0;
    double etime = 0;
    float exposure = 0;
    bool parallax = false;
    float eventtime = 0;
    short fps = 0;
    bool showtext = false;
    vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> grassPos;
    std::ostringstream fpss;
    std::shared_ptr<ResourceManager> rM;
    std::shared_ptr<Physics::SpherePhysicObject> ph3;
};

#endif /// EPSILON_H_INCLUDED
