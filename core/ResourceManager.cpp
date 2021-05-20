#include "ResourceManager.h"
#include "ProgramData.h"

namespace Epsilon
{
    std::string ResourceManager::requestTexture(const std::string &texPath)
    {
        try
        {
            bool should_create = false;
            std::map<std::string, std::shared_ptr<Renderer::Texture2D>>::iterator it;
            if (TextureList.contains(texPath))
            {
                if (TextureList.at(texPath) != nullptr)
                {
                    if (!TextureList.at(texPath)->wasCreated())
                    {
                        should_create = true;
                    }
                }
                else
                {
                    should_create = true;
                }
            }
            it = TextureList.find(texPath);
            if (!should_create)
            {
                return texPath; //TextureList.at(it->first)->getPath();
            }
            else
            {
                /*std::shared_ptr<eTexture> tmpTex = (std::shared_ptr<eTexture>)new eTexture(texPath.c_str());
                TextureList.insert(std::make_pair(texPath, tmpTex));
                return texPath;*/

                ProgramData DATA;
                using Renderer::Texture2D;

                API::Texture::Texture::TextureData TextureData;
                TextureData.MakeDefaultGL();

                auto is_normal = Helpers::isNormal(texPath.c_str());
                TextureData.Compressed = !is_normal;
                TextureData.SRGB = !is_normal;

                int outwidth = 0, outheight = 0, outchannels = 0;
                auto path = ("materials/" + std::string(texPath)).c_str();
                auto data = SOIL_load_image(path, &outwidth, &outheight, &outchannels, SOIL_LOAD_RGBA);
                TextureData.Width = outwidth;
                TextureData.Height = outheight;
                int lod_zero_size = outwidth * outheight * sizeof(unsigned char) * outchannels;
                mTextureMemoryAllocated += (unsigned long)((float)lod_zero_size * 1.3);
                TextureData.AnisotropyLevel = DATA.ANISOTROPY;
                auto &tmpTex = TextureList.at(texPath);
                tmpTex->Create(TextureData);
                tmpTex->setData(data, 0);
                SOIL_free_image_data(data);
                data = nullptr;
                return texPath;
            }
        } 
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return "";
    }

    std::string ResourceManager::requestModel(const std::string &modelPath)
    {
        try
        {
            std::map<std::string, std::shared_ptr<Renderer::Model>>::iterator it;
            it = ModelList.find(modelPath);
            if (it != ModelList.end())
            {
                return ModelList.at(it->first)->getPath();
            }
            else
            {
                //std::shared_ptr<Model> tmpModel(modelPath.c_str());
                ModelList.insert(std::make_pair(modelPath, std::make_shared<Renderer::Model>(modelPath.c_str())));
                return modelPath;
            }
        }
        catch (std::runtime_error &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << "\nMessage: " << e.what() << std::endl;
            std::cout << "Failed to load: " << modelPath.c_str() << std::endl;
        }
        return "";
    }
    /*
    void ResourceManager::useModel(const std::string &modelPath, std::shared_ptr<Shader> shader, glm::vec3 pos = glm::vec3(0, 0, 0))
    {
        try
        {

            ModelList.at(modelPath)->Draw(shader);
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << "useModel(" << modelPath << ", shader"
                      << ",glm:vec3(" << pos.x << ", " << pos.y << ", " << pos.z << ") :::" << e.what() << std::endl;
        }
    }*/
    /*
void ResourceManager::useModel(std::string modelPath, GLuint shader, glm::vec3 pos = glm::vec3(0,0,0))
{
    try
    {
        ModelList.at(modelPath).Draw(shader, pos);
    }

    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << "(std::string modelPath, GLuint shader, glm::vec3 pos = glm::vec3(0,0,0)) :::" << e.what() <<std::endl;
    }
}*/

    [[nodiscard]] std::shared_ptr<Renderer::Model> ResourceManager::getModel(const std::string &modelPath)
    {
        try
        {
            return ModelList.at(modelPath);
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return nullptr;
    }

    MIN_MAX_POINTS ResourceManager::getModelBoundingBox(const std::string &modelPath)
    {
        try
        {
            return ModelList.at(modelPath)->MinMaxPoints;
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return MIN_MAX_POINTS();
    }

    void ResourceManager::setModelVisibility(const std::string &path, bool visibility)
    {
        ModelList.at(path)->m_IsVisible = visibility;
    }

    //void ResourceManager::setModelUniforms(const std::string& path, std::shared_ptr<Shader> shader, glm::vec3 pos, glm::vec3 sc, glm::quat rot, std::shared_ptr<Camera> cam)
    //{
    //ModelList.at(path).SetUniforms(shader, pos, sc, rot, cam);
    //}

    void ResourceManager::setModelUniforms(const std::string &path, std::shared_ptr<Shader> shader, glm::vec3 pos, glm::vec3 sc, glm::quat rot, glm::vec3 ppos, glm::vec3 psc, glm::quat prot, std::shared_ptr<Camera> cam)
    {
        ModelList.at(path)->SetUniforms(shader, pos, sc, rot, ppos, psc, prot, cam);
    }

    void ResourceManager::destroyAllModels()
    {
        for (std::map<std::string, std::shared_ptr<Renderer::Model>>::iterator itr = ModelList.begin(); itr != ModelList.end(); itr++)
        {
            itr->second->Destroy();
        }
    }

    GLuint ResourceManager::useTexture(const std::string &texPath)
    {
        try
        {
            if (texPath.empty() != true)
            {
                auto texture = TextureList.at(texPath);
                if (texture != nullptr)
                    return texture->ID();
                else
                    return 0;
            }
            else
                return 0;
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return -1;
    }

    void ResourceManager::bindTexture(const std::string &texPath)
    {

        std::cout << "trying to bind: " << texPath << std::endl;
        try
        {
            if (texPath.empty() != true)
            {
                if (TextureList.contains(texPath))
                {
                    if (TextureList.at(texPath) != nullptr)
                        TextureList.at(texPath)->Bind();
                }
                else
                {
                    requestTexture(texPath);
                }
            }
            else
                return;
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << "//" << texPath << std::endl;
        }
    }

    void ResourceManager::addTextureToQueue(const std::string &texture)
    {
        try
        {
            auto tmpTex = std::make_shared<Renderer::Texture2D>();
            if (!TextureList.contains(texture))
                TextureList.insert(std::make_pair(texture, tmpTex));
            TextureQueue.push_back(texture);

            std::cout << texture << " added to queue." << std::endl;
            //std::cout << texture << " Added to the Queue." << std::endl;
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }
    }

    void ResourceManager::loadQueuedTextures()
    {
        try
        {
#pragma omp
            if (TextureQueue.size() > 0)
            {
                for (int i = static_cast<int>(TextureQueue.size())-1; i >= 0; i--)
                {
                    std::cout << "Loading queued texture: " << TextureQueue.at(i) << ":::" << i << std::endl;
                    requestTexture(TextureQueue.at(i));
                    //std::cout << "Loaded: " << i+1 << " out of " << TextureQueue.size() << std::endl << std::endl ;
                    TextureQueue.pop_back();
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }
        mShouldLoadQueuedTextures = false;
    }

    int ResourceManager::requestTextureUsage(const std::string &texPath)
    {
        try
        {
            std::map<std::string, std::shared_ptr<Renderer::Texture2D>>::iterator it;
            it = TextureList.find(texPath);
            if (it != TextureList.end())
            {
                return 0; //TextureList.at(it->first)->getTimesUsed();
            }
            else
            {
                std::cout << ":: ERROR :: The texture requested is not loaded in the system" << std::endl
                          << std::endl;
                return 0;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return 0;
    }

    void ResourceManager::resetTextureUsage(const std::string &texPath)
    {
        try
        {
            std::map<std::string, std::shared_ptr<Renderer::Texture2D>>::iterator it;
            it = TextureList.find(texPath);
            if (it != TextureList.end())
            {
                //TextureList.at(it->first)->resetRequestCount();
            }
            else
            {
                std::cout << ":: ERROR :: The texture requested is not loaded in the system" << std::endl
                          << std::endl;
                return;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }
    }

    void ResourceManager::destroyAllTextures()
    {
        for (std::map<std::string, std::shared_ptr<Renderer::Texture2D>>::iterator itr = TextureList.begin(); itr != TextureList.end(); itr++)
        {
            itr->second->Destroy();
        }
    }

    std::string ResourceManager::requestShader(const std::string &shaderPathv, const std::string &shaderPathf, const std::string &name)
    {
        try
        {
            std::map<std::string, std::shared_ptr<Shader>>::iterator it;
            it = ShadersList.find(name);
            if (it != ShadersList.end())
            {
                return ShadersList.at(it->first)->getPath();
            }
            else
            {
                std::shared_ptr<Shader> tmpShader = std::make_shared<Shader>(shaderPathv.c_str(), shaderPathf.c_str());
                ShadersList.insert(std::make_pair(name, tmpShader));
                return name;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return "";
    }
    std::string ResourceManager::requestShader(const std::string &shaderPathv, const std::string &shaderPathf, const std::string &shaderPathg, const std::string &name)
    {
        try
        {
            std::map<std::string, std::shared_ptr<Shader>>::iterator it;
            it = ShadersList.find(name);
            if (it != ShadersList.end())
            {
                return ShadersList.at(it->first)->getPath();
            }
            else
            {
                std::shared_ptr<Shader> tmpShader = std::make_shared<Shader>(shaderPathv.c_str(), shaderPathf.c_str(), shaderPathg.c_str());
                ShadersList.insert(std::make_pair(name, tmpShader));
                return name;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return "";
    }

    std::shared_ptr<Shader> ResourceManager::useShader(const std::string &shaderPath)
    {
        try
        {
            return ShadersList.at(shaderPath);
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return nullptr;
    }

    GLuint ResourceManager::getShaderID(const std::string &shaderPath)
    {
        try
        {
            return ShadersList.at(shaderPath)->getProgramID();
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return -1;
    }

    std::shared_ptr<Shader> ResourceManager::getShader(const std::string &shaderPath)
    {
        try
        {
            return ShadersList.at(shaderPath);
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return std::make_shared<Shader>("", "");
    }

    bool ResourceManager::requestCubeMap(int CubeMapID, glm::vec3 Position)
    {
        try
        {
            std::vector<std::string> paths;
            std::string path;
            switch (CubeMapID)
            {
            case 1:
                path = "materials/skyboxes/Miramar/";
                break;
            case 2:
                path = "materials/skyboxes/Miramar_Blurred/";
                break;
            default:
                path = "materials/skyboxes/Miramar/";
            }
            paths.push_back(path + "right.tga");
            paths.push_back(path + "left.tga");
            paths.push_back(path + "top.tga");
            paths.push_back(path + "bottom.tga");
            paths.push_back(path + "back.tga");
            paths.push_back(path + "front.tga");

            std::shared_ptr<CubeMap> tmpCubeMap = (std::shared_ptr<CubeMap>)new CubeMap(paths, CubeMapID, Position);
            std::cout << "Added Cubemap: " << CubeMapID << std::endl;
            try
            {
                CubeMapList.insert(std::make_pair(CubeMapID, tmpCubeMap));
                CubeMapPositions.push_back(Position);
                mCubemapIndex.push_back(CubeMapID);
            }
            catch (...)
            {
                std::cout << "Exception at requestCube" << std::endl;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return false;
    }

    bool ResourceManager::addCubemap(std::shared_ptr<CubeMap> cubemap, glm::vec3 position)
    {
        //if(cubemap != nullptr){
        CubeMapList.insert(std::make_pair(cubemap->getID(), cubemap));
        CubeMapPositions.push_back(position);
        mCubemapIndex.push_back(cubemap->getID());
        //std::cout << "Added Cubemap: " << cubemap->getID() << std::endl;
        return true;
        //}
        //else{
        //    return false;
        //}
    }

    int ResourceManager::NearestCubeMap(glm::vec3 TestingPoint)
    {
        return Helpers::findNearestPointFromSet(TestingPoint, CubeMapPositions);
    }

    GLuint ResourceManager::useCubeMap(int ID)
    {
        if (CubeMapList.at(ID) != nullptr)
            return CubeMapList.at(ID)->getTextureID();

        return 0;
    }

    template <>
    [[nodiscard]] std::shared_ptr<Renderer::Model> ResourceManager::Get(const std::string &name)
    {
        try
        {
            return ModelList.at(name);
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return nullptr;
    }

    template <>
    [[nodiscard]] std::shared_ptr<Renderer::Texture2D> ResourceManager::Get(const std::string &name)
    {
        try
        {
            if (name.empty() != true)
            {
                return TextureList.at(name);
            }
            else
                return 0;
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }
        return nullptr;
    }

} // namespace Epsilon