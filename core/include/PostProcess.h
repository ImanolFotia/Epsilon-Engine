///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef POSTPROCESS_H_INCLUDED
#define POSTPROCESS_H_INCLUDED

#include <GL/glew.h>
#include <Shader.h>
#include <camera.h>
#include <vector>
#include <ShadowMapping.h>
#include <memory>
#include <Light.h>
#include <Texture.h>
#include <OpenGL/FrameBuffer.h>

class PostProcess
{
public:

    PostProcess();

    virtual ~PostProcess(){
        //std::cout << "Deleted PostProcess" << std::endl;
    }

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
    void ShowFrame(glm::vec3, bool & hdr, std::shared_ptr<Camera>& cam, float exposure, std::unique_ptr<ShadowMap>&);

    /**
        Calculates the screen space ambient occlusion
        from the geometry
    */
    void applySSAO(std::shared_ptr<Camera>& cam);

    /**
        Render the post process image to the screen
        after the post-process effects has been applied
    */
    void ShowPostProcessImage(float exposure, GLuint, glm::vec3 Sun, std::shared_ptr<Camera>& cam);

    GLuint getSceneTexture()
    {
        return hdrFBO->getRenderTargetHandler("colorBuffer");
    }

    void Destroy()
    {
        glDeleteTextures(1, &colorBuffer);
        glDeleteTextures(1, &depthBuffer);
        glDeleteTextures(1, &MotionBlurBuffer);
        glDeleteTextures(2, SSRTexture);
        glDeleteTextures(2, pingpongSSRT);
        glDeleteTextures(1, &DownSampledTexture);
        glDeleteTextures(1, &SinglePixelColorBuffer);
        glDeleteTextures(1, &gExtraComponents);
        glDeleteTextures(1, &rboDepth);
        glDeleteTextures(1, &rDepth);
        glDeleteTextures(1, &gDepth);
        glDeleteTextures(1, &DepthTexture);
        glDeleteTextures(1, &lowresTex);
        glDeleteTextures(1, &DOFBuffer);
        glDeleteTextures(1, &sampler);
        glDeleteTextures(2, pingpongDOF);
        glDeleteTextures(2, pingpongColorbuffers);
        glDeleteTextures(2, pingpongColorbuffersDOF);

        glDeleteFramebuffers(1, &MotionBlurFBO);
        glDeleteFramebuffers(2, SSRFBO);
        glDeleteFramebuffers(2, pingpongSSRFBO);
        glDeleteFramebuffers(1, &lowresFBO);

        glDeleteFramebuffers(1, &ssaoFBO);
        glDeleteFramebuffers(2, pingpongFBO);
        glDeleteFramebuffers(1, &DownSamplerFBO);
    }

public:

    GLuint colorBuffer;
    GLuint depthBuffer;

    std::shared_ptr<FrameBuffer<std::string> > hdrFBO;
    std::shared_ptr<FrameBuffer<int> > mCompositeImage;
    std::unique_ptr<Shader> shader;
    std::shared_ptr<FrameBuffer<int> > CopyTextureFBO;

    float m_exposure;
    GLuint gDepth;
private:

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
    float lerp(float y1,float y2,float mu);

    /**
        Creates the GBuffer and attaches the proper textures
    */
    void SetupGBuffer(void);

    /**
        Creates the SSAO framebuffer and attaches the proper textures
    */
    void setupSSAO(void);

    /**
        Creates the SSR framebuffer and attaches the proper textures
    */
    void setupSSR(void);

    /**
        SSR
    */
    void SSRPass(std::shared_ptr<Camera>& cam);
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

private:
    //#pragma pack(push, 1)

    std::vector<t_light> m_Lights;
    GLuint SSAOwidth;
    GLuint SSAOheight;
    GLuint MotionBlurBuffer;
    GLuint MotionBlurFBO;
    GLuint SSRFBO[2];
    GLuint SSRTexture[2];
    GLuint pingpongSSRFBO[2];
    GLuint pingpongSSRT[2];
    GLuint DownSamplerFBO;
    GLuint DownSampledTexture;
    GLuint SinglePixelColorBuffer;
    GLuint gExtraComponents;
    bool CurrentSSR = 0;
    bool SSROn;
    bool m_MotionBlur;
    bool lightShafts;
    bool mParallaxMapping;
    bool mMotionBlurStrength;
    int mHBAOQuality;
    bool mChromaticAberration;
    bool mBokehDOF;

std::shared_ptr<FrameBuffer<int> > BRDFFramebuffer;
    GLuint ssaoFBO;
    GLuint ssaoBlurFBO;
    GLuint rboDepth;
    GLuint rDepth;
    GLuint quadVAO = 0;
    GLuint quadVBO;
    GLuint sampler;
    GLuint intermediateFBO;
    GLuint DepthTexture;
    GLuint lowresFBO;
    float lastDepth;
    GLuint lowresTex;
    GLuint pingpongFBO[2];
    GLuint pingpongColorbuffers[2];
    GLuint pingpongDOF[2];
    GLuint pingpongColorbuffersDOF[2];
    GLuint DOFBuffer;
    glm::vec3 Sun;
    std::vector<glm::vec3> ssaoNoise;
    GLenum attachment_type;
    int width, height;
    float exposureTime;
    std::unique_ptr<Shader> SSAO;
    std::unique_ptr<Shader> blurSSAO;
    std::unique_ptr<Shader> finalImage;
    std::unique_ptr<Shader> blurBloom;
    std::unique_ptr<Shader> ScreenSpaceReflectionShader;
    std::unique_ptr<Shader> blurSSRShader;
    std::unique_ptr<Shader> PassThroughShader;
    std::unique_ptr<Shader> MotionBlurShader;
    std::unique_ptr<Shader> CompositeShader;
    std::unique_ptr<Shader> BRDFShader;
    std::unique_ptr<Shader> DenoiseShader;

    std::vector<glm::vec3> LightPositions;
    /// G-Buffer texture samplers
    std::vector<glm::vec3> ssaoKernel;
    GLuint noiseTexture;
    GLuint DenoiseTexture;
    GLuint DenoiseFBO;
    GLuint brightColorBuffer;
    GLuint gBuffer;
    GLuint gAlbedoSpec;
    GLuint gPosition;
    GLuint gNormal;
    GLuint gLowResDepth;
    GLuint gExpensiveNormal;
    GLuint ssaoColorBuffer;
    GLuint ssaoColorBufferBlur;
    GLuint gWorldSpacePosition;
    GLuint gLightAccumulation;
    GLuint mLastFrametexture;
    GLuint mBlurredSSR;
    GLuint BRDF = 0;
    GLuint ssbo = 0;
    std::shared_ptr<eTexture> lensColor;
    std::shared_ptr<eTexture> lensDirt;
    std::shared_ptr<eTexture> lensStar;


    glm::vec2 FocalLen, InvFocalLen, UVToViewA, UVToViewB, LinMAD;



};

#endif // POSTPROCESS_H_INCLUDED
