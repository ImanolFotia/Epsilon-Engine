#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <Texture.h>
#include <Model.h>
#include <Water.hpp>
#include <Terrain.h>
#include <Grass.h>
#include <map>
#include <Shader.h>
#include <CubeMap.h>
#include <Includes.h>
#include <Physics/Physics.h>
#include <Audio/Audio.h>

class ResourceManager
{
public:

    ResourceManager(const ResourceManager&) = delete;

    static ResourceManager& Get() {
        return instance;
    }

    virtual ~ResourceManager()
    {
        //Destroy();
    }

    void Destroy() {

        for(std::map<std::string, std::shared_ptr<eTexture> >::iterator itr = TextureList.begin(); itr != TextureList.end(); itr++)
        {
            GLuint tex = itr->second->getTextureID();
            itr->second->Destroy();
        }

        for(std::map<std::string, Model>::iterator itr = ModelList.begin(); itr != ModelList.end(); itr++)
        {
            itr->second.Destroy();
        }
        std::cout << "Deleted Resource Manager" << std::endl;
    }

    std::string requestTexture(std::string texPath);

    std::string requestModel(std::string modelPath, glm::vec3 Pos, glm::vec3 scs, glm::quat rot);

    void useModel(std::string modelPath, Shader* shader, glm::vec3);

    void useModel(std::string modelPath, GLuint shader, glm::vec3);

    MIN_MAX_POINTS getModelBoundingBox(std::string modelPath);

    void setModelVisibility(std::string path ,bool visibility);

    glm::quat getModelRotation(std::string path);

    glm::vec3 getModelPosition(std::string path);

    glm::vec3 getModelScale(std::string path);

    void setModelUniforms(std::string, Shader* , glm::vec3, glm::vec3, glm::quat, std::shared_ptr<Camera>);

    void setModelUniforms(std::string path, Shader* shader, glm::vec3, glm::vec3, glm::quat, glm::vec3, glm::vec3, glm::quat, std::shared_ptr<Camera> cam);

    void destroyAllModels();

    GLuint useTexture(std::string texPath);

    void bindTexture(std::string texPath);

    void addTextureToQueue(std::string texture);

    void loadQueuedTextures();

    std::string requestShader(std::string shaderPathv, std::string shaderPathf, std::string name);

    void destroyAllTextures();

    std::shared_ptr<Shader> useShader(std::string shaderPath);

    GLuint getShaderID(std::string shaderPath);

    std::shared_ptr<Shader> getShader(std::string shaderPath);

    bool requestCubeMap(int CubeMapID, glm::vec3 Position);

    int NearestCubeMap(glm::vec3 TestingPoint);

    GLuint useCubeMap(int ID);

    int requestTextureUsage(std::string);

    void resetTextureUsage(std::string);

    bool addCubemap(std::shared_ptr<CubeMap>, glm::vec3);

    Model getModel(std::string modelPath);

    std::shared_ptr<Physics::Physics> getPhysicsWorld() {
        return m_PhysicsWorld;
    }

    uint32_t getNearestCubemapIndex(glm::vec3 pos) {
        return mCubemapIndex.at(NearestCubeMap(pos));
    }

    float timestep;
private:

    ResourceManager() {
        m_PhysicsWorld = std::make_shared<Physics::Physics>();
    }

    static ResourceManager instance;

    std::shared_ptr<Camera> m_Camera;

    std::shared_ptr<Physics::Physics> m_PhysicsWorld;

    std::shared_ptr<IO::Audio::Audio> m_AudioSystem;


    std::map<int, std::shared_ptr<CubeMap> > CubeMapList;

    std::vector<int> mCubemapIndex;

    std::map<std::string, std::shared_ptr<eTexture> >  TextureList;
    std::map<std::string, Water>    WaterPlanesList;
    std::map<std::string, Terrain>  TerrainList;
    std::map<std::string, Model>    ModelList;
    std::map<std::string, std::shared_ptr<Shader>> ShadersList;
    std::vector<glm::vec3> CubeMapPositions;

    /*!
    Temp Variables
    */
    std::vector<std::string> TextureQueue;

    unsigned int modelCounter;
    unsigned int textureCounter;

};

#endif // RESOURCEMANAGER_H
