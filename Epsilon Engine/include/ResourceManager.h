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
#include <Physics/Physics.h>
#include <Audio/Audio.h>

class ResourceManager
{
public:

    ResourceManager()
    {
        std::cout << "Created Resource Manager" << std::endl;
    }

    virtual ~ResourceManager()
    {
        for(std::map<std::string, std::shared_ptr<eTexture> >::iterator itr = TextureList.begin(); itr != TextureList.end(); itr++)
        {
            GLuint tex = itr->second->getTextureID();
            glDeleteTextures(1, &tex);
        }

        for(std::map<std::string, Model>::iterator itr = ModelList.begin(); itr != ModelList.end(); itr++)
        {
            itr->second.Destroy();
        }
        std::cout << "Deleted Resource Manager" << std::endl;
    }

    /** \brief Loads a texture from disk or assign the ID if it's already loaded
     *
     * \param Path to the texture
     * \return Path of the loaded texture
     */
    std::string requestTexture(std::string texPath);

    /** \brief Loads a model from disk or assign the ID if it's already loaded
     *
     * \param Path to the model
     * \param Instance of the Resource Manager
     * \param Model Position
     * \param Model Scale
     * \param Model Rotation
     * \return Path of the loaded model
     */
    std::string requestModel(std::string modelPath, std::shared_ptr<ResourceManager> rm, glm::vec3 Pos, glm::vec3 scs, glm::quat rot);

    /** \brief Renders the asked model
     *
     * \param Path to the model
     * \param Shader instance
     */
    void useModel(std::string modelPath, Shader* shader, glm::vec3);

    /** \brief Renders the asked model
     *
     * \param Path to the model
     * \param Shader ID
     */
    void useModel(std::string modelPath, GLuint shader, glm::vec3);

    /** \brief Retrieves the asked model's Mins and Max points contained in a structure
     *
     * \param Path to the model
     * \param Mins and Max points
     */
    MIN_MAX_POINTS getModelBoundingBox(std::string modelPath);

    /** \brief Based on the visibility calculation sets the current visibility state of the model
     *
     * \param Path to the model
     * \param State
     */
    void setModelVisibility(std::string path ,bool visibility);

    /** \brief Model assigned Rotation
     *
     * \param Path to the model
     * \return A quaternion with the orientation
     */
    glm::quat getModelRotation(std::string path);

    /** \brief Model assigned Position
     *
     * \param Path to the model
     * \return A 3 component vector with the position
     */
    glm::vec3 getModelPosition(std::string path);

    /** \brief Model assigned Scale
     *
     * \param Path to the model
     * \return A 3 component vector with the scale
     */
    glm::vec3 getModelScale(std::string path);

    /** \brief Set model's uniforms
     *
     * \param Path to the model
     * \param Position
     * \param Scale
     * \param Rotation
     * \return void
     */
    void setModelUniforms(std::string, Shader* , glm::vec3, glm::vec3, glm::quat, std::shared_ptr<Camera>);

    void setModelUniforms(std::string path, Shader* shader, glm::vec3, glm::vec3, glm::quat, glm::vec3, glm::vec3, glm::quat, std::shared_ptr<Camera> cam);

    void destroyAllModels();

    /** \brief Returns a Texture ID given a path
     *
     * \param Path to the texture
     * \return Texture ID
     */
    GLuint useTexture(std::string texPath);

    /** \brief Adds the path of a texture to be loaded when all the other resources are ready
     *
     * \param Path to the texture
     */
    void addTextureToQueue(std::string texture);

    /** \brief When there is no more queued commands, all the texture are loaded in order of assignment
     *
     */
    void loadQueuedTextures();

    /** \brief Loads a new shader into the engine or returns the existing one if already loaded
     *
     * \param Path to vertex shader
     * \param Path to fragment shader
     * \param Shader identification
     * \return Shader identification
     */
    std::string requestShader(std::string shaderPathv, std::string shaderPathf, std::string name);

    void destroyAllTextures();

    /** \brief Returns an instance of the requested shader
     *
     * \param Shader Name
     * \return Shader Instance
     */
    Shader useShader(std::string shaderPath);

    /** \brief Loads a new shader into the engine or returns the existing one if already loaded
     *
     * \param Shader Name
     * \return Shader ID
     */
    GLuint getShaderID(std::string shaderPath);

    /** \brief Loads a new Cube Map into the engine or returns the existing one if already loaded
     *
     * \param Cube Map Assign ID
     * \param Position of the cube map in world coordinates
     * \return Confirmation of correct loading
     */
    bool requestCubeMap(int CubeMapID, glm::vec3 Position);

    /** \brief Returns the ID of the closest cube map based on a given position
     *
     * \param Position to get the nearest cube map to.
     * \return Cube map ID
     */
    int NearestCubeMap(glm::vec3 TestingPoint);

    /** \brief Returns the ID of the requested cube map
     *
     * \param Cube map ID.
     * \return Cube-map Texture ID
     */
    GLuint useCubeMap(int ID);

     /** \brief Returns the number of times a texture is used per frame
     *
     * \param Texture name.
     * \return Number of times that texture was used in the frame
     */
    int requestTextureUsage(std::string);

    void resetTextureUsage(std::string);

    bool addCubemap(std::shared_ptr<CubeMap>, glm::vec3);

    Model getModel(std::string modelPath);

    std::shared_ptr<Camera> m_Camera;

    std::shared_ptr<Physics::Physics> m_PhysicsWorld;

    std::shared_ptr<IO::Audio::Audio> m_AudioSystem;

    float timestep;

    std::map<int, std::shared_ptr<CubeMap> > CubeMapList;

    std::vector<int> mCubemapIndex;
private:


    std::map<std::string, std::shared_ptr<eTexture> >  TextureList;
    std::map<std::string, Water>    WaterPlanesList;
    std::map<std::string, Terrain>  TerrainList;
    std::map<std::string, Model>    ModelList;
    std::map<std::string, Shader> ShadersList;
    std::vector<glm::vec3> CubeMapPositions;

    /*!
    Temp Variables
    */
    std::vector<std::string> TextureQueue;

    unsigned int modelCounter;
    unsigned int textureCounter;

};

#endif // RESOURCEMANAGER_H
