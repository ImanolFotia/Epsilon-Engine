///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef EPSILON_H_INCLUDED
#define EPSILON_H_INCLUDED

#include <map>
#define GLEW_STATIC
#include <GL/glew.h>

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

class Epsilon
{
public:

    Epsilon(GLFWwindow*&);

    virtual ~Epsilon(void)
    {
        delete eCamera;
        delete skybox;
        delete waterPlane;
        delete text;
        //delete terrain;
        delete sun;
        delete m_AnimModel;
        delete PP;
        BSPMap->Destroy();
        delete BSPMap;
        delete shadowMap;

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

    void Render3D(int clip);

    void Render3D(void);

    void Render2D(void);

    void RenderLegacy(void);

    void RenderSkybox(void);

    void ComputeWater(void);

    void SwapBuffers(void);

    void ClearBuffers(void);

    void RenderFrame(void);

    void ProcessFrame(void);

    void SetUniforms(Shader*&, glm::vec3 position, glm::vec3 scale, glm::vec3 rotation, float degree = 0.0);

    void Clock(void);

    void ComputeShadow(void);


public:

    Camera* eCamera;

    /**Shaders**/
    std::map <string, Shader*> Shaders;
    Skybox* skybox;
    Water* waterPlane;
    std::vector<Grass> grass;
    std::vector<Model> model;
    GLuint VertexArrayID;
    GLFWwindow* window;
    Text* text;
    Terrain* terrain;
    Sun* sun;
    CQuake3BSP* BSPMap;
    MD5Model* m_AnimModel;
    ShadowMap* shadowMap;
    /** Window Properties **/

    short WIDTH;
    short HEIGHT;
    bool SSAO;
private:
    bool Running;
    bool normal;
    bool flashLight;
    bool hdr;
    double lastTime;
    double frametime;
    double etime;
    float exposure;
    bool parallax;
    float eventtime;
    short fps;
    vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> grassPos;
    std::ostringstream fpss;
    PostProcess* PP;

};

#endif /// EPSILON_H_INCLUDED
