#pragma once
#include <pch.hpp>
#include <CubeMap.h>
#include <Helpers.hpp>

namespace Epsilon
{
class SphericalHarmonics {

public:
    SphericalHarmonics() {
        mCohefficientsNames.push_back("u_L00");
        mCohefficientsNames.push_back("u_L1m1");
        mCohefficientsNames.push_back("u_L10");
        mCohefficientsNames.push_back("u_L11");
        mCohefficientsNames.push_back("u_L2m2");
        mCohefficientsNames.push_back("u_L2m1");
        mCohefficientsNames.push_back("u_L20");
        mCohefficientsNames.push_back("u_L21");
        mCohefficientsNames.push_back("u_L22");

        m_Cohefficients.resize(9);
        for(unsigned int i=0; i < 3; i++) {
            m_Cohefficients[i].x = 0.5;
            m_Cohefficients[i].y = 0.5;
            m_Cohefficients[i].z = 0.5;
        }
    
    }

    ~SphericalHarmonics() {}

    void CalculateCohefficients(GLuint cubemap, const unsigned int order);


    std::vector<glm::vec3> getCohefficients() {
        return m_Cohefficients;
    }

    struct SphericalHarmonicsFormat {
        glm::vec3 u_L00; float padding0;
        glm::vec3 u_L1m1; float padding1;
        glm::vec3 u_L10; float padding2;
        glm::vec3 u_L11; float padding3;
        glm::vec3 u_L2m2; float padding4;
        glm::vec3 u_L2m1; float padding5;
        glm::vec3 u_L20; float padding6;
        glm::vec3 u_L21; float padding7;
        glm::vec3 u_L22; float padding8;
    };

    void SaveCohefficients() {
        std::ofstream FILE(std::string("ambient/" +
                                       Helpers::floatTostring(0.0) + "-" +
                                       Helpers::floatTostring(0.0) + "-" +
                                       Helpers::floatTostring(0.0) + "_"
                                       "test.shc").c_str(), std::ios::binary);

        SphericalHarmonicsFormat SHF;

        SHF.u_L00 = m_Cohefficients[0];
        SHF.u_L1m1 = m_Cohefficients[1];
        SHF.u_L10 = m_Cohefficients[2];
        SHF.u_L11 = m_Cohefficients[3];
        SHF.u_L2m2 = m_Cohefficients[4];
        SHF.u_L2m1 = m_Cohefficients[5];
        SHF.u_L20 = m_Cohefficients[6];
        SHF.u_L21 = m_Cohefficients[7];
        SHF.u_L22 = m_Cohefficients[8];

        FILE.write((char*)&SHF, sizeof(SphericalHarmonicsFormat));

    }

    SphericalHarmonicsFormat toStruct()
    {
        SphericalHarmonicsFormat SHF;

        SHF.u_L00 = m_Cohefficients[0];
        SHF.u_L1m1 = m_Cohefficients[1];
        SHF.u_L10 = m_Cohefficients[2];
        SHF.u_L11 = m_Cohefficients[3];
        SHF.u_L2m2 = m_Cohefficients[4];
        SHF.u_L2m1 = m_Cohefficients[5];
        SHF.u_L20 = m_Cohefficients[6];
        SHF.u_L21 = m_Cohefficients[7];
        SHF.u_L22 = m_Cohefficients[8];

        return SHF;
    }

    std::vector<std::string> mCohefficientsNames;

    void setId(unsigned inId) { this->m_ID = inId;}

private:
    std::vector<glm::vec3> m_Cohefficients;
    float m_scaleFactor;
    glm::vec3 m_Position;
    unsigned m_ID;


    void sphericalHarmonicsEvaluateDirection(float * result, int order,
            const glm::vec3 & dir, std::string why);

    void sphericalHarmonicsAdd(float * result, int order,
                               const float * inputA, const float * inputB, std::string why);

    void sphericalHarmonicsScale(float * result, int order,
                                 const float * input, float scale, std::string why);


};

}