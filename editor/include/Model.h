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
#define GLM_ENABLE_EXPERIMENTAL
#include <GL/glad.h> /// Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <include/Mesh.h>
#include <include/Types.h>
#include <memory>
#include <include/emlFormat.h>
#include <include/Types.h>
#include <include/texture.hpp>

struct ModelData
{
    glm::vec3 pos;
    glm::vec3 sc;
    glm::vec3 rot;
};

class Model
{
public:

    glm::vec3 Position = glm::vec3(0.0f,0.0f,0.0f), Scale;
    glm::quat Rotation;
    bool m_CastShadows;
    bool m_IsVisible;
    MODEL_TYPE m_Type;
    glm::vec3 m_Centroid;

    /**  Functions   */
    /// Constructor, expects a filepath to a 3D model.
    const char* path;

    Model(const char* path, glm::vec3 pos = glm::vec3(0,0,0), glm::vec3 sc = glm::vec3(0,0,0), glm::quat rot = glm::quat(0,0,0,1));

    std::string getPath()
    {
        return this->path;
    }

    ~Model()
    {
    }

    void Destroy()
    {
        for(int i = 0; i < meshes.size(); ++i)
            meshes.at(i).Destroy();
        meshes.clear();

        for(auto &t: textures_loaded) {
            t->Destroy();
        }

    }
    string directory;

    /// Draws the model, and thus all its meshes
    void Draw(GLuint shader);

    void DrawNoTexture()
        {
            for(GLuint i = 0; i < this->meshes.size(); i++)
                this->meshes[i].DrawNoTexture();
        }

    vector<Mesh> meshes;

    vector<std::shared_ptr<OpenGL::Texture> > textures_loaded;

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

    bool picked = false;
private:
    OpenGL::Texture gltex;
    /**  Model Data  */
    /// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.


bool loadModel(string emlPath, int a);

};



