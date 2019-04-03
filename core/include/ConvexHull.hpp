#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <Model.h>

class Model;
class Mesh;

static std::vector<glm::vec3> toConvexHull(const std::vector<glm::vec3> verts)
{
    //First calculate min/max points (bounding box)
    glm::vec3 test = glm::vec3(0.0f);

    struct BOUNDING_BOX
    {
        glm::vec3 maxX = glm::vec3(0.0f);
        glm::vec3 minX = glm::vec3(0.0f);
        glm::vec3 maxY = glm::vec3(0.0f);
        glm::vec3 minY = glm::vec3(0.0f);
        glm::vec3 maxZ = glm::vec3(0.0f);
        glm::vec3 minZ = glm::vec3(0.0f);
    };

    BOUNDING_BOX bb;
    for(int i = 0; i < verts.size(); i++)
    {
        if(bb.maxX.x < verts[i].x)
            bb.maxX = verts[i];
        if(bb.minX.x > verts[i].x)
            bb.minX = verts[i];

        if(bb.maxY.y < verts[i].y)
            bb.maxY = verts[i];
        if(bb.minY.y > verts[i].y)
            bb.minY = verts[i];

        if(bb.maxZ.z < verts[i].z)
            bb.maxZ = verts[i];
        if(bb.minZ.z > verts[i].z)
            bb.minZ = verts[i];
    }
} 

