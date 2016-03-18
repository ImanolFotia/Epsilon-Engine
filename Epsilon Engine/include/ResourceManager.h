#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <Texture.h>
#include <Model.h>
#include <Water.h>
#include <Terrain.h>
#include <Grass.h>
#include <map>
#include <Shader.h>

using namespace std;


class ResourceManager
{
public:

    ResourceManager() {}

    virtual ~ResourceManager() {}

    std::string requestTexture(std::string texPath)
    {
        try
        {
            std::map<std::string, eTexture>::iterator it;
            it = TextureList.find(texPath);
            if(it != TextureList.end())
            {
                return TextureList.at(it->first).getPath();
            }
            else
            {
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

    std::string requestModel(std::string modelPath)
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
                Model tmpModel(modelPath.c_str());
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

    std::string loadWater();
    std::string loadTerrain();

private:


    std::map<std::string, eTexture>  TextureList;
    std::map<std::string, Model>    ModelList;
    std::map<std::string, Water>    WaterPlanesList;
    std::map<std::string, Terrain>  TerrainList;

    unsigned int modelCounter;
    unsigned int textureCounter;


};

#endif // RESOURCEMANAGER_H
