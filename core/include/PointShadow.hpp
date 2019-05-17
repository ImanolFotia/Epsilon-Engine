#pragma once
#include <vector>
#include <GL/glew.h>

#include <Shader.h>

class PointShadow
{
    public:
        PointShadow(glm::vec3 p, unsigned short r = 512, unsigned short n = 500, unsigned short f = 500) : mPosition(p), mResolution(r), mNear(n), mFar(f){ }

        void Setup()
        {
            SetupTexture();

            mProjection = glm::perspective(glm::radians(90.0), 1.0, (double)mNear, (double)mFar);

            glm::vec3 dirs[6] = {   glm::vec3( 1.0f,  0.0f,  0.0f),
                                    glm::vec3(-1.0f,  0.0f,  0.0f),
                                    glm::vec3( 0.0f,  1.0f,  0.0f),
                                    glm::vec3( 0.0f, -1.0f,  0.0f),
                                    glm::vec3( 0.0f,  0.0f,  1.0f),
                                    glm::vec3( 0.0f,  0.0f, -1.0f)
                                };

            glm::vec3 ups[6] =  {   glm::vec3(0.0f, -1.0f,  0.0f),
                                    glm::vec3(0.0f, -1.0f,  0.0f),
                                    glm::vec3(0.0f,  0.0f,  1.0f),
                                    glm::vec3(0.0f,  0.0f, -1.0f),
                                    glm::vec3(0.0f, -1.0f,  0.0f),
                                    glm::vec3(0.0f, -1.0f,  0.0f)
                                };

            unsigned i = 0;
            mViews.reserve(6);
            for(auto& b: mViews) {
                b = glm::lookAt(mPosition, dirs[i] + mPosition, ups[i]);
                i++;
            }
        }

        void Bind(Shader*){
            glBindFramebuffer(GL_FRAMEBUFFER, this->mShadowFBO);
            glViewport(0, 0,  this->mResolution, this->mResolution);
            glClear(GL_DEPTH_BUFFER_BIT);
        }

        void Unbind(){
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        GLuint getTexture() { return mShadowTexture; }

        void SetupTexture() {
                glGenFramebuffers(1, &this->mShadowFBO);
                glGenTextures(1, &mShadowTexture);
                glBindTexture(GL_TEXTURE_2D, mShadowTexture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->mResolution, this->mResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
                glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
                GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
                glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
                glBindFramebuffer(GL_FRAMEBUFFER, this->mShadowFBO);
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mShadowTexture, 0);
                glDrawBuffer(GL_NONE);
                glReadBuffer(GL_NONE);
                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) std::cout << "Framebuffer not complete!" << std::endl;
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                glBindTexture(GL_TEXTURE_2D, 0);
        }

    private:
        GLuint mShadowTexture = 0;
        GLuint mShadowFBO = 0;
        glm::vec3 mPosition;
        glm::mat4 mProjection;
        std::vector<glm::mat4> mViews;

        unsigned short mResolution = 512;
        unsigned short mNear, mFar;
    protected:
};
