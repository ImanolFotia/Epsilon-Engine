///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#pragma once
#include <pch.hpp>
#include <Shader.h>
#include <camera.h>
#include <Renderer/Shadows/ShadowMapping.h>

#include <Light.h>
#include <Texture.h>
#include <Driver/API/OpenGL/FrameBuffer.h>
#include <Renderer/Shadows/PointShadow.hpp>

namespace Epsilon
{
    class PostProcess
    {
    public:

        enum GBUFFER_TARGETS {
            GBUFFER_ALBEDO_SPEC = 0,
            GBUFFER_NORMAL,
            GBUFFER_IBL_DEPTH,
            GBUFFER_MOTION_EXTRA,
            GBUFFER_GI
        };

        PostProcess();

       /* virtual ~PostProcess()
        {
            //std::cout << "Deleted PostProcess" << std::endl;
        }*/

    public:
        /**
        Attaches the off-screen rendering buffer
    */
        void beginOffScreenrendering(void);

        /**
        Disattaches the off-screen rendering buffer
    */
        void endOffScreenRendering(void);

        /**
        Render the output from the G-Buffer
    */
        void ShowFrame(glm::vec3, bool &hdr, std::shared_ptr<Camera> &cam, float exposure, std::shared_ptr<Renderer::ShadowMap>, std::shared_ptr<Renderer::PointShadow>);

        /**
        Calculates the screen space ambient occlusion
        from the geometry
    */
        void applySSAO(std::shared_ptr<Camera> &cam);
        
        void applyHBIL(std::shared_ptr<Camera> &cam);

        /**
        Render the post process image to the screen
        after the post-process effects has been applied
    */
        void ShowPostProcessImage(float exposure, GLuint, glm::vec3 Sun, std::shared_ptr<Camera> &cam, std::shared_ptr<Epsilon::OpenGL::FrameBuffer<int>>);

        GLuint getSceneTexture()
        {
            return hdrFBO->getRenderTargetHandler("colorBuffer");
        }

        void Destroy()
        {
            glDeleteTextures(1, &colorBuffer);
            glDeleteTextures(1, &depthBuffer);
            //glDeleteTextures(1, &MotionBlurBuffer);
            glDeleteTextures(2, SSRTexture);
            glDeleteTextures(2, pingpongSSRT);
            glDeleteTextures(1, &DownSampledTexture);
            glDeleteTextures(1, &SinglePixelColorBuffer);
            //glDeleteTextures(1, &gExtraComponents);
            //glDeleteTextures(1, &rboDepth);
            //glDeleteTextures(1, &rDepth);
            //glDeleteTextures(1, &gDepth);
            glDeleteTextures(1, &DepthTexture);
            glDeleteTextures(1, &lowresTex);
            glDeleteTextures(1, &DOFBuffer);
            glDeleteTextures(1, &sampler);
            glDeleteTextures(2, pingpongDOF);
            //glDeleteTextures(2, pingpongColorbuffers);
            glDeleteTextures(2, pingpongColorbuffersDOF);

            //glDeleteFramebuffers(1, &MotionBlurFBO);
            glDeleteFramebuffers(2, SSRFBO);
            glDeleteFramebuffers(2, pingpongSSRFBO);
            glDeleteFramebuffers(1, &lowresFBO);

            //glDeleteFramebuffers(1, &ssaoFBO);
            //glDeleteFramebuffers(2, pingpongFBO);
            glDeleteFramebuffers(1, &DownSamplerFBO);
        }

        void Resize(int w, int h) {
            width = w;
            height = h;
            mRecalculateSSAO = true;
            gBufferFramebuffer->Resize(w, h);
            mCompositeImage->Resize(w, h);
            mHBAOFramebuffer->Resize(w, h);
            mHBAOBlurFramebuffer->Resize(w, h);
            hdrFBO->Resize(w, h);
            mPingPongFramebuffer[0]->Resize(w, h);
            mPingPongFramebuffer[1]->Resize(w, h);
            mMotionBlurFramebuffer->Resize(w, h);
        }

    public:
        GLuint colorBuffer;
        GLuint depthBuffer;
        bool mRecalculateSSAO = false;
        std::shared_ptr<OpenGL::FrameBuffer<std::string>> hdrFBO;
        std::shared_ptr<OpenGL::FrameBuffer<int>> mCompositeImage;
        std::shared_ptr<Shader> shader;
        std::shared_ptr<OpenGL::FrameBuffer<int>> CopyTextureFBO;
        std::shared_ptr<OpenGL::FrameBuffer<int>> CopyTextureBlurredFBO;

        float m_exposure;
        //GLuint gDepth;

        bool HBAOOn;
    public:
        /**
        loads the shaders used for render off-screen
        and post process affects
    */
        void LoadOffscreensShaders(void);

        /**
        Creates the framebuffer objects and attaches the proper textures
    */
        void SetupFramebuffer(void);

        /**
        Renders a quad that fills the screen
    */
        void RenderQuad(void);

        /**
        Lerp function to interpolate values
    */
        float lerp(float y1, float y2, float mu);

        /**
        Creates the GBuffer and attaches the proper textures
    */
        void SetupGBuffer(void);

        /**
        Creates the SSAO framebuffer and attaches the proper textures
    */
        void setupSSAO(void);
        
        void setupHBIL(void);

        /**
        Creates the SSR framebuffer and attaches the proper textures
    */
        void setupSSR(void);

        /**
        SSR
    */
        void SSRPass(std::shared_ptr<Camera> &cam);
        void setupDownSampledSSR();
        void DownSampleSSR(double);

        /**
        Blurs and image using a Gaussian Blur shader
    */
        GLuint blurImage(GLuint Buffer, bool);

        /**
        Set up buffers for blurring
    */
        void SetupPingPongFBO(void);

        /**
        Set up buffers for blurring
    */
        void SetupPingPongDOF(void);

        /**
        gets the luminicense from a texture
    */
        GLfloat applyAutoAxposure(GLuint Buffer);

        void SetupMotionBlur();

        void MotionBlur(float);

        GLuint blurSSR(GLuint);
        void setupPingPongSSR();

        void CompositeImage(bool);

        GLuint GetPixel(GLuint);

        void generateBRDF();

        void setupDenoise();

    public:
        //#pragma pack(push, 1)

        std::vector<t_light> m_Lights;
        GLuint SSAOwidth;
        GLuint SSAOheight;
        //GLuint MotionBlurBuffer;
        //GLuint MotionBlurFBO;
        GLuint SSRFBO[2];
        GLuint SSRTexture[2];
        GLuint pingpongSSRFBO[2];
        GLuint pingpongSSRT[2];
        GLuint DownSamplerFBO;
        GLuint DownSampledTexture;
        GLuint SinglePixelColorBuffer;
       // GLuint gExtraComponents;
        bool CurrentSSR = 0;
        bool SSROn;
        bool m_MotionBlur;
        bool lightShafts;
        bool mParallaxMapping;
        bool mMotionBlurStrength;
        int mHBAOQuality;
        bool mChromaticAberration;
        bool mBokehDOF;

        std::shared_ptr<OpenGL::FrameBuffer<int>> BRDFFramebuffer;
        std::shared_ptr<OpenGL::FrameBuffer<int>> HBILFramebuffer;
        std::shared_ptr<OpenGL::FrameBuffer<int>> mHBAOFramebuffer;
        std::shared_ptr<OpenGL::FrameBuffer<int>> mHBAOBlurFramebuffer;
        
        std::shared_ptr<OpenGL::FrameBuffer<int>> gBufferFramebuffer; 
        std::shared_ptr<OpenGL::FrameBuffer<int>> mMotionBlurFramebuffer;

        
        std::shared_ptr<OpenGL::FrameBuffer<int>> mPingPongFramebuffer[2];

        //GLuint ssaoFBO;
        //GLuint ssaoBlurFBO;
        //GLuint rboDepth;
        //GLuint rDepth;
        GLuint quadVAO = 0;
        GLuint quadVBO;
        GLuint sampler;
        GLuint intermediateFBO;
        GLuint DepthTexture;
        GLuint lowresFBO;
        float lastDepth;
        GLuint lowresTex;
        //GLuint pingpongFBO[2];
        //GLuint pingpongColorbuffers[2];
        GLuint pingpongDOF[2];
        GLuint pingpongColorbuffersDOF[2];
        GLuint DOFBuffer;
        glm::vec3 Sun;
        std::vector<glm::vec3> ssaoNoise;
        GLenum attachment_type;
        int width, height;
        float exposureTime;
        std::shared_ptr<Shader> SSAO;
        std::shared_ptr<Shader> HBIL;
        std::shared_ptr<Shader> blurSSAO;
        std::shared_ptr<Shader> finalImage;
        std::shared_ptr<Shader> blurBloom;
        std::shared_ptr<Shader> ScreenSpaceReflectionShader;
        std::shared_ptr<Shader> blurSSRShader;
        std::shared_ptr<Shader> PassThroughShader;
        std::shared_ptr<Shader> MotionBlurShader;
        std::shared_ptr<Shader> CompositeShader;
        std::shared_ptr<Shader> BRDFShader;
        std::shared_ptr<Shader> DenoiseShader;

        std::vector<glm::vec3> LightPositions;
        /// G-Buffer texture samplers
        std::vector<glm::vec3> ssaoKernel;
        GLuint noiseTexture;
        GLuint DenoiseTexture;
        GLuint DenoiseFBO;
        GLuint brightColorBuffer;
        //GLuint gBuffer;
        //GLuint gAlbedoSpec;
        GLuint gPosition;
        GLuint gNormal;
        GLuint gLowResDepth;
        //GLuint gExpensiveNormal;
        GLuint ssaoColorBuffer;
        GLuint ssaoColorBufferBlur;
        GLuint gWorldSpacePosition;
        //GLuint gLightAccumulation;
        GLuint mLastFrametexture;
        GLuint mBlurredSSR;
        GLuint BRDF = 0;
        GLuint ssbo = 0;
        float blurSize = 0.0f;
        std::shared_ptr<eTexture> lensColor;
        std::shared_ptr<eTexture> lensDirt;
        std::shared_ptr<eTexture> lensStar;
        std::shared_ptr<eTexture> BlueNoiseTexture;

        glm::vec2 FocalLen, InvFocalLen, UVToViewA, UVToViewB, LinMAD;

        GLuint ReflectionTexture;
        bool HBAO_params_calculated = false;
    };
} // namespace Epsilon

