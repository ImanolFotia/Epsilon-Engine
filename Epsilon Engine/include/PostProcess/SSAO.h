#pragma once

#include <PostProcess/Effect.h>
#include <Includes.h>

namespace PostProcess {

    class SSAO : public Effect
    {
    public:
        SSAO(int, int);

        virtual GLuint Apply(GLuint, GLuint, glm::mat4 projection, glm::mat4 view);

        virtual void Update(){}

        /**----------------------------------------*/
        /**           SSAO Specific Code           */
        /**----------------------------------------*/

        void PrepareSSAOTexture();

        void PrepareSSAOBlurTexture();

        void PrepareRendomSamplingTexture();


    private:
        GLuint noiseTexture;
        std::vector<glm::vec3> ssaoKernel;
        std::shared_ptr<Shader> blurShader;
        std::shared_ptr<FrameBuffer<std::string> > m_pFrameBufferBlur;
        std::vector<glm::vec3> ssaoNoise;


    protected:

    };
}
