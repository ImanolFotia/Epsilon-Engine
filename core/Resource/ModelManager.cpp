#include "ResourceManager.h"

namespace Epsilon
{

    const std::string &ResourceManager::requestModel(const std::string &modelPath, bool loadAsync)
    {
        try
        {
            bool should_create = false;
            /*std::map<std::string, std::shared_ptr<Renderer::Model>>::iterator it;
            it = ModelList.find(modelPath);
            if (it != ModelList.end())
            {
                return ModelList.at(it->first)->getPath();
            }*/
            if (ModelList.contains(modelPath))
            {
                if (ModelList.at(modelPath) != nullptr)
                {
                    if (!ModelList.at(modelPath)->isResident())
                    {
                        should_create = true;
                    }
                }
                else
                {
                    should_create = true;
                }
            }
            std::map<std::string, std::shared_ptr<Renderer::Model>>::iterator it;
            it = ModelList.find(modelPath);
            if (!should_create)
            {
                return modelPath; //TextureList.at(it->first)->getPath();
            }
            else
            {
                //std::shared_ptr<Model> tmpModel(modelPath.c_str());
                ModelList.insert(std::make_pair(modelPath, std::make_shared<Renderer::Model>(modelPath.c_str())));
                if (loadAsync)
                {
                    ModelQueueFuture[modelPath] = LoadModelAsync(modelPath);
                }
                else
                {
                    LoadModel(modelPath);
                    ModelList.at(modelPath)->toGPU();
                }

                return modelPath;
            }
        }
        catch (std::runtime_error &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << "\nMessage: " << e.what() << std::endl;
            std::cout << "Failed to load: " << modelPath.c_str() << std::endl;
        }
        return modelPath;
    }

    void ResourceManager::deleteModel(const std::string &path)
    {
        if (ModelList.contains(path))
        {
            ModelList.at(path)->Destroy();
        }
    }

    [[nodiscard]] std::shared_ptr<Renderer::Model> ResourceManager::getModel(const std::string &modelPath)
    {
        try
        {
            if (ModelList.contains(modelPath))
                return ModelList.at(modelPath);
            return nullptr;
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
            if (ModelList.contains(modelPath))
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
        if (ModelList.contains(path))
            ModelList.at(path)->m_IsVisible = visibility;
    }

    //void ResourceManager::setModelUniforms(const std::string& path, std::shared_ptr<Shader> shader, glm::vec3 pos, glm::vec3 sc, glm::quat rot, std::shared_ptr<Camera> cam)
    //{
    //ModelList.at(path).SetUniforms(shader, pos, sc, rot, cam);
    //}

    void ResourceManager::setModelUniforms(const std::string &path, std::shared_ptr<Shader> shader, glm::vec3 pos, glm::vec3 sc, glm::quat rot, glm::vec3 ppos, glm::vec3 psc, glm::quat prot, std::shared_ptr<Camera> cam)
    {
        if (ModelList.contains(path))
            ModelList.at(path)->SetUniforms(shader, pos, sc, rot, ppos, psc, prot, cam);
    }

    void ResourceManager::destroyAllModels()
    {
        for (std::map<std::string, std::shared_ptr<Renderer::Model>>::iterator itr = ModelList.begin(); itr != ModelList.end(); itr++)
        {
            itr->second->Destroy();
        }
    }

    void ResourceManager::addModelToQueue(const std::string &modelPath)
    {
        try
        {
            auto tmpModel = std::make_shared<Renderer::Model>();
            if (!ModelList.contains(modelPath))
                ModelList.insert(std::make_pair(modelPath, tmpModel));
            ModelQueue.push_back(modelPath);
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }
    }

    std::future<void> ResourceManager::LoadModelAsync(std::string model_id)
    {
        return std::async(std::launch::async, &ResourceManager::LoadModel, this, model_id);
    }

    void ResourceManager::loadQueuedModels()
    {
        int index = 0;
        std::mutex ModelQueue_mutex;
        std::mutex ModelQueueFuture_mutex;

        try
        {
            if (ModelQueue.size() > 0)
            {
                for (index = 0; index < static_cast<int>(ModelQueue.size()); ++index)
                {
                    //std::cout << "Loading queued model: " << ModelQueue.at(index) << ":::" << index << std::endl;
                    requestModel(ModelQueue.at(index));
                    //std::cout << "Loaded: " << index + 1 << " out of " << ModelQueue.size() << std::endl
                    //          << std::endl;
                    std::lock_guard<std::mutex> guard(ModelQueue_mutex);
                    ModelQueue.erase(ModelQueue.begin() + index);
                }
            }

            if (ModelQueueFuture.size() > 0)
            {
                auto key = ModelQueueFuture.begin()->first;
                auto &future = ModelQueueFuture.begin()->second;
                {
                    auto status = future.wait_for(std::chrono::milliseconds(0));

                    if (status == std::future_status::timeout)
                    {
                    }
                    else if (status == std::future_status::ready && future.valid())
                    {
                        //auto data = future.get();
                        //std::cout << "Loading future model: " << key << ":::" << index << std::endl;
                        auto &tmpModel = ModelList.at(key);
                        if (tmpModel->isLoaded)
                        {
                            tmpModel->toGPU();
                        }
                        //std::cout << "llega" << std::endl;
                        auto it = ModelQueueFuture.find(key);

                        std::lock_guard<std::mutex> guard(ModelQueueFuture_mutex);
                        if (it != ModelQueueFuture.end())
                            ModelQueueFuture.erase(it);
                    }
                }
            }
        }
        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << " at index " << index << std::endl;
        }
        mShouldLoadQueuedModels = false;
    }

    template <>
    [[nodiscard]] std::shared_ptr<Renderer::Model> ResourceManager::Get(const std::string &name)
    {
        try
        {
            if (ModelList.contains(name))
                return ModelList.at(name);
        }

        catch (std::exception &e)
        {
            std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() << std::endl;
        }

        return nullptr;
    }

}