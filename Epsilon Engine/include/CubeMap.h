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

        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
        m_CubemapSides.push_back(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);

    }

    virtual ~CubeMap(){
        /*std::cout << "Deleted CubeMap" << std::endl;*/}



    virtual glm::vec3 getPosition()
    {
        return this->Position;
    }

    virtual GLuint getTextureID()
    {
        return texture->getTextureID();
    }

    virtual GLuint getCubemapFace(int index)
    {
        return m_CubemapSides.at(index);
    }


private:
        int ID = 0;
        glm::vec3 Position;
        std::shared_ptr<eTexture> texture;
        std::vector<glm::vec3> m_CubeMapsDirections;
        std::vector<GLuint> m_CubemapSides;

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
