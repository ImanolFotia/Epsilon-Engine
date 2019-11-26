#pragma once
#include <CubeMap.h>

class LightProbe{
public:

    LightProbe();

    ~LightProbe(){}

    void CaptureLight();

    void FilterProve();

private:

    glm::vec3 m_Position;

    std::vector<glm::vec3> m_Directions;

    CubeMap m_CubeMap;

    int m_Width, m_Height;

    float m_FOV;
    float m_NearPlane, m_FarPlane;

};
