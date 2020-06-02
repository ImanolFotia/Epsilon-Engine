///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#pragma once
/// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
/// GL Includes
#include <GL/glew.h> /// Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Texture.h>
#include <Mesh.h>
#include <Types.h>
#include <memory>
#include <emlFormat.h>
#include <camera.h>
class ResourceManager;



class Model
{
public:

    glm::vec3 Position = glm::vec3(0,0,0), Scale;
    glm::quat Rotation;
    glm::mat4 PrevModel;
    glm::mat4 ModelMatrix;
    glm::vec3 PrevPos, PrevScale;
    glm::quat PrevRot;
    bool m_CastShadows;
    bool m_IsVisible;
    MODEL_TYPE m_Type;

    /**  Functions   */
    /// Constructor, expects a filepath to a 3D model.
    const char* path;

    Model(const char* path)
    {
        this->loadModel(path, 1);
        this->path = path;

        //cout << "Cantidad de texturas: " << textures_loaded.size() << endl;
    }

    Model(const char* path, glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 sc = glm::vec3(0,0,0), glm::quat rot = glm::quat(0,0,0,1));

    std::string getPath()
    {
        return this->path;
    }

    void Destroy()
    {
        for(unsigned i = 0; i < meshes.size(); ++i)
            meshes[i].Destroy();
    }

    ~Model()
    {
        //std::cout << "Deleted Model" << std::endl;
    }
    string directory;
    /// Draws the model, and thus all its meshes
    void Draw(Shader* shader, glm::vec3);

    /// Draws the model, and thus all its meshes
    void Draw(GLuint shader, glm::vec3);

    /// Draws the model, and thus all its meshes
    void DrawNoTexture()
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].DrawNoTexture();
    }

    void SetUniforms(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation,
                                      glm::vec3 pposition, glm::vec3 pscale, glm::quat protation,
                                      std::shared_ptr<Camera> cam);

    void SetUniforms(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation,
                                      std::shared_ptr<Camera> cam);
    vector<Mesh> meshes;

    vector<Texture> textures_loaded;

    /// Structure to store the models bounding box for visibility and collision computation
    struct BOUNDING_BOX
    {
        float FRONT_TOP_LEFT = 0.f;
        float FRONT_TOP_RIGHT = 0.f;
        float FRONT_BOTTOM_LEFT = 0.f;
        float FRONT_BOTTOM_RIGHT = 0.f;

        float BACK_TOP_LEFT = 0.;
        float BACK_TOP_RIGHT = 0.f;
        float BACK_BOTTOM_LEFT = 0.f;
        float BACK_BOTTOM_RIGHT = 0.f;
    };

    MIN_MAX_POINTS MinMaxPoints;

    BOUNDING_BOX BoundingBox;

    std::vector<MIN_MAX_POINTS> mMeshesBoundingBoxes;

    void setMeshVisibility(unsigned int meshIndex, bool state){
        meshes.at(meshIndex).isVisible = state;
    }

    long toHash()
    {
        return std::hash<std::string>{}(path);
    }


private:

bool loadModel(string emlPath, int a);

};



