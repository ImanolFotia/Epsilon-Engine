#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <Texture.h>
#include <Model.h>
#include <Water.h>
#include <Terrain.h>
#include <Grass.h>
#include <map>
#include <Shader.h>
#include <CubeMap.h>
#include <Includes.h>
#include <Physics.h>

class ResourceManager
{
public:

    ResourceManager()
    {
        std::cout << "Created Resource Manager" << std::endl;
    }

    virtual ~ResourceManager()
    {
        std::cout << "Deleted Resource Manager" << std::endl;
        for(std::map<std::string, eTexture>::iterator itr = TextureList.begin(); itr != TextureList.end(); itr++)
        {
            GLuint tex = itr->second.getTextureID();
            glDeleteTextures(1, &tex);
        }
    }

    std::string requestTexture(std::string texPath);

    std::string requestModel(std::string modelPath, std::shared_ptr<ResourceManager> rm, glm::vec3 Pos, glm::vec3 scs, glm::quat rot);

    void useModel(std::string modelPath, Shader* shader);

    void useModel(std::string modelPath, GLuint shader);

    MIN_MAX_POINTS getModelBoundingBox(std::string modelPath);

    void setModelVisibility(std::string path ,bool visibility);

    glm::quat getModelRotation(std::string path);

    glm::vec3 getModelPosition(std::string path);

    glm::vec3 getModelScale(std::string path);

    GLuint useTexture(std::string texPath);

    void addTextureToQueue(std::string texture);

    void loadQueuedTextures();

    std::string requestShader(std::string shaderPathv, std::string shaderPathf, std::string name);

    Shader useShader(std::string shaderPath);


    GLuint getShaderID(std::string shaderPath);

    bool requestCubeMap(int CubeMapID, glm::vec3 Position);

    int NearestCubeMap(glm::vec3 TestingPoint);

    GLuint useCubeMap(int ID);

    std::shared_ptr<Camera> m_Camera;

    std::shared_ptr<Physics::Physics> m_PhysicsWorld;

    float timestep;

private:


    std::map<std::string, eTexture>  TextureList;
    std::map<std::string, Water>    WaterPlanesList;
    std::map<std::string, Terrain>  TerrainList;
    std::map<std::string, Model>    ModelList;
    std::map<std::string, Shader> ShadersList;
    std::map<int, CubeMap> CubeMapList;
    std::vector<glm::vec3> CubeMapPositions;


    /*!
    Temp Variables
    */

    std::vector<std::string> TextureQueue;

    unsigned int modelCounter;
    unsigned int textureCounter;

};

#endif // RESOURCEMANAGER_H
