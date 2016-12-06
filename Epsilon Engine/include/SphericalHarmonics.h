#pragma once
#include <CubeMap.h>
#include <memory>
class SphericalHarmonics{

public:
    SphericalHarmonics(){}

    ~SphericalHarmonics(){}

    void CalculateCohefficients(CubeMap cubemap, const unsigned int order);


    std::vector<glm::vec3> getCohefficients()
    {
        return m_Cohefficients;
    }


private:
    std::vector<glm::vec3> m_Cohefficients;
    float m_scaleFactor;
    glm::vec3 m_Position;


    void sphericalHarmonicsEvaluateDirection(float * result, int order,
                              const glm::vec3 & dir);

    void sphericalHarmonicsAdd(float * result, int order,
               const float * inputA, const float * inputB);

    void sphericalHarmonicsScale(float * result, int order,
               const float * input, float scale);


};

