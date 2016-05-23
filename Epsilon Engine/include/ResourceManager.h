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

using namespace std;


class ResourceManager
{
public:

    ResourceManager() {
        std::cout << "Created Resource Manager" << std::endl;}

    virtual ~ResourceManager() {
        std::cout << "Deleted Resource Manager" << std::endl;
        for(std::map<std::string, eTexture>::iterator itr = TextureList.begin(); itr != TextureList.end(); itr++)
        {
            GLuint tex = itr->second.getTextureID();
            glDeleteTextures(1, &tex);
        }
/*
        for(std::map<std::string, Model>::iterator itr = ModelList.begin(); itr != ModelList.end(); itr++)
        {
            itr->second = nullptr;
        }*/
    }

    std::string requestTexture(std::string texPath)
    {
        try
        {
            std::map<std::string, eTexture>::iterator it;
            std::cout << "Loading texture: " << texPath << " ... " << std::endl;
            it = TextureList.find(texPath);
            if(it != TextureList.end())
            {
                std::cout << "Texture already loaded, assigning existing one to object" << std::endl << std::endl;
                return TextureList.at(it->first).getPath();
            }
            else
            {
                std::cout << "Texture not loaded, reading it from hard drive" << std::endl << std::endl;
                eTexture tmpTex(texPath.c_str());
                TextureList.insert(std::make_pair(texPath, tmpTex));
                return texPath;
            }
        }
        catch(...)
        {
            throw;
        }
    }

    std::string requestModel(std::string modelPath, std::shared_ptr<ResourceManager> rm, glm::vec3 Pos, glm::vec3 scs, glm::quat rot)
    {
        try
        {
            std::map<std::string, Model>::iterator it;
            it = ModelList.find(modelPath);
            std::cout << "Loading model: " << modelPath << " ... " << std::endl;
            if(it != ModelList.end())
            {
                std::cout << "Model already loaded, assigning existing one to object" << std::endl << std::endl;
                return ModelList.at(it->first).getPath();
            }
            else
            {
                std::cout << "Model not loaded, reading it from hard drive" << std::endl << std::endl;
                Model tmpModel(modelPath.c_str(), rm, Pos, scs, rot);
                ModelList.insert(std::make_pair(modelPath, tmpModel));
                return modelPath;
            }
        }
        catch(...)
        {
            throw;
        }
    }

    void useModel(std::string modelPath, Shader* shader)
    {
        try
        {
            ModelList.at(modelPath).Draw(shader);
        }

        catch(...)
        {
            throw;
        }
    }

    void useModel(std::string modelPath, GLuint shader)
    {
        try
        {
            ModelList.at(modelPath).Draw(shader);
        }

        catch(...)
        {
            throw;
        }
    }

    MIN_MAX_POINTS getModelBoundingBox(std::string modelPath)
    {
        try
        {
            return ModelList.at(modelPath).MinMaxPoints;
        }

        catch(...)
        {
            throw;
        }
    }

    void setModelVisibility(std::string path ,bool visibility)
    {
        ModelList.at(path).m_IsVisible = visibility;
    }

    glm::quat getModelRotation(std::string path)
    {
        return ModelList.at(path).Rotation;
    }

    glm::vec3 getModelPosition(std::string path)
    {
        return ModelList.at(path).Position;
    }

    glm::vec3 getModelScale(std::string path)
    {
        return ModelList.at(path).Scale;
    }

    GLuint useTexture(std::string texPath)
    {
        try
        {   if(texPath.empty() != true)
                return TextureList.at(texPath).getTextureID();
            else
                return 0;
        }

        catch(...)
        {
            throw;
        }
    }

    void addTextureToQueue(std::string texture)
    {
        try{
        TextureQueue.push_back(texture);
        std::cout << texture << " Added to the Queue." << std::endl;
        }
        catch(...){throw;}
    }

    void loadQueuedTextures()
    {
        for(int i = 0 ; i < TextureQueue.size() ; ++i)
        {
            requestTexture(TextureQueue.at(i));
            TextureQueue.at(i).pop_back();
        }
    }

    std::string requestShader(std::string shaderPathv, std::string shaderPathf, std::string name)
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
        catch(...)
        {
            throw;
        }
    }

    Shader useShader(std::string shaderPath)
    {
        try
        {
            return ShadersList.at(shaderPath);
        }

        catch(...)
        {
            throw;
        }
    }


    GLuint getShaderID(std::string shaderPath)
    {
        try
        {
            return ShadersList.at(shaderPath).getProgramID();
        }

        catch(...)
        {
            throw;
        }
    }

    bool requestCubeMap(int CubeMapID, glm::vec3 Position)
    {
        try
        {
                std::vector<std::string> paths;
                std::string path;
            switch(CubeMapID){
                case 1:
                    path = "materials/skyboxes/Miramar/";
                    break;
                case 2:
                    path = "materials/skyboxes/cubemap/";
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

                CubeMap tmpCubeMap(paths, CubeMapList.size() + 1, Position);

                CubeMapList.insert(std::make_pair(CubeMapList.size() + 1, tmpCubeMap));
                CubeMapPositions.push_back(Position);
        }
        catch(...)
        {
            throw;
        }
    }

    int NearestCubeMap(glm::vec3 TestingPoint)
    {
        return Helpers::findNearestPointFromSet(TestingPoint, CubeMapPositions);
    }

    GLuint useCubeMap(int ID)
    {
        return CubeMapList.at(ID).getTextureID();
    }

    std::shared_ptr<Camera> m_Camera;
    std::shared_ptr<Physics::Physics> m_PhysicsWorld;

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
