#pragma once
#include <CubeMap.h>
#include <memory>
#include <fstream>
#include <Includes.h>
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
    }

    ~SphericalHarmonics() {}

    void CalculateCohefficients(GLuint cubemap, const unsigned int order);


    std::vector<glm::vec3> getCohefficients() {
        return m_Cohefficients;
    }

    struct SphericalHarmonicsFormat {
        glm::vec3 position;
        glm::vec3 u_L00;
        glm::vec3 u_L1m1;
        glm::vec3 u_L10;
        glm::vec3 u_L11;
        glm::vec3 u_L2m2;
        glm::vec3 u_L2m1;
        glm::vec3 u_L20;
        glm::vec3 u_L21;
        glm::vec3 u_L22;
    };

    void SaveCohefficients() {
        std::ofstream FILE(std::string("ambient/" +
                                       Helpers::floatTostring(0.0) + "-" +
                                       Helpers::floatTostring(0.0) + "-" +
                                       Helpers::floatTostring(0.0) + "_"
                                       "test.shc").c_str(), std::ios::binary);

        SphericalHarmonicsFormat SHF;

        SHF.position = glm::vec3(0.0, 0.0, 0.0);
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

    std::vector<std::string> mCohefficientsNames;

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

