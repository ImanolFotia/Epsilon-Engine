#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <Texture.h>
#include <Model.h>
#include <Water.h>
#include <Terrain.h>
#include <Grass.h>
#include <map>

using namespace std;

namespace Epsilon{
class ResourceManager
{
    public:

        std::string loadTexture(std::string texPath)
        {
            std::map<std::string, eTexture*>::iterator it;
            it = TextureList.find(texPath);
            if(it != TextureList.end())
            {
                return TextureList.at(it->first)->getPath();
            }
            else
            {
                eTexture* tmpTex = new eTexture(texPath.c_str());
                TextureList[texPath] = tmpTex;
                return texPath;
            }
        }

        GLuint getTextureID(std::string texPath)
        {
            return TextureList[texPath]->getTextureID();
        }

        std::string loadModel(std::string);

        std::string loadWater();
        std::string loadTerrain();

    private:

        ResourceManager(){}
        virtual ~ResourceManager(){}

        std::map<std::string, eTexture*>  TextureList;
        std::map<std::string, Model*>    ModelList;
        std::map<std::string, Water*>    WaterPlanesList;
        std::map<std::string, Terrain*>  TerrainList;


};
}
#endif // RESOURCEMANAGER_H
