#include <SphericalHarmonics.h>
#include <Types.h>
void SphericalHarmonics::CalculateCohefficients(CubeMap cubemap, const unsigned int order)
{
   const unsigned int sqOrder = order*order;

   // allocate memory for calculations
   m_Cohefficients.resize(sqOrder);
   std::vector<float> resultR(sqOrder);
   std::vector<float> resultG(sqOrder);
   std::vector<float> resultB(sqOrder);

   // variables that describe current face of cube texture
   std::unique_ptr<GLubyte[]> data;
   GLint width, height;
   GLint internalFormat;
   GLint numComponents;

   // initialize values
   float fWt = 0.0f;
   for(unsigned int i=0; i < sqOrder; i++)
   {
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
   glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap.getTextureID());
   // for each face of cube texture
   for(int face=0; face < 6; face++)
   {
      // get width and height
      glGetTexLevelParameteriv(cubemap.getCubemapFace(face), 0, GL_TEXTURE_WIDTH, &width);
      glGetTexLevelParameteriv(cubemap.getCubemapFace(face), 0, GL_TEXTURE_HEIGHT, &height);

      if(width != height)
      {
         return;
      }

      // get format of data in texture
      glGetTexLevelParameteriv(cubemap.getCubemapFace(face), 0,
            GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);

      // get data from texture
      if(internalFormat == GL_RGBA)
      {
         numComponents = 4;
         data = (std::unique_ptr<GLubyte[]>)(new GLubyte[numComponents * width * width]);
      }
      else if(internalFormat == GL_RGB || internalFormat == GL_SRGB)
      {
         numComponents = 3;
         data = (std::unique_ptr<GLubyte[]>)(new GLubyte[numComponents * width * width]);
      }
      else
      {
         return;
      }
      glGetTexImage(cubemap.getCubemapFace(face), 0, internalFormat, GL_UNSIGNED_BYTE, data.get());

      // step between two texels for range [0, 1]
      float invWidth = 1.0f / float(width);
      // initial negative bound for range [-1, 1]
      float negativeBound = -1.0f + invWidth;
      // step between two texels for range [-1, 1]
      float invWidthBy2 = 2.0f / float(width);

      for(int y=0; y < width; y++)
      {
         // texture coordinate V in range [-1 to 1]
         const float fV = negativeBound + float(y) * invWidthBy2;

         for(int x=0; x < width; x++)
         {
            // texture coordinate U in range [-1 to 1]
            const float fU = negativeBound + float(x) * invWidthBy2;

            // determine direction from center of cube texture to current texel
            glm::vec3 dir;
            switch(cubemap.getCubemapFace(face))
            {
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
            const float fDiffSolid = 4.0f / ((1.0f + fU*fU + fV*fV) *
                                       sqrtf(1.0f + fU*fU + fV*fV));
            fWt += fDiffSolid;

            // calculate coefficients of spherical harmonics for current direction
            sphericalHarmonicsEvaluateDirection(shBuff.data(), order, dir);

            // index of texel in texture
            unsigned int pixOffsetIndex = (x + y * width) * numComponents;
            // get color from texture and map to range [0, 1]
            glm::vec3 clr(
                  float(data[pixOffsetIndex]) / 255,
                  float(data[pixOffsetIndex+1]) / 255,
                  float(data[pixOffsetIndex+2]) / 255
               );

            // scale color and add to previously accumulated coefficients
            sphericalHarmonicsScale(shBuffB.data(), order,
                  shBuff.data(), clr.r * fDiffSolid);
            sphericalHarmonicsAdd(resultR.data(), order,
                  resultR.data(), shBuffB.data());
            sphericalHarmonicsScale(shBuffB.data(), order,
                  shBuff.data(), clr.g * fDiffSolid);
            sphericalHarmonicsAdd(resultG.data(), order,
                  resultG.data(), shBuffB.data());
            sphericalHarmonicsScale(shBuffB.data(), order,
                  shBuff.data(), clr.b * fDiffSolid);
            sphericalHarmonicsAdd(resultB.data(), order,
                  resultB.data(), shBuffB.data());
         }
      }
   }

   // final scale for coefficients
   const float fNormProj = (4.0f * PI) / fWt;
   sphericalHarmonicsScale(resultR.data(), order, resultR.data(), fNormProj);
   sphericalHarmonicsScale(resultG.data(), order, resultG.data(), fNormProj);
   sphericalHarmonicsScale(resultB.data(), order, resultB.data(), fNormProj);


   // save result
   for(unsigned int i=0; i < sqOrder; i++)
   {
      m_Cohefficients[i].r = resultR[i];
      m_Cohefficients[i].g = resultG[i];
      m_Cohefficients[i].b = resultB[i];
   }

   this->SaveCohefficients();
}

void SphericalHarmonics::sphericalHarmonicsEvaluateDirection(float * result, int order,
                              const glm::vec3 & dir)
{
   // calculate coefficients for first 3 bands of spherical harmonics
   double p_0_0 = 0.282094791773878140;
   double p_1_0 = 0.488602511902919920 * dir.z;
   double p_1_1 = -0.488602511902919920;
   double p_2_0 = 0.946174695757560080 * dir.z * dir.z - 0.315391565252520050;
   double p_2_1 = -1.092548430592079200 * dir.z;
   double p_2_2 = 0.546274215296039590;
   result[0] = p_0_0;
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
               const float * inputA, const float * inputB)
{
   const int numCoeff = order * order;
   for(int i=0; i < numCoeff; i++)
   {
      result[i] = inputA[i] + inputB[i];
   }
}

void SphericalHarmonics::sphericalHarmonicsScale(float * result, int order,
               const float * input, float scale)
{
   const int numCoeff = order * order;
   for(int i=0; i < numCoeff; i++)
   {
      result[i] = input[i] * scale;
   }
}
