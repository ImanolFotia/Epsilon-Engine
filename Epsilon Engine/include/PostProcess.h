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

class PostProcess
{
public:
    /**
        Purpose: Constructor
    */
    PostProcess();

    /**
        Purpose: Destructor
    */
    virtual ~PostProcess(){
        delete shader;
        delete shader2;
        delete SSAO;
        delete blurSSAO;
        delete finalImage;
        delete blurBloom;
    }

public:

    /**
        Purpose: Attaches the off-screen rendering buffer
    */
    void beginOffScreenrendering(void);

    /**
        Purpose: Disattaches the off-screen rendering buffer
    */
    void endOffScreenRendering(void);

    /**
        Purpose: Render the output from the G-Buffer
    */
    void ShowFrame(glm::vec3, bool & hdr, Camera* cam, float exposure);

    /**
        Purpose: Calculates the screen space ambient occlusion
        from the geometry
    */
    void applySSAO(Camera * cam);

    /**
        Purpose: Render the post process image to the screen
        after the post-process effects has been applied
    */
    void ShowPostProcessImage(float exposure);

public:

    GLuint colorBuffer;
    GLuint depthBuffer;
    GLuint hdrFBO;
    Shader* shader;

private:

    /**
        Purpose: loads the shaders used for render off-screen
        and post process affects
    */
    void LoadOffscreensShaders(void);

    /**
        Purpose: Creates the framebuffer objects and attaches the proper textures
    */
    void SetupFramebuffer(void);

    /**
        Purpose: Renders a quad that fills the screen
    */
    void RenderQuad(void);

    /**
        Purpose: Lerp function to interpolate values
    */
    float lerp(float y1,float y2,float mu);

    /**
        Purpose: Creates the GBuffer and attaches the proper textures
    */
    void SetupGBuffer(void);

    /**
        Purpose: Creates the SSAO framebuffer and attaches the proper textures
    */
    void setupSSAO(void);

    /**
        Purpose: Blurs and image using a Gaussian Blur shader
    */
    GLuint blurImage(GLuint Buffer);

    /**
        Purpose: Set up buffers for blurring
    */
    void SetupPingPongFBO(void);

    /**
        Purpose: Set up buffers for blurring
    */
    void SetupPingPongDOF(void);

    /**
        Purpose: gets the luminicense from a texture
    */
    GLfloat applyAutoAxposure(GLuint Buffer);

private:
    GLuint SSAOwidth;
    GLuint SSAOheight;
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
    Shader* shader2;
    Shader* SSAO;
    Shader* blurSSAO;
    Shader* finalImage;
    Shader* blurBloom;
    /// G-Buffer texture samplers
    std::vector<glm::vec3> ssaoKernel;
    GLuint noiseTexture;
    GLuint brightColorBuffer;
    GLuint gBuffer;
    GLuint gAlbedoSpec;
    GLuint gPositionDepth;
    GLuint gNormal;
    GLuint gExpensiveNormal;
    GLuint ssaoColorBuffer;
    GLuint ssaoColorBufferBlur;
    GLuint gWorldSpacePosition;
    GLuint m_exposure;

};

#endif // POSTPROCESS_H_INCLUDED
