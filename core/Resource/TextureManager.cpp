#include "ResourceManager.h"

namespace Epsilon
{

    uint32_t ResourceManager::getNearestCubemapIndex(glm::vec3 pos)
    {
        return mCubemapIndex.at(NearestCubeMap(pos));
    }

    uint32_t ResourceManager::getNumCubemaps()
    {
        return mCubemapIndex.size();
    }

    uint32_t ResourceManager::getNumTextures()
    {
        return TextureList.size();
    }

    const ResourceManager::TextureArray &ResourceManager::getTextureArray()
    {
        return TextureList;
    }

    ResourceManager::Texture2D_ptr ResourceManager::getTexture2D(const std::string &name)
    {
        return TextureList.at(name);
    }

    unsigned long ResourceManager::getTextureBytesAllocation()
    {
        return mTextureMemoryAllocated;
    }

    bool ResourceManager::ShouldLoadQueuedTextures()
    {
        return mShouldLoadQueuedTextures;
    }

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

                //ProgramData DATA;
                using Renderer::Texture2D;

                auto is_normal = Helpers::isNormal(texPath.c_str());

                int outwidth = 0, outheight = 0, outchannels = 0;
                auto path = std::string("./materials/") + std::string(texPath);
                getTextureInfo(path, &outwidth, &outheight, &outchannels);
                //auto data = stbi_load(path, &outwidth, &outheight, &outchannels, 4);
                int lod_zero_size = outwidth * outheight * sizeof(unsigned char) * outchannels;

                mTextureMemoryAllocated += (unsigned long)((float)lod_zero_size * 1.3);

                API::Texture::Texture::TextureData TextureData;
                TextureData.MakeDefaultGL();
                TextureData.Compressed = !is_normal;
                TextureData.SRGB = !is_normal;
                TextureData.Width = outwidth;
                TextureData.Height = outheight;
                TextureData.AnisotropyLevel = 16;

                auto &tmpTex = TextureList.at(texPath);
                tmpTex->Create(TextureData);

                //tmpTex->setData(data, 0);
                //stbi_image_free(data);
                //SOIL_free_image_data(data);
                //std::cout << "Requesting texture from thread: " << std::this_thread::get_id() << std::endl;
                TextureQueueFuture[texPath] = LoadTextureAsync(path);
                return texPath;
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return "";
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

        //std::cout << "trying to bind: " << texPath << std::endl;
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

            //std::cout << texture << " added to queue." << std::endl;
            //std::cout << texture << " Added to the Queue." << std::endl;
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }
    }

    void ResourceManager::getTextureInfo(const std::string &path, int *w, int *h, int *c)
    {
        stbi_info(path.c_str(), w, h, c);
    }

    uint8_t *ResourceManager::LoadTexture(std::string path)
    {

        int attemps = 0;
        uint8_t *data = nullptr;
        //while (attemps < 15)
        {
            int w, h, c;
            auto *f = fopen64(path.c_str(), "rb");
            if (f == nullptr)
            {
                std::puts(path.c_str());
                //std::cerr << std::strerror(errno) << std::endl;
                //std::cout << GetLastError() << std::endl;
            }
            if (f != nullptr)
                data = stbi_load_from_file(f, &w, &h, &c, 4);
            fclose(f);

            //if (data != nullptr)
            //    break;
            attemps++;
        }

        if (data == nullptr)
        {
            //std::cout << "texture could not be loaded" << std::endl;
            //std::cout << stbi_failure_reason() << std::endl;
        }

        //std::cout << "Texture Loaded Async from thread: " << std::this_thread::get_id() << std::endl;
        return data;
    }

    //std::mutex m;
    std::future<uint8_t *> ResourceManager::LoadTextureAsync(std::string path)
    {
        return std::async(std::launch::async, &ResourceManager::LoadTexture, this, path);
    }

    void ResourceManager::loadQueuedTextures()
    {
        int index = 0;
        std::string texName;
        try
        {
            if (TextureQueue.size() > 0)
            {
                for (index = 0; index < static_cast<int>(TextureQueue.size()); ++index)
                {
                    //std::cout << "Loading queued texture: " << TextureQueue.at(index) << ":::" << index << std::endl;
                    requestTexture(TextureQueue.at(index));
                    //std::cout << "Loaded: " << i+1 << " out of " << TextureQueue.size() << std::endl << std::endl ;
                    TextureQueue.erase(TextureQueue.begin() + index);
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << " at index " << index << std::endl;
        }
        try
        {
            if (TextureQueueFuture.size() > 0)
            {
                auto key = TextureQueueFuture.begin()->first;
                auto &future = TextureQueueFuture.begin()->second;
                {
                    texName = key;
                    auto status = future.wait_for(std::chrono::milliseconds(0));

                    if (status == std::future_status::timeout)
                    {
                    }
                    else if (status == std::future_status::ready && future.valid())
                    {
                        auto data = future.get();

                        if (data != nullptr)
                        {

                            auto &tmpTex = TextureList.at(key);
                            tmpTex->setData(data, 0);
                            auto it = TextureQueueFuture.find(key);

                            if (it != TextureQueueFuture.end())
                                TextureQueueFuture.erase(it);

                            stbi_image_free(data);
                        }
                        else
                        {
                            //std::cout << "future for " << key << " is null" << std::endl;
                            auto it = TextureQueueFuture.find(key);

                            if (it != TextureQueueFuture.end())
                                TextureQueueFuture.erase(it);
                        }
                    }
                    else if (status == std::future_status::deferred)
                    {

                        //std::cout << "texture" << key << " is deferred" << std::endl;

                        auto data = future.get();

                        if (data != nullptr)
                        {

                            auto &tmpTex = TextureList.at(key);
                            tmpTex->setData(data, 0);
                            auto it = TextureQueueFuture.find(key);

                            if (it != TextureQueueFuture.end())
                                TextureQueueFuture.erase(it);

                            stbi_image_free(data);
                        }
                        else
                        {
                            //std::cout << "texture " << key << " could not be loaded" << std::endl;
                            auto it = TextureQueueFuture.find(key);

                            if (it != TextureQueueFuture.end())
                                TextureQueueFuture.erase(it);
                        }
                    }
                    else
                    {
                    }
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << " at index " << index << " ::: Texture name: " << texName << std::endl;
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
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << "name: " << name << std::endl;
        }
        return nullptr;
    }

    void ResourceManager::updateCubemap(uint32_t id, std::shared_ptr<CubeMap> cubemap)
    {
        CubeMapList.at(id) = cubemap;
    }
}
