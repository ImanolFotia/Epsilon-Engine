
#include <pch.hpp>
#include <SphericalHarmonics.h>
#include <Types.h>
#include <Driver/API/OpenGL/HelperFunctions/CheckError.h>



namespace Epsilon {
void SphericalHarmonics::CalculateCohefficients(GLuint cubemap, const unsigned int order) {
    const unsigned int sqOrder = order*order;

    // allocate memory for calculations
    m_Cohefficients.resize(sqOrder);
    std::vector<float> resultR(sqOrder);
    std::vector<float> resultG(sqOrder);
    std::vector<float> resultB(sqOrder);

    // variables that describe current face of cube texture
    float* data;
    GLint width, height;
    GLint internalFormat;
    GLint numComponents;

    // initialize values
    float fWt = 0.0f;
    for(unsigned int i=0; i < sqOrder; i++) {
        m_Cohefficients[i].x = 0;
        m_Cohefficients[i].y = 0;
        m_Cohefficients[i].z = 0;
        resultR[i] = 0;
        resultG[i] = 0;
        resultB[i] = 0;
    }
    std::vector<float> shBuff(sqOrder);
    std::vector<float> shBuffB(sqOrder);

    // bind current texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    // for each face of cube texture
    for(int face=0; face < 6; face++) {
        // get width and height
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, 0, GL_TEXTURE_HEIGHT, &height);

        if(width != height) {
            std::cout << "Non power of 2 texture cubemap ERROR" <<  std::endl;
            return;
        }

        if(width == 0 || height == 0) {
            std::cout << "invalid texture size" <<  std::endl;
            return;
        }

        // get format of data in texture
        glGetTexLevelParameteriv(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, 0,
                                 GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

        // get data from texture
        if(internalFormat == GL_RGBA) {
            numComponents = 4;
            data = new GLfloat[numComponents * width * width];
        } else if(internalFormat == GL_RGB16F || internalFormat == GL_SRGB) {
            numComponents = 3;
            data = new GLfloat[numComponents * width * width];
        } else {
            std::cout << "Format not compatible" << std::endl;
            return;
        }
        glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, 0, GL_RGB, GL_FLOAT, data);
        glCheckError();

        if(std::isnan((float)data[0])) std::cout << "data[0] " << data[0] << std::endl;

        // step between two texels for range [0, 1]
        float invWidth = 1.0f / float(width);
        // initial negative bound for range [-1, 1]
        float negativeBound = -1.0f + invWidth;
        // step between two texels for range [-1, 1]
        float invWidthBy2 = 2.0f / float(width);

        for(int y=0; y < width; y++) {
            // texture coordinate V in range [-1 to 1]
            const float fV = negativeBound + float(y) * invWidthBy2;

            for(int x=0; x < width; x++) {
                // texture coordinate U in range [-1 to 1]
                const float fU = negativeBound + float(x) * invWidthBy2;

                // determine direction from center of cube texture to current texel
                glm::vec3 dir;
                switch(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face) {
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
                    dir.x = 1.0f;
                    dir.y = 1.0f - (invWidthBy2 * float(y) + invWidth);
                    dir.z = 1.0f - (invWidthBy2 * float(x) + invWidth);
                    dir = -dir;
                    break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
                    dir.x = -1.0f;
                    dir.y = 1.0f - (invWidthBy2 * float(y) + invWidth);
                    dir.z = -1.0f + (invWidthBy2 * float(x) + invWidth);
                    dir = -dir;
                    break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
                    dir.x = - 1.0f + (invWidthBy2 * float(x) + invWidth);
                    dir.y = 1.0f;
                    dir.z = - 1.0f + (invWidthBy2 * float(y) + invWidth);
                    dir = -dir;
                    break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
                    dir.x = - 1.0f + (invWidthBy2 * float(x) + invWidth);
                    dir.y = - 1.0f;
                    dir.z = 1.0f - (invWidthBy2 * float(y) + invWidth);
                    dir = -dir;
                    break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
                    dir.x = - 1.0f + (invWidthBy2 * float(x) + invWidth);
                    dir.y = 1.0f - (invWidthBy2 * float(y) + invWidth);
                    dir.z = 1.0f;
                    break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                    dir.x = 1.0f - (invWidthBy2 * float(x) + invWidth);
                    dir.y = 1.0f - (invWidthBy2 * float(y) + invWidth);
                    dir.z = - 1.0f;
                    break;
                default:
                    return;
                }

                // normalize direction
                dir = glm::normalize(dir);

                // scale factor depending on distance from center of the face
                if(fV == 0 || std::isnan((float)fV)) {
                    std::cout << "fV is not valid or nan" << std::endl;
                    return;
                }
                const float fDiffSolid = 4.0f / ((1.0f + fU*fU + fV*fV) *
                                                 sqrtf(1.0f + fU*fU + fV*fV));
                fWt += fDiffSolid;

                // calculate coefficients of spherical harmonics for current direction
                sphericalHarmonicsEvaluateDirection(shBuff.data(), order, dir, "Evaluating direction");

                // index of texel in texture
                const unsigned int pixOffsetIndex = (x + y * width) * numComponents;

                //if(std::isnan((int)pixOffsetIndex)) std::cout << "pixOffsetIndex == " << pixOffsetIndex << std::endl;


                if(std::isnan((float)data[pixOffsetIndex])){
                    data[pixOffsetIndex] = 0.0f;
                    //std::cout << "data[pixOffsetIndex] is nan on face: " << face << " for pixOffsetIndex = " << pixOffsetIndex  << std::endl;

                }
                if(std::isnan((float)data[pixOffsetIndex+1])){
                    data[pixOffsetIndex+1] = 0.0f;
                    //std::cout << "data[pixOffsetIndex+1] is nan on face: " << face << " for pixOffsetIndex = " << pixOffsetIndex  << std::endl;

                }
                if(std::isnan((float)data[pixOffsetIndex+2])){
                    data[pixOffsetIndex+2] = 0.0f;
                    //std::cout << "data[pixOffsetIndex+2] is nan on face: " << face << " for pixOffsetIndex = " << pixOffsetIndex  << std::endl;

                }
                // get color from texture and map to range [0, 1]
                glm::vec3 clr(
                    data[pixOffsetIndex] /*/ 255.0f*/,
                    data[pixOffsetIndex+1] /*/ 255.0f*/,
                    data[pixOffsetIndex+2] /*/ 255.0f*/
                );


                if(std::isnan((float)clr.r)) {
                    std::cout << "clr.r is nan" << std::endl;
                }
                if(std::isnan((float)fDiffSolid)) {
                    std::cout << "fDiffSolid is nan" << std::endl;
                }

                // scale color and add to previously accumulated coefficients
                sphericalHarmonicsScale(shBuffB.data(), order,
                                        shBuff.data(), clr.r * fDiffSolid, "Calculating B first time");

                sphericalHarmonicsAdd(resultR.data(), order,
                                      resultR.data(), shBuffB.data(), "Calculating R");

                sphericalHarmonicsScale(shBuffB.data(), order,
                                        shBuff.data(), clr.g * fDiffSolid, "Calculating B first time");
                sphericalHarmonicsAdd(resultG.data(), order,
                                      resultG.data(), shBuffB.data(), "Calculating G");
                sphericalHarmonicsScale(shBuffB.data(), order,
                                        shBuff.data(), clr.b * fDiffSolid, "Calculating B first time");
                sphericalHarmonicsAdd(resultB.data(), order,
                                      resultB.data(), shBuffB.data(), "Calculating B");
            }

        }
    }

    // final scale for coefficients
    if(fWt == 0) {
        std::cout << "fWt is zero" << std::endl;
        return;
    }

    const float fNormProj = (4.0f * PI) / fWt;
    sphericalHarmonicsScale(resultR.data(), order, resultR.data(), fNormProj, "Calculating R first time");
    sphericalHarmonicsScale(resultG.data(), order, resultG.data(), fNormProj, "Calculating G first time");
    sphericalHarmonicsScale(resultB.data(), order, resultB.data(), fNormProj, "Calculating B first time");


    // save result
    for(unsigned int i=0; i < sqOrder; i++) {
        m_Cohefficients[i].r = resultR[i];
        m_Cohefficients[i].g = resultG[i];
        m_Cohefficients[i].b = resultB[i];
    }
    this->SaveCohefficients();
}

void SphericalHarmonics::sphericalHarmonicsEvaluateDirection(float * result, int order,
        const glm::vec3 & dir, std::string why) {
    // calculate coefficients for first 3 bands of spherical harmonics
    double p_0_0 = 0.282094791773878140;
    double p_1_0 = 0.488602511902919920 * dir.z;
    double p_1_1 = -0.488602511902919920;
    double p_2_0 = 0.946174695757560080 * dir.z * dir.z - 0.315391565252520050;
    double p_2_1 = -1.092548430592079200 * dir.z;
    double p_2_2 = 0.546274215296039590;

    if(std::isnan(dir.x) || std::isnan(dir.y) || std::isnan(dir.z)) {
        std::cout << "In function sphericalHarmonicsEvaluateDirection: dir is nan; while attempting " << why << std::endl;
        std::exit(1);
        return;
    }

    result[0] = p_0_0;

    if(std::isnan(result[0])) {
        std::cout << "In function sphericalHarmonicsEvaluateDirection: result[0] is nan; while attempting " << why << std::endl;
        std::exit(1);
        return;
    }
    result[1] = p_1_1 * dir.y;
    result[2] = p_1_0;
    result[3] = p_1_1 * dir.x;
    result[4] = p_2_2 * (dir.x * dir.y + dir.y * dir.x);
    result[5] = p_2_1 * dir.y;
    result[6] = p_2_0;
    result[7] = p_2_1 * dir.x;
    result[8] = p_2_2 * (dir.x * dir.x - dir.y * dir.y);
}

void SphericalHarmonics::sphericalHarmonicsAdd(float * result, int order,
        const float * inputA, const float * inputB, std::string why) {
    const int numCoeff = order * order;
    for(int i=0; i < numCoeff; i++) {
        if(std::isnan(inputA[i])) {
            std::cout << "In function sphericalHarmonicsAdd: inputA[" << i << "] is nan; while attempting " << why << std::endl;
            std::exit(1);
            return;
        } else if(std::isnan(inputB[i])) {
            std::cout << "In function sphericalHarmonicsAdd: inputB[" << i << "] is nan; while attempting " << why << std::endl;
            std::exit(1);
            return;
        }
        result[i] = inputA[i] + inputB[i];
    }
}

void SphericalHarmonics::sphericalHarmonicsScale(float * result, int order,
        const float * input, float scale, std::string why) {
    const int numCoeff = order * order;
    for(int i=0; i < numCoeff; i++) {
        if(std::isnan(input[i])) {
            std::cout << "In function sphericalHarmonicsScale: input[" << i << "] is nan; while attempting " << why << std::endl;
            std::exit(1);
            return;
        }
        result[i] = input[i] * scale;

        if(std::isnan(result[i])) {
            std::cout << "In function sphericalHarmonicsScale: input[" << i << "] * " << scale << " = result == nan; while attempting " << why << std::endl;
            std::exit(1);
            return;
        }
    }
}

}