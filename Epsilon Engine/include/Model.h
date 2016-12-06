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

typedef struct MIN_MAX_POINTS
{
    float MAX_X;
    float MAX_Y;
    float MAX_Z;

    float MIN_X;
    float MIN_Y;
    float MIN_Z;
};

class Model
{
public:

    glm::vec3 Position = glm::vec3(0,0,0), Scale;
    glm::quat Rotation;
    bool m_CastShadows;
    bool m_IsVisible;
    MODEL_TYPE m_Type;

    std::shared_ptr<ResourceManager> resm;

    /**  Functions   */
    /// Constructor, expects a filepath to a 3D model.
    const char* path;

    Model(const char* path)
    {
        this->loadModel(path, 1);
        this->path = path;

        //cout << "Cantidad de texturas: " << textures_loaded.size() << endl;
    }

    Model(const char* path, std::shared_ptr<ResourceManager> rm, glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 sc = glm::vec3(0,0,0), glm::quat rot = glm::quat(0,0,0,1));

    std::string getPath()
    {
        return this->path;
    }

    void Destroy()
    {
        for(int i = 0; i < meshes.size(); ++i)
            meshes[i].Destroy();
    }

    ~Model()
    {
        //std::cout << "Deleted Model" << std::endl;
    }
    string directory;
    /// Draws the model, and thus all its meshes
    void Draw(Shader* shader);

    /// Draws the model, and thus all its meshes
    void Draw(GLuint shader);

    /// Draws the model, and thus all its meshes
    void DrawNoTexture()
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].DrawNoTexture();
    }

    void SetUniforms(Shader*& shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation, std::shared_ptr<Camera> cam);

    vector<Mesh> meshes;

    vector<Texture> textures_loaded;

    /// Structure to store the models bounding box for visibility and collision computation
    typedef struct BOUNDING_BOX
    {
        float FRONT_TOP_LEFT;
        float FRONT_TOP_RIGHT;
        float FRONT_BOTTOM_LEFT;
        float FRONT_BOTTOM_RIGHT;

        float BACK_TOP_LEFT;
        float BACK_TOP_RIGHT;
        float BACK_BOTTOM_LEFT;
        float BACK_BOTTOM_RIGHT;
    };

    MIN_MAX_POINTS MinMaxPoints;

    BOUNDING_BOX BoundingBox;


private:

bool loadModel(string emlPath, int a);

};



