#include "ResourceManager.h"

std::string ResourceManager::requestTexture(std::string texPath)
{
    try
    {
        std::map<std::string, std::shared_ptr<eTexture> >::iterator it;
        it = TextureList.find(texPath);
        if(it != TextureList.end())
        {
            return TextureList.at(it->first)->getPath();
        }
        else
        {
            std::shared_ptr<eTexture> tmpTex = (std::shared_ptr<eTexture>) new eTexture(texPath.c_str());
            TextureList.insert(std::make_pair(texPath, tmpTex));
            return texPath;
        }
    }
    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

std::string ResourceManager::requestModel(std::string modelPath, std::shared_ptr<ResourceManager> rm, glm::vec3 Pos, glm::vec3 scs, glm::quat rot)
{
    try
    {
        std::map<std::string, Model>::iterator it;
        it = ModelList.find(modelPath);
        if(it != ModelList.end())
        {
            return ModelList.at(it->first).getPath();
        }
        else
        {
            Model tmpModel(modelPath.c_str(), rm, Pos, scs, rot);
            ModelList.insert(std::make_pair(modelPath, tmpModel));
            return modelPath;
        }
    }
    catch(std::runtime_error& e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << "\nMessage: " << e.what() <<std::endl;
        std::cout << "Failed to load: " << modelPath.c_str() << std::endl;
    }
}

void ResourceManager::useModel(std::string modelPath, Shader* shader, glm::vec3 pos = glm::vec3(0,0,0))
{
    try
    {
        ModelList.at(modelPath).Draw(shader, pos);
    }

    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << "useModel(std::string modelPath, Shader* shader, glm::vec3 pos = glm::vec3(0,0,0)) :::" << e.what() <<std::endl;
    }
}

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
}

Model ResourceManager::getModel(std::string modelPath)
{
    try
    {
        return ModelList.at(modelPath);
    }

    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

MIN_MAX_POINTS ResourceManager::getModelBoundingBox(std::string modelPath)
{
    try
    {
        return ModelList.at(modelPath).MinMaxPoints;
    }

    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

void ResourceManager::setModelVisibility(std::string path ,bool visibility)
{
    ModelList.at(path).m_IsVisible = visibility;
}

glm::quat ResourceManager::getModelRotation(std::string path)
{
    return ModelList.at(path).Rotation;
}

glm::vec3 ResourceManager::getModelPosition(std::string path)
{
    return ModelList.at(path).Position;
}

glm::vec3 ResourceManager::getModelScale(std::string path)
{
    return ModelList.at(path).Scale;
}

void ResourceManager::setModelUniforms(std::string path, Shader* shader, glm::vec3 pos, glm::vec3 sc, glm::quat rot, std::shared_ptr<Camera> cam)
{
    this->ModelList.at(path).SetUniforms(shader, pos, sc, rot, cam);
}

void ResourceManager::setModelUniforms(std::string path, Shader* shader, glm::vec3 pos, glm::vec3 sc, glm::quat rot, glm::vec3 ppos, glm::vec3 psc, glm::quat prot, std::shared_ptr<Camera> cam)
{
    this->ModelList.at(path).SetUniforms(shader, pos, sc, rot, ppos, psc, prot, cam);
}


void ResourceManager::destroyAllModels()
{
    for(std::map<std::string, Model>::iterator itr = ModelList.begin(); itr != ModelList.end(); itr++)
        {
            itr->second.Destroy();
        }
}

GLuint ResourceManager::useTexture(std::string texPath)
{
    try
    {
        if(texPath.empty() != true){
            return TextureList.at(texPath)->getTextureID();
        }
        else
            return 0;
    }

    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

void ResourceManager::addTextureToQueue(std::string texture)
{
    try
    {
        TextureQueue.push_back(texture);
        //std::cout << texture << " Added to the Queue." << std::endl;
    }
    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

void ResourceManager::loadQueuedTextures()
{
    try {
        #pragma omp
        for(unsigned int i = 0 ; i < TextureQueue.size() ; ++i)
        {
            requestTexture(TextureQueue.at(i));
            //std::cout << "Loaded: " << i+1 << " out of " << TextureQueue.size() << std::endl << std::endl ;
            TextureQueue.at(i).pop_back();
        }
    }
    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}


int ResourceManager::requestTextureUsage(std::string texPath)
{
    try
    {
        std::map<std::string, std::shared_ptr<eTexture> >::iterator it;
        it = TextureList.find(texPath);
        if(it != TextureList.end())
        {
            return TextureList.at(it->first)->getTimesUsed();
        }
        else
        {
            std::cout << ":: ERROR :: The texture requested is not loaded in the system" << std::endl << std::endl;
            return 0;
        }
    }
    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

void ResourceManager::resetTextureUsage(std::string texPath)
{
    try
    {
        std::map<std::string, std::shared_ptr<eTexture> >::iterator it;
        it = TextureList.find(texPath);
        if(it != TextureList.end())
        {
            TextureList.at(it->first)->resetRequestCount();
        }
        else
        {
            std::cout << ":: ERROR :: The texture requested is not loaded in the system" << std::endl << std::endl;
            return;
        }
    }
    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

void ResourceManager::destroyAllTextures()
{
    for(std::map<std::string, std::shared_ptr<eTexture> >::iterator itr = TextureList.begin(); itr != TextureList.end(); itr++)
        {
            itr->second->Destroy();
        }
}

std::string ResourceManager::requestShader(std::string shaderPathv, std::string shaderPathf, std::string name)
{
    try
    {
        std::map<std::string, Shader>::iterator it;
        it = ShadersList.find(name);
        if(it != ShadersList.end())
        {
            return ShadersList.at(it->first).getPath();
        }
        else
        {
            Shader tmpShader(shaderPathv.c_str(), shaderPathf.c_str());
            ShadersList.insert(std::make_pair(name, tmpShader));
            return name;
        }
    }
    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

Shader ResourceManager::useShader(std::string shaderPath)
{
    try
    {
        return ShadersList.at(shaderPath);
    }

    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

GLuint ResourceManager::getShaderID(std::string shaderPath)
{
    try
    {
        return ShadersList.at(shaderPath).getProgramID();
    }

    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

bool ResourceManager::requestCubeMap(int CubeMapID, glm::vec3 Position)
{
    try
    {
        std::vector<std::string> paths;
        std::string path;
        switch(CubeMapID)
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

        std::shared_ptr<CubeMap> tmpCubeMap = (std::shared_ptr<CubeMap>) new CubeMap(paths, CubeMapID, Position);
        std::cout << "Added Cubemap: " << CubeMapID << std::endl;
        CubeMapList.insert(std::make_pair(CubeMapID, tmpCubeMap));
        CubeMapPositions.push_back(Position);
        mCubemapIndex.push_back(CubeMapID);
    }
    catch(std::exception e)
    {
        std::cout << "Exception caught at: " << __FUNCTION__ << ":::" << e.what() <<std::endl;
    }
}

bool ResourceManager::addCubemap(std::shared_ptr<CubeMap> cubemap, glm::vec3 position)
{
    if(cubemap != nullptr){
        CubeMapList.insert(std::make_pair(cubemap->getID(), cubemap));
        CubeMapPositions.push_back(position);
        mCubemapIndex.push_back(cubemap->getID());
        std::cout << "Added Cubemap: " << cubemap->getID() << std::endl;
        return true;
    }
    else{
        return false;
    }

}

int ResourceManager::NearestCubeMap(glm::vec3 TestingPoint)
{
    return Helpers::findNearestPointFromSet(TestingPoint, CubeMapPositions);
}

GLuint ResourceManager::useCubeMap(int ID)
{
    return CubeMapList.at(ID)->getTextureID();
}
