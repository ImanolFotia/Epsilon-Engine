#include "ResourceManager.h"
#include "ProgramData.h"

namespace Epsilon
{
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


} // namespace Epsilon