#include <PostProcess.h>
#include <iostream>
#include <ProgramData.h>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <string>
#include <Includes.h>
#include <IO/KeyBoard.h>
#include <OpenGL/HelperFunctions/CheckError.h>
#include <chrono>
PostProcess::PostProcess()
{
    this->LoadOffscreensShaders();
    this->SetupFramebuffer();
    this->lastDepth = 0.2;
}

static int TotalFrames = 0;

void PostProcess::SetupFramebuffer()
{
    ProgramData PG;
    width = PG.WINDOW_WIDTH;
    height = PG.WINDOW_HEIGHT;
    SSROn = PG.SCREEN_SPACE_REFLECTIONS;
    this->SSAOwidth = width;
    this->SSAOheight = height;
    this->lightShafts = PG.LIGHTSHAFTS;
    this->m_MotionBlur = PG.MOTION_BLUR;
    this->HBAOOn = PG.HBAO;

    mParallaxMapping = PG.PARALLAX_OCLUSSION_MAPPING;
    mMotionBlurStrength = PG.MOTION_BLUR_STRENGTH;
    mHBAOQuality = PG.HBAO_QUALITY;
    mChromaticAberration = PG.CHROMATIC_ABERRATION;
    mBokehDOF = PG.BOKEH_DOF;

    m_exposure = 1.5;

    std::uniform_real_distribution<GLfloat> randomFloatsz(-20.0f, 45.0f); // generates random floats between 0.0 and 1.0
    std::uniform_real_distribution<GLfloat> randomFloatsx(50.0f, 130.0f); // generates random floats between 0.0 and 1.0
    //std::random_device ;
    std::default_random_engine generator(glfwGetTime());

    t_light tmpLight;

    tmpLight.position = glm::vec4(-1.29, 5.64, 25.7, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 300.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);
/*
    tmpLight.position = glm::vec4(-0.4, 4.75, 16.21, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 100.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);

    tmpLight.position = glm::vec4(-85.072, 13.9, -44.56, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 400.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);

    tmpLight.position = glm::vec4(-14.4, 8.5, -2.81, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 0.6, 0.6, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 200.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);

    tmpLight.position = glm::vec4(21.4, 8.5, -2.81, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 0.6, 0.6, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 200.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);

    tmpLight.position = glm::vec4(21.4, 8.5, 13.6, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 0.6, 0.6, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 200.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);

    tmpLight.position = glm::vec4(-14.4, 8.5, 13.6, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 0.6, 0.6, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 200.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);
*/
    /*
    tmpLight.position = glm::vec4(-112, 7, -12, 1.0);
    tmpLight.direction = glm::vec4(0.74, -0.5761, -0.60, 1.0);
    tmpLight.color = glm::vec4(1, 0.8, 0.8, 1.0);
    tmpLight.radius = 0.25f;
    tmpLight.watts = 300.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);

    tmpLight.position = glm::vec4(-112, 7, 12, 1.0);
    tmpLight.direction = glm::vec4(0.74, -0.5761, -0.60, 1.0);
    tmpLight.color = glm::vec4(1, 0.8, 0.8, 1.0);
    tmpLight.radius = 0.25f;
    tmpLight.watts = 300.0f;
    tmpLight.type = 2;


    tmpLight.position = glm::vec4(-100, 2, 0, 1.0);
    tmpLight.direction = glm::vec4(0.74, -0.5761, -0.60, 1.0);
    tmpLight.color = glm::vec4(1, 0.5, 0.5, 1.0);
    tmpLight.radius = 0.4f;
    tmpLight.watts = 500.0f;
    tmpLight.type = 2;
*/
    /*
    m_Lights.push_back(tmpLight);
*/
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(t_light) * m_Lights.size(), (const void *)&m_Lights[0], GL_DYNAMIC_COPY);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    lensColor = (std::shared_ptr<eTexture>)new eTexture("effects/lenscolor2.png", GL_REPEAT, GL_TEXTURE_1D);
    lensDirt = (std::shared_ptr<eTexture>)new eTexture("effects/lensdirt.png");
    lensStar = (std::shared_ptr<eTexture>)new eTexture("effects/lensstar.png");

    this->BlueNoiseTexture = (std::shared_ptr<eTexture>)new eTexture("LDR_RGBA_0_n.png", GL_REPEAT, GL_TEXTURE_2D, GL_LINEAR);
    /*
                LightPositions.push_back(glm::vec3(-41, 12.0, -23));
                LightPositions.push_back(glm::vec3(-39, 10.3, 2));
                LightPositions.push_back(glm::vec3(17.2, 17.3, -17));
                LightPositions.push_back(glm::vec3(17, 17.3, 27));
                LightPositions.push_back(glm::vec3(122.7, 13.63, 5.3));
                LightPositions.push_back(glm::vec3(-14, 16.63, 32.3));
                LightPositions.push_back(glm::vec3(80.5, 17.21, 1.57));
    */

    hdrFBO = (std::shared_ptr<FrameBuffer<std::string>>)new FrameBuffer<std::string>(width, height, true);

    hdrFBO->addRenderTarget("colorBuffer", GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, false);
    hdrFBO->addRenderTarget("brightColorBuffer", GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
    hdrFBO->FinishFrameBuffer();

    CopyTextureFBO = (std::shared_ptr<FrameBuffer<int>>)new FrameBuffer<int>(width, height, false);
    CopyTextureFBO->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
    CopyTextureFBO->FinishFrameBuffer();

    CopyTextureBlurredFBO = (std::shared_ptr<FrameBuffer<int>>)new FrameBuffer<int>(width, height, true);
    CopyTextureBlurredFBO->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
    CopyTextureBlurredFBO->FinishFrameBuffer();

    SetupGBuffer();
    setupSSAO();
    SetupPingPongFBO();
    setupSSR();
    SetupMotionBlur();
    setupDenoise();

    mCompositeImage = (std::shared_ptr<FrameBuffer<int>>)new FrameBuffer<int>(width, height, true);
    mCompositeImage->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
    mCompositeImage->FinishFrameBuffer();

    BRDFFramebuffer = (std::shared_ptr<FrameBuffer<int>>)new FrameBuffer<int>(512, 512, true);
    BRDFFramebuffer->addRenderTarget(0, GL_RG16F, GL_RG, GL_LINEAR, GL_LINEAR, false);
    BRDFFramebuffer->FinishFrameBuffer();

    generateBRDF();
}

void PostProcess::generateBRDF()
{
    BRDFFramebuffer->bindFramebuffer();
    BRDFFramebuffer->setViewport();
    BRDFFramebuffer->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    BRDFShader->Use();
    glDisable(GL_BLEND);
    this->RenderQuad();
    glEnable(GL_BLEND);

    BRDFFramebuffer->unbindFramebuffer();
    BRDFShader->Free();
    glViewport(0, 0, width, height);
}

void PostProcess::LoadOffscreensShaders()
{
    shader = (std::unique_ptr<Shader>)(new Shader("shaders/Lighting.vglsl", "shaders/Lighting.fglsl"));
    SSAO = (std::unique_ptr<Shader>)(new Shader("shaders/SSAO.vglsl", "shaders/HBAO.glsl"));
    blurSSAO = (std::unique_ptr<Shader>)(new Shader("shaders/blurSSAO.vglsl", "shaders/blurSSAO.fglsl"));
    finalImage = (std::unique_ptr<Shader>)(new Shader("shaders/hdr.vglsl", "shaders/hdr.fglsl"));
    blurBloom = (std::unique_ptr<Shader>)(new Shader("shaders/blurBloom.vglsl", "shaders/blurBloom.fglsl"));
    blurSSRShader = (std::unique_ptr<Shader>)(new Shader("shaders/blurSSR.vglsl", "shaders/blurSSR.fglsl"));
    ScreenSpaceReflectionShader = (std::unique_ptr<Shader>)(new Shader("shaders/SSR.vglsl", "shaders/SSR.fglsl"));
    PassThroughShader = (std::unique_ptr<Shader>)(new Shader("shaders/PassThrough.vglsl", "shaders/PassThrough.fglsl"));
    MotionBlurShader = (std::unique_ptr<Shader>)(new Shader("shaders/MotionBlur.vglsl", "shaders/MotionBlur.fglsl"));
    CompositeShader = (std::unique_ptr<Shader>)(new Shader("shaders/Composite.vglsl", "shaders/Composite.fglsl"));
    BRDFShader = (std::unique_ptr<Shader>)(new Shader("shaders/Composite.vglsl", "shaders/BRDF.glsl"));
    DenoiseShader = (std::unique_ptr<Shader>)(new Shader("shaders/Composite.vglsl", "shaders/denoise.glsl"));
}

void PostProcess::beginOffScreenrendering()
{
    glDisable(GL_BLEND);
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, this->gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void PostProcess::endOffScreenRendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::SetupGBuffer()
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    /// - Color + Specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedoSpec, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    /// - Normal color buffer

    glGenTextures(1, &gExpensiveNormal);
    glBindTexture(GL_TEXTURE_2D, gExpensiveNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gExpensiveNormal, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gDepth, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &gExtraComponents);
    glBindTexture(GL_TEXTURE_2D, gExtraComponents);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gExtraComponents, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &gLightAccumulation);
    glBindTexture(GL_TEXTURE_2D, gLightAccumulation);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gLightAccumulation, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /// - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
    glDrawBuffers(5, DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FB error, status: " << Status << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::setupSSAO()
{

    /// Also create framebuffer to hold SSAO processing stage
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    /// - SSAO color buffer
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->SSAOwidth, this->SSAOheight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    /// - and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->SSAOwidth, this->SSAOheight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    //glGenerateMipmap(GL_TEXTURE_2D);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glGenerateMipmap(GL_TEXTURE_2D);

    // Sample kernel
    std::uniform_real_distribution<GLfloat> randomFloatsClamped(0.2, 1.0); // generates random floats between 0.0 and 1.0
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0);        // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
    for (GLuint i = 0; i < 9; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloatsClamped(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 9.0;

        // Scale samples s.t. they're more aligned to center of kernel
        scale = lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // Noise texture
    for (GLuint i = 0; i < 16; i++)
    {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcess::applySSAO(std::shared_ptr<Camera> &cam)
{
    //DownSampleSSR();
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    SSAO->Use();
    glActiveTexture(GL_TEXTURE0);
    SSAO->PushUniform("gDepth", 0);
    glBindTexture(GL_TEXTURE_2D, this->gDepth);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    SSAO->PushUniform("texNoise", 1);
    glBindTexture(GL_TEXTURE_2D, this->noiseTexture);
    /*
    	glActiveTexture(GL_TEXTURE2);
    	SSAO->PushUniform("gNormal", 2);
    	glBindTexture(GL_TEXTURE_2D, this->gExpensiveNormal);
    */
    {
        FocalLen.x = 1.0f / tanf(glm::radians(cam->getFoV()) * 0.5f) * ((float)SSAOheight / (float)SSAOwidth);
        FocalLen.y = 1.0f / tanf(glm::radians(cam->getFoV()) * 0.5f);
        InvFocalLen.x = 1.0f / FocalLen.x;
        InvFocalLen.y = 1.0f / FocalLen.y;

        UVToViewA.x = -2.0f * InvFocalLen.x;
        UVToViewA.y = -2.0f * InvFocalLen.y;
        UVToViewB.x = 1.0f * InvFocalLen.x;
        UVToViewB.y = 1.0f * InvFocalLen.y;

        float near = 0.1f, far = 3000.0f;
        LinMAD.x = (near - far) / (2.0f * near * far);
        LinMAD.y = (near + far) / (2.0f * near * far);

        SSAO->PushUniform("FocalLen", FocalLen);
        SSAO->PushUniform("UVToViewA", UVToViewA);
        SSAO->PushUniform("UVToViewB", UVToViewB);
        SSAO->PushUniform("LinMAD", LinMAD);
        SSAO->PushUniform("Resolution", glm::vec2(SSAOwidth, SSAOheight));
    }

    switch (this->mHBAOQuality)
    {
    case 0:
        SSAO->PushUniform("NumDirections", 3);
        SSAO->PushUniform("NumSamples", 4);

    case 1:
        SSAO->PushUniform("NumDirections", 5);
        SSAO->PushUniform("NumSamples", 6);

    case 2:
        SSAO->PushUniform("NumDirections", 6);
        SSAO->PushUniform("NumSamples", 8);
    }

    glUniform3fv(glGetUniformLocation(SSAO->getProgramID(), "samples"), 9, &ssaoKernel[0][0]);

    SSAO->PushUniform("projection", cam->getProjectionMatrix());
    SSAO->PushUniform("invprojection", glm::inverse(cam->getProjectionMatrix()));
    SSAO->PushUniform("view", cam->getViewMatrix());
    SSAO->PushUniform("invView", glm::inverse(cam->getViewMatrix()));
    glViewport(0, 0, SSAOwidth, SSAOheight);
    this->RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //#define cheapblur

#ifndef cheapblur
    glViewport(0, 0, width, height);

    ssaoColorBufferBlur = this->blurImage(ssaoColorBuffer, true);
#else

    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    blurSSAO->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    this->RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif // cheapblur

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(0, 0, width, height);
}

GLuint PostProcess::blurImage(GLuint Buffer, bool cheap = false)
{

    GLboolean horizontal = true, first_iteration = true, direction = true;
    GLuint amount = 5;
    blurBloom->Use();

    for (unsigned int i = 0; i < amount; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glViewport(0, 0, 426, 240);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glUniform1i(glGetUniformLocation(blurBloom->getProgramID(), "horizontal"), horizontal);
        glUniform1i(glGetUniformLocation(blurBloom->getProgramID(), "cheap"), cheap);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? Buffer : pingpongColorbuffers[!horizontal]); // bind texture of other framebuffer (or scene if first iteration)
        RenderQuad();

        if (i >= 4)
            direction = !direction;

        horizontal = !horizontal;

        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0, 0, width, height);

    return pingpongColorbuffers[horizontal];
}

GLfloat PostProcess::applyAutoAxposure(GLuint Buffer)
{
    /*
    	GLboolean horizontal = true, first_iteration = true;
    	blurBloom->Use();

    	float Ewidth = 640;
    	float Eheight = 480;

    	while(Ewidth > 1) {
    		glBindFramebuffer(GL_FRAMEBUFFER, pingpongDOF[horizontal]);
    		Ewidth /= 2;
    		Eheight /= 2;
    		glViewport(0,0,Ewidth, Eheight);
    		glUniform1i(glGetUniformLocation(blurBloom->getProgramID(), "horizontal"), horizontal);
    		glBindTexture(GL_TEXTURE_2D, first_iteration ? Buffer : pingpongColorbuffersDOF[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
    		RenderQuad();
    		horizontal = !horizontal;
    		if (first_iteration)
    			first_iteration = false;

    	}
    	// Once done read the luminescence value of 1x1 texture
    	GLfloat luminescence[3];
    	//glReadPixels(0, 0, 1, 1, GL_RGB, GL_FLOAT, &luminescence);
    	glm::vec3 rgbColor = glm::vec3(luminescence[0], luminescence[1], luminescence[2]);
    	rgbColor = rgbColor * glm::vec3(0.2126, 0.7152, 0.0722);
    	float lum = rgbColor.x + rgbColor.y + rgbColor.z;
    	m_exposure = lerp(m_exposure, 2.0 / lum, 0.005); // slowly adjust exposure based on average brightness


    	glBindFramebuffer(GL_FRAMEBUFFER, 0);

    	glViewport(0,0,width, height);
    */
    return 3.5;
}

void PostProcess::SetupPingPongFBO()
{
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 426, 240, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
        // Also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcess::SetupMotionBlur()
{
    glGenFramebuffers(1, &MotionBlurFBO);
    glGenTextures(1, &MotionBlurBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, MotionBlurFBO);
    glBindTexture(GL_TEXTURE_2D, MotionBlurBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, MotionBlurBuffer, 0);
    // Also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcess::SetupPingPongDOF()
{
    glGenFramebuffers(2, pingpongDOF);
    glGenTextures(2, pingpongColorbuffersDOF);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongDOF[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffersDOF[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 854, 480, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffersDOF[i], 0);
        // Also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcess::setupSSR()
{
    glGenFramebuffers(2, SSRFBO);
    glGenTextures(2, SSRTexture);
    for (int i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, SSRFBO[i]);
        glBindTexture(GL_TEXTURE_2D, SSRTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSRTexture[i], 0);
    }
    // Also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //setupPingPongSSR();
    setupDownSampledSSR();
}

void PostProcess::setupDenoise()
{
    glGenFramebuffers(1, &DenoiseFBO);
    glGenTextures(1, &DenoiseTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, DenoiseFBO);
    glBindTexture(GL_TEXTURE_2D, DenoiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGB, GL_FLOAT, NULL);
    //glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DenoiseTexture, 0);
    // Also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::setupPingPongSSR()
{
    glGenFramebuffers(2, pingpongSSRFBO);
    glGenTextures(2, pingpongSSRT);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongSSRFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongSSRT[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 854, 480, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongSSRT[i], 0);
        // Also check if framebuffers are complete (no need for depth buffer)
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
    }
}

void PostProcess::setupDownSampledSSR()
{
    glGenFramebuffers(1, &DownSamplerFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, DownSamplerFBO);
    glGenTextures(1, &this->DownSampledTexture);

    glBindTexture(GL_TEXTURE_2D, DownSampledTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DownSampledTexture, 0);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum DrawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    // Also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::SSRPass(std::shared_ptr<Camera> &cam)
{

    glBindFramebuffer(GL_FRAMEBUFFER, SSRFBO[this->CurrentSSR]);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_BLEND);
    ScreenSpaceReflectionShader->Use();
    glViewport(0, 0, this->width, this->height);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gFinalImage"), 0);
    glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("colorBuffer"));
    //glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gNormal"), 1);
    glBindTexture(GL_TEXTURE_2D, this->gExpensiveNormal);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gExtraComponents"), 2);
    glBindTexture(GL_TEXTURE_2D, this->gExtraComponents);

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gDepth"), 3);
    glBindTexture(GL_TEXTURE_2D, this->gDepth);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "BRDF"), 4);
    glBindTexture(GL_TEXTURE_2D, BRDFFramebuffer->getRenderTargetHandler(0));

    glActiveTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "noiseTexture"), 5);
    glBindTexture(GL_TEXTURE_2D, /*this->noiseTexture*/ this->BlueNoiseTexture->getTextureID());

    glActiveTexture(GL_TEXTURE6);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "PreviousReflection"), 6);
    glBindTexture(GL_TEXTURE_2D, this->SSRTexture[!this->CurrentSSR]);

    glActiveTexture(GL_TEXTURE7);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gAlbedo"), 7);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

    glm::mat4 proj = cam->getProjectionMatrix();
    glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "projection"), 1, GL_FALSE, &proj[0][0]);
    glm::mat4 invproj = glm::inverse(proj);
    glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "invprojection"), 1, GL_FALSE, &invproj[0][0]);
    glm::mat4 invView = glm::inverse(cam->getViewMatrix());
    glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "invView"), 1, GL_FALSE, &invView[0][0]);
    glm::mat4 view = cam->getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
    glUniform2f(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "Resolution"), this->width, this->height);
    glUniform3fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "camDir"), 1, &cam->getDirection()[0]);
    glUniform3fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "camPos"), 1, &cam->getPosition()[0]);
    ScreenSpaceReflectionShader->PushUniform("SSROn", SSROn);
    ScreenSpaceReflectionShader->PushUniform("LinMAD", LinMAD);
    ScreenSpaceReflectionShader->PushUniform("Time", (float)glfwGetTime());
    ScreenSpaceReflectionShader->PushUniform("ID", TotalFrames);
    ScreenSpaceReflectionShader->PushUniform("isMoving", cam->isMoving());
    this->RenderQuad();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 3);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 4);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 5);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 6);

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /** Denoise SSR*/

    if (cam->isMoving())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, DenoiseFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        DenoiseShader->Use();
        glViewport(0, 0, this->width, this->height);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(DenoiseShader->getProgramID(), "texture0"), 0);
        glBindTexture(GL_TEXTURE_2D, SSRTexture[this->CurrentSSR]);
        glGenerateMipmap(GL_TEXTURE_2D);

        glUniform2f(glGetUniformLocation(DenoiseShader->getProgramID(), "resolution"), this->width, this->height);
        glUniform1f(glGetUniformLocation(DenoiseShader->getProgramID(), "exponent"), 0.05);
        this->RenderQuad();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glEnable(GL_BLEND);
        glViewport(0, 0, this->width, this->height);
    }
    this->CurrentSSR = !this->CurrentSSR;

    /*****************/
}

void PostProcess::MotionBlur(float frametime)
{
    glBindFramebuffer(GL_FRAMEBUFFER, MotionBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    MotionBlurShader->Use();
    glViewport(0, 0, this->width, this->height);

    glActiveTexture(GL_TEXTURE0);
    MotionBlurShader->PushUniform("gFinalImage", 0);
    glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("colorBuffer"));
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    MotionBlurShader->PushUniform("gExtraComponents", 1);
    glBindTexture(GL_TEXTURE_2D, this->gExtraComponents);

    float currentFPS = (1.0 / frametime);
    float targetFPS = (1.0 / 0.016f);
    float velocityScale = currentFPS / targetFPS;
    MotionBlurShader->PushUniform("uVelocityScale", velocityScale * this->mMotionBlurStrength);

    this->RenderQuad();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    MotionBlurShader->Free();
}

void PostProcess::DownSampleSSR(double frametime)
{
    /*
    glBindFramebuffer(GL_FRAMEBUFFER, DownSamplerFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 0.0f, 1.0f);

    PassThroughShader->Use();
    glViewport(0,0, 1, 1);
    glBindTexture(GL_TEXTURE_2D, mCompositeImage->getRenderTargetHandler(0));
    glGenerateMipmap(GL_TEXTURE_2D);
    this->RenderQuad();

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    PassThroughShader->Free();

    glBindFramebuffer(GL_FRAMEBUFFER, DownSamplerFBO);

    GLfloat luminescence[3];
    glReadPixels(0, 0, 1, 1, GL_RGB, GL_FLOAT, &luminescence);
    GLfloat lum = 0.2126 * luminescence[0] + 0.7152 * luminescence[1] + 0.0722 * luminescence[2];
    if((0.07/lum) > m_exposure)
    	m_exposure += frametime * 2.0f;
    else if((0.07/lum) < m_exposure)
    	m_exposure -= frametime * 2.0f;
    else {}

    m_exposure = glm::clamp(m_exposure, 7.0f, 12.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */
    m_exposure = 10.0f;
    // std::cout << m_exposure << std::endl;
}

GLuint PostProcess::blurSSR(GLuint Buffer)
{

    GLboolean horizontal = true, first_iteration = true, direction = true;
    GLuint amount = 15;
    blurSSRShader->Use();

    for (unsigned int i = 0; i < amount; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongSSRFBO[horizontal]);
        glViewport(0, 0, 854, 480);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glUniform1i(glGetUniformLocation(blurSSRShader->getProgramID(), "horizontal"), horizontal);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(blurSSRShader->getProgramID(), "image"), 0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? this->DownSampledTexture : pingpongSSRT[!horizontal]); // bind texture of other framebuffer (or scene if first iteration)
        RenderQuad();

        if (i >= 4)
            direction = !direction;

        horizontal = !horizontal;

        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0, 0, width, height);

    return pingpongSSRT[horizontal];
}

GLuint PostProcess::GetPixel(GLuint tex)
{
    return 0;
}

void PostProcess::CompositeImage(bool isMoving)
{

    if (isMoving)
        TotalFrames = 0;
    else
        TotalFrames++;

    mCompositeImage->bindFramebuffer();
    mCompositeImage->setViewport();
    mCompositeImage->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    CompositeShader->Use();

    glActiveTexture(GL_TEXTURE0);
    CompositeShader->PushUniform("gColorSampler", 0);
    if (m_MotionBlur)
        glBindTexture(GL_TEXTURE_2D, this->MotionBlurBuffer);
    else
        glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("colorBuffer"));
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    CompositeShader->PushUniform("gReflectionSampler", 1);
    if (isMoving)
        glBindTexture(GL_TEXTURE_2D, SSRTexture[!this->CurrentSSR]);
    else
    {
        glBindTexture(GL_TEXTURE_2D, SSRTexture[this->CurrentSSR]);
        if (TotalFrames >= 250)
            glBindTexture(GL_TEXTURE_2D, SSRTexture[!this->CurrentSSR]);
    }

    glActiveTexture(GL_TEXTURE2);
    CompositeShader->PushUniform("ssaoColorBufferBlur", 2);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

    CompositeShader->PushUniform("TotalFrames", TotalFrames);
    CompositeShader->PushUniform("isMoving", isMoving);
    CompositeShader->PushUniform("HBAOOn", (int)HBAOOn);

    this->RenderQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
    mCompositeImage->unbindFramebuffer();
}

void PostProcess::ShowPostProcessImage(float frametime, GLuint onmenu, glm::vec3 Sun, std::shared_ptr<Camera> &cam)
{

    //if(SSROn) {
    SSRPass(cam);
    //}

    if (m_MotionBlur)
        MotionBlur(frametime);

    this->CompositeImage(cam->isMoving());

    GLuint blurred = this->blurImage(hdrFBO->getRenderTargetHandler("brightColorBuffer"), false);

    DownSampleSSR(frametime);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, this->width, this->height);
    finalImage->Use();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (Input::KeyBoard::KEYS[Input::GLFW::Key::ARROW_UP])
        blurSize += 0.01;

    if (Input::KeyBoard::KEYS[Input::GLFW::Key::ARROW_DOWN])
        blurSize -= 0.01;

    glActiveTexture(GL_TEXTURE0);
    finalImage->PushUniform("compositeImage", 0);
    glBindTexture(GL_TEXTURE_2D, mCompositeImage->getRenderTargetHandler(0));
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    finalImage->PushUniform("blurredSampler", 1);
    glBindTexture(GL_TEXTURE_2D, blurred);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE2);
    finalImage->PushUniform("uLensColor", 2);
    glBindTexture(GL_TEXTURE_1D, lensColor->getTextureID());

    glActiveTexture(GL_TEXTURE3);
    finalImage->PushUniform("uLensDirtTex", 3);
    glBindTexture(GL_TEXTURE_2D, lensDirt->getTextureID());

    glActiveTexture(GL_TEXTURE4);
    finalImage->PushUniform("uLensStarTex", 4);
    glBindTexture(GL_TEXTURE_2D, lensStar->getTextureID());

    glActiveTexture(GL_TEXTURE5);
    finalImage->PushUniform("brightColorBuffer", 5);
    glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("brightColorBuffer"));

    glActiveTexture(GL_TEXTURE6);
    finalImage->PushUniform("gDepth", 6);
    glBindTexture(GL_TEXTURE_2D, gDepth);

    glm::mat4 choppedView = glm::mat4(glm::mat3(cam->getViewMatrix()));
    finalImage->PushUniform("choppedView", choppedView);
    finalImage->PushUniform("projection", cam->getProjectionMatrix());
    finalImage->PushUniform("position", cam->getPosition());
    finalImage->PushUniform("sunPos", Sun);
    finalImage->PushUniform("blurSize", blurSize);
    finalImage->PushUniform("lightShafts", this->lightShafts);
    finalImage->PushUniform("onmenu", (int)onmenu);
    finalImage->PushUniform("Resolution", glm::vec2(this->width, this->height));

    finalImage->PushUniform("BokehOn", this->mBokehDOF);
    finalImage->PushUniform("ChromaticAberration", this->mChromaticAberration);

    finalImage->PushUniform("exposure", m_exposure);
    finalImage->PushUniform("time", (float)glfwGetTime());

    this->RenderQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcess::ShowFrame(glm::vec3 Sun, bool &hdr, std::shared_ptr<Camera> &cam, float exposure, std::unique_ptr<ShadowMap> &shadowMap)
{

    hdrFBO->bindFramebuffer();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->shader->Use();

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glViewport(0, 0, this->width, this->height);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gDepth"), 0);
    glBindTexture(GL_TEXTURE_2D, this->gDepth);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gNormal"), 1);
    glBindTexture(GL_TEXTURE_2D, this->gExpensiveNormal);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gAlbedoSpec"), 2);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "ssaoColorBufferBlur"), 3);
    glBindTexture(GL_TEXTURE_2D, this->ssaoColorBufferBlur);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "shadowMap"), 4);
    glBindTexture(GL_TEXTURE_2D, shadowMap->getShadowTextureID());

    glActiveTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gExtraComponents"), 5);
    glBindTexture(GL_TEXTURE_2D, this->gExtraComponents);

    glActiveTexture(GL_TEXTURE6);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gLightAccumulation"), 6);
    glBindTexture(GL_TEXTURE_2D, this->gLightAccumulation);

    glUniform1i(glGetUniformLocation(this->shader->getProgramID(), "hdr"), hdr);
    glUniform1f(glGetUniformLocation(this->shader->getProgramID(), "time"), glfwGetTime());
    glUniform1f(glGetUniformLocation(this->shader->getProgramID(), "exposure"), exposure);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "depthBias"), 1, GL_FALSE, &shadowMap->getBiasMatrix()[0][0]);

    glm::mat4 invProj = glm::inverse(cam->getProjectionMatrix());
    glm::mat4 invView = glm::inverse(cam->getViewMatrix());
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, &invProj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "view"), 1, GL_FALSE, &invView[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "uView"), 1, GL_FALSE, &cam->getViewMatrix()[0][0]);
    //glUniform3fv(glGetUniformLocation(shader->getProgramID(), "LightPositions"), LightPositions.size(), &LightPositions[0][0]);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "NUMLIGHTS"), m_Lights.size());
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "viewPos"), cam->getPosition().x, cam->getPosition().y, cam->getPosition().z);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "uViewDir"), cam->getDirection().x, cam->getDirection().y, cam->getDirection().z);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "lightDir"), Sun.x, Sun.y, Sun.z);

    this->RenderQuad();

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    glCache::glUseProgram(0);

    /** copy texture */

    CopyTextureFBO->bindFramebuffer();
    CopyTextureFBO->setViewport();
    CopyTextureFBO->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PassThroughShader->Use();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(PassThroughShader->getProgramID(), "texture0"), 0);
    glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("colorBuffer"));
    glGenerateMipmap(GL_TEXTURE_2D);

    CopyTextureFBO->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->RenderQuad();

    PassThroughShader->Free();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);

    CopyTextureFBO->unbindFramebuffer();
    glViewport(0, 0, this->width, this->height);

    /** end copy texture*/

    /**Fill mip maps begin*/
    CopyTextureBlurredFBO->bindFramebuffer();
    blurBloom->Use();
	unsigned int maxMipLevels = 5;
    for (int mip = 0; mip < maxMipLevels; mip++)
    {
        unsigned int mipWidth = CopyTextureBlurredFBO->WIDTH * std::pow(0.5, mip);
        unsigned int mipHeight = CopyTextureBlurredFBO->HEIGHT * std::pow(0.5, mip);
        CopyTextureBlurredFBO->setViewport(mipWidth, mipHeight);

        blurBloom->PushUniform("cheap", 0);
        blurBloom->PushUniform("horizontal", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, CopyTextureFBO->getRenderTargetHandler(0));
        blurBloom->PushUniform("image", 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CopyTextureBlurredFBO->getRenderTargetHandler(0), mip);

        CopyTextureBlurredFBO->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        this->RenderQuad();
    }
    blurBloom->Free();
    CopyTextureBlurredFBO->unbindFramebuffer();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(0, 0, this->width, this->height);

    /**Fill mip maps end*/

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    hdrFBO->setToDraw();
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    hdrFBO->bindFramebuffer();
}

float PostProcess::lerp(float v0, float v1, float t)
{
    return (1.0f - t) * v0 + t * v1;
}

void PostProcess::RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] = {
            // Positions         //Texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f};
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glCache::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
