#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <pch.hpp>
#include <Texture.h>
#include <Renderer/Model.h>
#include <Water.hpp>
#include <Renderer/Terrain.h>
#include <Grass.h>
#include <Shader.h>
#include <CubeMap.h>
#include <Helpers.hpp>
#include <Physics/Physics.h>
//#include <Audio/Audio.h>
#include <Renderer/Texture2D.hpp>
#include <Renderer/TextureCube.hpp>
#include <beacon/beacon.hpp>

namespace Epsilon
{
    class ResourceManager
    {
        using TextureArray = std::map<std::string, std::shared_ptr<Renderer::Texture2D>>;
        using Texture2D_ptr = std::shared_ptr<Renderer::Texture2D>;

    public:
        ResourceManager(const ResourceManager &) = delete;

        static ResourceManager &Get()
        {
            return instance;
        }

        static ResourceManager &Instance()
        {
            return instance;
        }

        virtual ~ResourceManager()
        {
            //Destroy();
        }

        void Destroy()
        {
            for (std::map<std::string, std::shared_ptr<Renderer::Texture2D>>::iterator itr = TextureList.begin(); itr != TextureList.end(); itr++)
            {
                //GLuint tex = itr->second->getTextureID();
                itr->second->Destroy();
            }

            for (std::map<std::string, std::shared_ptr<Renderer::Model>>::iterator itr = ModelList.begin(); itr != ModelList.end(); itr++)
            {
                itr->second->Destroy();
            }

            std::cout << "Deleted Resource Manager" << std::endl;
        }

        std::string requestTexture(const std::string &texPath);

        const std::string &requestModel(const std::string &modelPath, bool loadAsync = true);

        void deleteModel(const std::string&);

        //std::shared_ptr<Model> useModel(const std::string& modelPath, std::shared_ptr<Shader> shader, glm::vec3);

        //std::shared_ptr<Model> useModel(const std::string& modelPath, GLuint shader, glm::vec3);

        MIN_MAX_POINTS getModelBoundingBox(const std::string &modelPath);

        void setModelVisibility(const std::string &path, bool visibility);

        //void setModelUniforms(const std::string&, std::shared_ptr<Shader>, glm::vec3, glm::vec3, glm::quat, std::shared_ptr<Camera>);

        void setModelUniforms(const std::string &path, std::shared_ptr<Shader> shader, glm::vec3, glm::vec3, glm::quat, glm::vec3, glm::vec3, glm::quat, std::shared_ptr<Camera> cam);

        void destroyAllModels();

        GLuint useTexture(const std::string &texPath);

        void bindTexture(const std::string &texPath);

        void addTextureToQueue(const std::string &texture);
        void addModelToQueue(const std::string &modelPath);

        std::future<uint8_t *> LoadTextureAsync(std::string path);

        std::future<void> LoadModelAsync(std::string);

        void getTextureInfo(const std::string &path, int *, int *, int *);

        void loadQueuedTextures();

        std::string requestShader(const std::string &shaderPathv, const std::string &shaderPathf, const std::string &name);
        std::string requestShader(const std::string &shaderPathv, const std::string &shaderPathf, const std::string &shaderPathg, const std::string &name);

        void destroyAllTextures();

        std::shared_ptr<Shader> useShader(const std::string &shaderPath);

        GLuint getShaderID(const std::string &shaderPath);

        std::shared_ptr<Shader> getShader(const std::string &shaderPath);

        bool requestCubeMap(int CubeMapID, glm::vec3 Position);

        int NearestCubeMap(glm::vec3 TestingPoint);

        GLuint useCubeMap(int ID);

        std::shared_ptr<Renderer::TextureCube> getCubemap(int Id);

        int requestTextureUsage(const std::string &);

        void resetTextureUsage(const std::string &);

        bool addCubemap(std::shared_ptr<CubeMap>, glm::vec3);

        void updateCubemap(uint32_t, std::shared_ptr<CubeMap>);

        [[nodiscard]] std::shared_ptr<Renderer::Model> getModel(const std::string &modelPath);

        template <class T>
        [[maybe_unused]] T Get(const std::string &);

        std::shared_ptr<Physics::Physics> getPhysicsWorld()
        {
            return m_PhysicsWorld;
        }

        uint32_t getNearestCubemapIndex(glm::vec3 pos);

        uint32_t getNumCubemaps();

        uint32_t getNumTextures();

        uint32_t getNumModels()
        {
            return ModelList.size();
        }

        const TextureArray &getTextureArray();

        Texture2D_ptr getTexture2D(const std::string &name);

        unsigned long getTextureBytesAllocation();

        bool ShouldLoadQueuedTextures();

        bool ShouldLoadQueuedModels()
        {
            return mShouldLoadQueuedModels;
        }

        void LoadModel(const std::string &model_id)
        {

            std::cout << "Calling moadModel with argument: " << model_id << std::endl;
            ModelList.at(model_id)->loadModel(model_id, 0);
        }

        void loadQueuedModels();

        float timestep;

        bool cubemapsLoaded = false;

        void LaunchIntervals()
        {

            mQueuedTexturesInterval.set(100, [&mShouldLoadQueuedTextures = mShouldLoadQueuedTextures]()
                                        { mShouldLoadQueuedTextures = true; });

            mQueuedModelsInterval.set(100, [&mShouldLoadQueuedModels = mShouldLoadQueuedModels]()
                                      { mShouldLoadQueuedModels = true; });
        }

        void ForceLoading() {
            while(ModelQueue.size() > 0 || ModelQueueFuture.size() > 0) {
                loadQueuedModels();
            }
            
            while(TextureQueue.size() > 0 || TextureQueueFuture.size() > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
                loadQueuedTextures();
            }
        }

    private:
        ResourceManager()
        {
            m_PhysicsWorld = std::make_shared<Physics::Physics>();
        }

        uint8_t *LoadTexture(std::string path);

        static ResourceManager instance;

        std::shared_ptr<Physics::Physics> m_PhysicsWorld;

        //std::shared_ptr<IO::Audio::Audio> m_AudioSystem;

        std::map<int, std::shared_ptr<CubeMap>> CubeMapList;

        std::vector<int> mCubemapIndex;

        TextureArray TextureList;
        std::map<std::string, Water> WaterPlanesList;
        std::map<std::string, Terrain> TerrainList;
        std::map<std::string, std::shared_ptr<Renderer::Model>> ModelList;
        std::map<std::string, std::shared_ptr<Shader>> ShadersList;
        std::map<std::string, std::future<uint8_t *>> TextureQueueFuture;
        std::map<std::string, std::future<void>> ModelQueueFuture;
        std::vector<glm::vec3> CubeMapPositions;
        bool mShouldLoadQueuedTextures = false;
        bool mShouldLoadQueuedModels = false;

        bool mPrevTextureWasLoaded = true;

        uint32_t mFrameSinceLastUpload = 0;

        /*!
    Temp Variables
    */
        std::vector<std::string> TextureQueue;
        std::vector<std::string> ModelQueue;
        beacon::interval mQueuedTexturesInterval;
        beacon::interval mQueuedModelsInterval;

        unsigned int modelCounter;
        unsigned int textureCounter;
        unsigned long mTextureMemoryAllocated = 0;

        std::atomic<unsigned int> mOpenFiles = 0;
    };
} // namespace Epsilon
#endif // RESOURCEMANAGER_H
