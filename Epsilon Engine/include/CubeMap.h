#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

#include <Texture.h>

class CubeMap
{
public:
    CubeMap(int ID){}

    CubeMap(std::vector<std::string> paths, int ID, glm::vec3 Pos)
    {
        texture = std::move((shared_ptr<eTexture>)(new eTexture(paths)));
        std::cout << "CubeMapID" << texture->getTextureID() << std::endl;
        Position = Pos;
        this->ID = ID;
    }

    virtual ~CubeMap(){}

    virtual glm::vec3 getPosition()
    {
        return this->Position;
    }

    virtual GLuint getTextureID()
    {
        return texture->getTextureID();
    }

private:
        int ID = 0;
        glm::vec3 Position;
        std::shared_ptr<eTexture> texture;

protected:
};

class PCCB : public CubeMap
{

public:

    PCCB();

    ~PCCB(){}

    std::vector<glm::vec3> getBoxMinMaxPoints()
    {
        return BoxMinMaxPoints;
    }


    std::vector<glm::vec3> BoxMinMaxPoints;
};
