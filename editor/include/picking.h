#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <include/Types.h>

class RayPick{

public:
    float mouse_x, mouse_y, width, height, lenght, distance, lastdistance, deltadist;
    glm::vec3 mPos, mLastPos, deltapos, mScale, rDir, rLastDir, rDeltaDir;

public:

    RayPick(){}

    glm::vec3 getRayDir(){return rDir;}

    glm::vec3 GenRay(glm::vec3 pos, glm::mat4 view, glm::mat4 proj)
    {
        float x = (2.0f * mouse_x) / width - 1.0f;
        float y = 1.0f - (2.0f * mouse_y) / height;
        float z = 1.0f;
        glm::vec3 ray_nds = glm::vec3 (x, y, z);

        glm::vec4 ray_clip = glm::vec4 (ray_nds.x, ray_nds.y, -1.0, 1.0);

        glm::vec4 ray_eye = glm::inverse (proj) * ray_clip;
        ray_eye = glm::vec4 (ray_eye.x, ray_eye.y, -1.0, 0.0);

        glm::vec3 ray_wor = glm::vec3( glm::inverse(view) * ray_eye);
        // don't forget to normalise the vector at some point
        return glm::normalize (ray_wor);
    }

    bool intersect(const glm::vec3 origin, const glm::vec3 direction, const MIN_MAX_POINTS limits)
    {
        lastdistance = distance;
        float tmin = (((limits.MIN_X*mScale.x+mPos.x)) - origin.x) / direction.x;
        float tmax = (((limits.MAX_X*mScale.x+mPos.x)) - origin.x) / direction.x;

        if (tmin > tmax) swap(tmin, tmax);

        float tymin = (((limits.MIN_Y*mScale.y+mPos.y)) - origin.y) / direction.y;
        float tymax = (((limits.MAX_Y*mScale.y+mPos.y)) - origin.y) / direction.y;

        if (tymin > tymax) swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (((limits.MIN_Z*mScale.z+mPos.z)) - origin.z) / direction.z;
        float tzmax = (((limits.MAX_Z*mScale.z+mPos.z)) - origin.z) / direction.z;

        if (tzmin > tzmax) swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        distance = glm::length(origin - mPos);
        deltadist = distance - lastdistance;
        return true;
    }

    bool pickObject(float mx, float my, float w, float h, float l, glm::vec3 pos, glm::mat4 view, glm::mat4 proj, MIN_MAX_POINTS limits, glm::vec3 mposition, glm::vec3 mscale)
    {
        mouse_x = mx, mouse_y = my, width = w, height = h, lenght = l;
        mPos = pos;
        mLastPos = mPos;
        mPos = mposition;
        deltapos = mPos - mLastPos;
        mScale = mscale;
        rLastDir = rDir;
        rDir = GenRay(pos, view, proj);
        rDeltaDir = rDir - rLastDir;
        return intersect(pos, rDir, limits);
    }
};
