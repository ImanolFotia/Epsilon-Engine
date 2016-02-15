#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <Texture.h>
#include <Model.h>
#include <Water.h>
#include <Terrain.h>
#include <Grass.h>
//#include <Audio.h>

namespace ResourceManager{
class ResourceManager
{
    public:
        ResourceManager();
        virtual ~ResourceManager();

    protected:

    private:

        typedef map<std::string, Texture*>  TextureList;
        typedef map<std::string, Model*>    ModelList;
        typedef map<std::string, Water*>    WaterPlanesList;
        typedef map<std::string, Terrain*>  TerrainList;


};
}
#endif // RESOURCEMANAGER_H
