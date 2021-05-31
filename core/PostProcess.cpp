#include <pch.hpp>
#include <Core.hpp>
#include <PostProcess.h>
#include <ProgramData.h>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Helpers.hpp>
#include <IO/KeyBoard.h>
#include <Driver/API/OpenGL/HelperFunctions/CheckError.h>

namespace Epsilon
{
    PostProcess::PostProcess()
    {
        LoadOffscreensShaders();
        SetupFramebuffer();
        lastDepth = 0.2;
    }

    static int TotalFrames = 0;

    void PostProcess::SetupFramebuffer()
    {
        ProgramData PG;
        width = PG.WINDOW_WIDTH;
        height = PG.WINDOW_HEIGHT;
        SSROn = PG.SCREEN_SPACE_REFLECTIONS;
        SSAOwidth = width;
        SSAOheight = height;
        lightShafts = PG.LIGHTSHAFTS;
        m_MotionBlur = PG.MOTION_BLUR;
        HBAOOn = PG.HBAO;
 
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
        /*
        tmpLight.position = glm::vec4(20, 10, 22, 1.0); //-1.29, 5.64, 25.7
        tmpLight.direction = glm::vec4(0, 0, 0, 1.0);  //5, 15, 5
        tmpLight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        tmpLight.radius = 2.0;
        tmpLight.watts = 500.0f;
        tmpLight.type = 2;
        m_Lights.push_back(tmpLight);
        */
        tmpLight.position = glm::vec4(10, 700.5, 6, 1.0);
        tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
        tmpLight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        tmpLight.radius = 1.0f;
        tmpLight.watts = 600.0f;
        tmpLight.type = 2;
        m_Lights.push_back(tmpLight);
/*
        tmpLight.position = glm::vec4(-20, 10, 2, 1.0);
        tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
        tmpLight.color = glm::vec4(1.0, 1.0, 1.0, 1.0);
        tmpLight.radius = 2.0f;
        tmpLight.watts = 700.0f;
        tmpLight.type = 2;
        m_Lights.push_back(tmpLight);*/
        /*
    tmpLight.position = glm::vec4(-14.4, 8.5, -2.81, 1.0);
    tmpLight.direction = glm::vec4(0, 0, 0, 1.0);
    tmpLight.color = glm::vec4(1.0, 0.6, 0.6, 1.0);
    tmpLight.radius = 0.5f;
    tmpLight.watts = 200.0f;
    tmpLight.type = 2;
    m_Lights.push_back(tmpLight);
/*
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

        lensColor = std::make_shared<eTexture>("effects/lenscolor2.png", GL_REPEAT, GL_TEXTURE_1D);
        lensDirt = std::make_shared<eTexture>("effects/lensdirt.png");
        lensStar = std::make_shared<eTexture>("effects/lensstar.png");

        BlueNoiseTexture = std::make_shared<eTexture>("LDR_RGBA_0_n.png", GL_REPEAT, GL_TEXTURE_2D, GL_LINEAR);
        /*
                LightPositions.push_back(glm::vec3(-41, 12.0, -23));
                LightPositions.push_back(glm::vec3(-39, 10.3, 2));
                LightPositions.push_back(glm::vec3(17.2, 17.3, -17));
                LightPositions.push_back(glm::vec3(17, 17.3, 27));
                LightPositions.push_back(glm::vec3(122.7, 13.63, 5.3));
                LightPositions.push_back(glm::vec3(-14, 16.63, 32.3));
                LightPositions.push_back(glm::vec3(80.5, 17.21, 1.57));
    */

        hdrFBO = std::make_shared<OpenGL::FrameBuffer<std::string>>(width, height, true);

        hdrFBO->addRenderTarget("colorBuffer", GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, false);
        hdrFBO->addRenderTarget("brightColorBuffer", GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        hdrFBO->FinishFrameBuffer();

        CopyTextureFBO = std::make_shared<OpenGL::FrameBuffer<int>>(width, height, false);
        CopyTextureFBO->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        CopyTextureFBO->FinishFrameBuffer();

        CopyTextureBlurredFBO = std::make_shared<OpenGL::FrameBuffer<int>>(width, height, true);
        CopyTextureBlurredFBO->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
        CopyTextureBlurredFBO->FinishFrameBuffer();

        SetupGBuffer();
        setupSSAO();
        setupHBIL();
        SetupPingPongFBO();
        setupSSR();
        SetupMotionBlur();
        setupDenoise();

        mCompositeImage = std::make_shared<OpenGL::FrameBuffer<int>>(width, height, true);
        mCompositeImage->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR, true);
        mCompositeImage->FinishFrameBuffer();
        
        mTAAFramebuffer[0] = std::make_shared<OpenGL::FrameBuffer<int>>(width, height, false);
        mTAAFramebuffer[0]->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        mTAAFramebuffer[0]->FinishFrameBuffer();
        
        mTAAFramebuffer[1] = std::make_shared<OpenGL::FrameBuffer<int>>(width, height, false);
        mTAAFramebuffer[1]->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        mTAAFramebuffer[1]->FinishFrameBuffer();
        
        mTAAFramebufferCopy = std::make_shared<OpenGL::FrameBuffer<int>>(width, height, false);
        mTAAFramebufferCopy->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        mTAAFramebufferCopy->FinishFrameBuffer();

        BRDFFramebuffer = std::make_shared<OpenGL::FrameBuffer<int>>(512, 512, true);
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
        RenderQuad();
        glEnable(GL_BLEND);

        BRDFFramebuffer->unbindFramebuffer();
        BRDFShader->Free();
        glViewport(0, 0, width, height);
    }

    void PostProcess::LoadOffscreensShaders()
    {
        shader = (std::shared_ptr<Shader>)(new Shader("shaders/Lighting.vglsl", "shaders/Lighting.fglsl"));
        SSAO = (std::shared_ptr<Shader>)(new Shader("shaders/SSAO.vglsl", "shaders/HBAO.glsl"));
        HBIL = (std::shared_ptr<Shader>)(new Shader("shaders/SSAO.vglsl", "shaders/HBIL.glsl"));
        blurSSAO = (std::shared_ptr<Shader>)(new Shader("shaders/blurSSAO.vglsl", "shaders/blurSSAO.fglsl"));
        finalImage = (std::shared_ptr<Shader>)(new Shader("shaders/hdr.vglsl", "shaders/hdr.fglsl"));
        blurBloom = (std::shared_ptr<Shader>)(new Shader("shaders/blurBloom.vglsl", "shaders/blurBloom.fglsl"));
        blurSSRShader = (std::shared_ptr<Shader>)(new Shader("shaders/blurSSR.vglsl", "shaders/blurSSR.fglsl"));
        ScreenSpaceReflectionShader = (std::shared_ptr<Shader>)(new Shader("shaders/SSR.vglsl", "shaders/SSR.glsl"));
        PassThroughShader = (std::shared_ptr<Shader>)(new Shader("shaders/PassThrough.vglsl", "shaders/PassThrough.fglsl"));
        MotionBlurShader = (std::shared_ptr<Shader>)(new Shader("shaders/MotionBlur.vglsl", "shaders/MotionBlur.fglsl"));
        CompositeShader = (std::shared_ptr<Shader>)(new Shader("shaders/Composite.vglsl", "shaders/Composite.fglsl"));
        BRDFShader = (std::shared_ptr<Shader>)(new Shader("shaders/Composite.vglsl", "shaders/BRDF.glsl"));
        DenoiseShader = (std::shared_ptr<Shader>)(new Shader("shaders/Composite.vglsl", "shaders/denoise.glsl"));
        TAAShader = (std::shared_ptr<Shader>)(new Shader("shaders/Composite.vglsl", "shaders/TAA.glsl"));
    }

    void PostProcess::beginOffScreenrendering()
    {
        glDisable(GL_BLEND);
        gBufferFramebuffer->bindFramebuffer();
        gBufferFramebuffer->setViewport();
        //glViewport(0, 0, width, height);
        //glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        gBufferFramebuffer->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    }

    void PostProcess::endOffScreenRendering()
    {
        gBufferFramebuffer->unbindFramebuffer();
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PostProcess::SetupGBuffer()
    {
        gBufferFramebuffer = std::make_shared<OpenGL::FrameBuffer<int>>(width, height, true);
        gBufferFramebuffer->addRenderTarget(GBUFFER_ALBEDO_SPEC, GL_RGBA, GL_RGBA, GL_LINEAR, GL_LINEAR, false); //gAlbedoSpec
        gBufferFramebuffer->addRenderTarget(GBUFFER_NORMAL, GL_RGBA16F, GL_RGBA, GL_LINEAR, GL_LINEAR, false); //gExpensiveNormal
        gBufferFramebuffer->addRenderTarget(GBUFFER_IBL_DEPTH, GL_RGBA32F, GL_RGBA, GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST, true); //gDepth
        gBufferFramebuffer->addRenderTarget(GBUFFER_MOTION_EXTRA, GL_RGBA32F, GL_RGBA, GL_NEAREST, GL_NEAREST, false); //gExtraComponents
        gBufferFramebuffer->addRenderTarget(GBUFFER_GI, GL_RGBA32F, GL_RGBA, GL_NEAREST, GL_NEAREST, false); //gLightAccumulation
        gBufferFramebuffer->FinishFrameBuffer();
    }

    void PostProcess::setupSSAO()
    {

        mHBAOFramebuffer = std::make_shared<OpenGL::FrameBuffer<int>>(SSAOwidth, SSAOheight, false);
        mHBAOFramebuffer->addRenderTarget(0, GL_RED, GL_RGB, GL_LINEAR, GL_LINEAR, false); //gAlbedoSpec
        mHBAOFramebuffer->FinishFrameBuffer();
        
        mHBAOBlurFramebuffer = std::make_shared<OpenGL::FrameBuffer<int>>(SSAOwidth, SSAOheight, false);
        mHBAOBlurFramebuffer->addRenderTarget(0, GL_RED, GL_RGB, GL_LINEAR, GL_LINEAR, false); //gAlbedoSpec
        mHBAOBlurFramebuffer->FinishFrameBuffer();

        // Sample kernel
        std::uniform_real_distribution<GLfloat> randomFloatsClamped(0.0, 1.0); // generates random floats between 0.0 and 1.0
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
        for (GLuint i = 0; i < 16 * 3; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }

        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void PostProcess::setupHBIL()
    {

        HBILFramebuffer = (std::shared_ptr<OpenGL::FrameBuffer<int>>)new OpenGL::FrameBuffer<int>(SSAOwidth, SSAOheight, true);
        HBILFramebuffer->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        HBILFramebuffer->FinishFrameBuffer();
    }

    void PostProcess::applyHBIL(std::shared_ptr<Camera> &cam)
    {

        HBILFramebuffer->bindFramebuffer();
        HBILFramebuffer->setViewport();
        HBILFramebuffer->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        HBIL->Use();

        glActiveTexture(GL_TEXTURE0);
        HBIL->PushUniform("gDepth", 0);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_IBL_DEPTH));
        glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        HBIL->PushUniform("gNormal", 1);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_NORMAL));

        glActiveTexture(GL_TEXTURE2);
        HBIL->PushUniform("texNoise", 2);
        glBindTexture(GL_TEXTURE_2D, BlueNoiseTexture->getTextureID());

        glActiveTexture(GL_TEXTURE3);
        HBIL->PushUniform("gAlbedo", 3);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_ALBEDO_SPEC));

        HBIL->PushUniform("ViewMatrix", cam->getViewMatrix());
        glUniform3fv(glGetUniformLocation(HBIL->getProgramID(), "samples"), 9, &ssaoKernel[0][0]);
        
        glm::mat4 proj = cam->getProjectionMatrix();
        glm::mat4 invproj = glm::inverse(proj);
        glUniformMatrix4fv(glGetUniformLocation(HBIL->getProgramID(), "invprojection"), 1, GL_FALSE, &invproj[0][0]);

        HBIL->PushUniform("FocalLen", FocalLen);
        HBIL->PushUniform("UVToViewA", UVToViewA);
        HBIL->PushUniform("UVToViewB", UVToViewB);
        HBIL->PushUniform("LinMAD", LinMAD);
        HBIL->PushUniform("Resolution", glm::vec2(SSAOwidth, SSAOheight));

        HBIL->PushUniform("projection", cam->getProjectionMatrix());
        HBIL->PushUniform("invprojection", glm::inverse(cam->getProjectionMatrix()));
        HBIL->PushUniform("view", cam->getViewMatrix());
        HBIL->PushUniform("invView", glm::inverse(cam->getViewMatrix()));
        HBIL->PushUniform("viewPos", cam->getPosition());
        HBIL->PushUniform("iTime", (float)glfwGetTime());

        RenderQuad();

        HBILFramebuffer->unbindFramebuffer();
        HBIL->Free();
    }

    void PostProcess::applySSAO(std::shared_ptr<Camera> &cam)
    {
        //DownSampleSSR();
        //glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        mHBAOFramebuffer->bindFramebuffer();
        mHBAOFramebuffer->setViewport();
        mHBAOFramebuffer->clearBuffer(GL_COLOR_BUFFER_BIT);
        SSAO->Use();
        glActiveTexture(GL_TEXTURE0);
        SSAO->PushUniform("gDepth", 0);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_IBL_DEPTH));
        glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        SSAO->PushUniform("texNoise", 1);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);

        glActiveTexture(GL_TEXTURE2);
        SSAO->PushUniform("gNormal", 2);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_NORMAL));

        if (!HBAO_params_calculated || mRecalculateSSAO)
        {
            HBAO_params_calculated = true;
            FocalLen.x = 1.0f / tanf(glm::radians(cam->getFoV()) * 0.5f) * ((float)width / (float)height);
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
            mRecalculateSSAO = false;
        }

        SSAO->PushUniform("FocalLen", FocalLen);
        SSAO->PushUniform("UVToViewA", UVToViewA);
        SSAO->PushUniform("UVToViewB", UVToViewB);
        SSAO->PushUniform("LinMAD", LinMAD);
        SSAO->PushUniform("Resolution", glm::vec2(width, height));
/*
        switch (mHBAOQuality)
        {
        case 0:
            SSAO->PushUniform("NumDirections", 3);
            SSAO->PushUniform("NumSamples", 4);

        case 1:
            SSAO->PushUniform("NumDirections", 5);
            SSAO->PushUniform("NumSamples", 6);

        case 2:
            SSAO->PushUniform("NumDirections", 7);
            SSAO->PushUniform("NumSamples", 8);
        }*/
        
        SSAO->PushUniform("NumDirections", mPostProcessData.HBAO.NumDirections);
        SSAO->PushUniform("NumSamples", mPostProcessData.HBAO.Samples);

        glUniform3fv(glGetUniformLocation(SSAO->getProgramID(), "samples"), 9, &ssaoKernel[0][0]);

        SSAO->PushUniform("projection", cam->getProjectionMatrix());
        SSAO->PushUniform("invprojection", glm::inverse(cam->getProjectionMatrix()));
        SSAO->PushUniform("view", cam->getViewMatrix());
        SSAO->PushUniform("invView", glm::inverse(cam->getViewMatrix()));
        SSAO->PushUniform("viewPos", cam->getPosition());
        //glViewport(0, 0, width, height);
        RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //#define cheapblur

#ifndef cheapblur
        //glViewport(0, 0, width, height);

        ssaoColorBufferBlur = blurImage(mHBAOFramebuffer->getRenderTargetHandler(0), false);
#else
        mHBAOBlurFramebuffer->bindFramebuffer();
        mHBAOBlurFramebuffer->setViewport();
        mHBAOBlurFramebuffer->clearBuffer(GL_COLOR_BUFFER_BIT);
        blurSSAO->Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mHBAOFramebuffer->getRenderTargetHandler(0));
        RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif // cheapblur

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        //glViewport(0, 0, width, height);
    }

    GLuint PostProcess::blurImage(GLuint Buffer, bool cheap = false)
    {

        GLboolean horizontal = true, first_iteration = true, direction = true;
        GLuint amount = 5;
        blurBloom->Use();

        for (unsigned int i = 0; i < amount; ++i)
        {
            //glBindFramebuffer(GL_FRAMEBUFFER, mPingPongFramebuffer[horizontal]->getRenderTargetHandler(0));
            mPingPongFramebuffer[horizontal]->bindFramebuffer();
            mPingPongFramebuffer[horizontal]->setViewport();
            //glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glUniform1i(glGetUniformLocation(blurBloom->getProgramID(), "horizontal"), horizontal);
            glUniform1i(glGetUniformLocation(blurBloom->getProgramID(), "cheap"), cheap);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, first_iteration ? Buffer : mPingPongFramebuffer[!horizontal]->getRenderTargetHandler(0)); // bind texture of other framebuffer (or scene if first iteration)
            RenderQuad();

            if (i >= 4)
                direction = !direction;

            horizontal = !horizontal;

            if (first_iteration)
                first_iteration = false;
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glViewport(0, 0, width, height);

        return mPingPongFramebuffer[horizontal]->getRenderTargetHandler(0);
    }

    GLfloat PostProcess::applyAutoAxposure(GLuint Buffer)
    {
        return 3.5;
    }

    void PostProcess::SetupPingPongFBO()
    {
        mPingPongFramebuffer[0] = (std::shared_ptr<OpenGL::FrameBuffer<int>>)new OpenGL::FrameBuffer<int>(width, height, true);
        mPingPongFramebuffer[0]->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        mPingPongFramebuffer[0]->FinishFrameBuffer();
        
        mPingPongFramebuffer[1] = (std::shared_ptr<OpenGL::FrameBuffer<int>>)new OpenGL::FrameBuffer<int>(width, height, true);
        mPingPongFramebuffer[1]->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        mPingPongFramebuffer[1]->FinishFrameBuffer();

    }

    void PostProcess::SetupMotionBlur()
    {
        mMotionBlurFramebuffer = (std::shared_ptr<OpenGL::FrameBuffer<int>>)new OpenGL::FrameBuffer<int>(width, height, false);
        mMotionBlurFramebuffer->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_LINEAR, GL_LINEAR, false);
        mMotionBlurFramebuffer->FinishFrameBuffer();
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
        
        mSSRFramebuffer[0] = (std::shared_ptr<OpenGL::FrameBuffer<int>>)new OpenGL::FrameBuffer<int>(width, height, false);
        mSSRFramebuffer[0]->addRenderTarget(0, GL_RGBA16F, GL_RGBA, GL_NEAREST, GL_NEAREST, false);
        mSSRFramebuffer[0]->FinishFrameBuffer();
        
        mSSRFramebuffer[1] = (std::shared_ptr<OpenGL::FrameBuffer<int>>)new OpenGL::FrameBuffer<int>(width, height, false);
        mSSRFramebuffer[1]->addRenderTarget(0, GL_RGBA16F, GL_RGBA, GL_NEAREST, GL_NEAREST, false);
        mSSRFramebuffer[1]->FinishFrameBuffer();

    }

    void PostProcess::setupDenoise()
    {

        mDenoiseFramebuffer = (std::shared_ptr<OpenGL::FrameBuffer<int>>)new OpenGL::FrameBuffer<int>(width, height, false);
        mDenoiseFramebuffer->addRenderTarget(0, GL_RGB16F, GL_RGB, GL_NEAREST, GL_NEAREST, false);
        mDenoiseFramebuffer->FinishFrameBuffer();
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
        glGenTextures(1, &DownSampledTexture);

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

        mSSRFramebuffer[CurrentSSR]->bindFramebuffer();
        mSSRFramebuffer[CurrentSSR]->setViewport();
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glDisable(GL_BLEND);
        ScreenSpaceReflectionShader->Use();
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gFinalImage"), 0);
        glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("colorBuffer"));
        //std::cout << hdrFBO->getRenderTargetHandler("colorBuffer") << std::endl;
        glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gNormal"), 1);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_NORMAL));

        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gExtraComponents"), 2);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_MOTION_EXTRA));

        glActiveTexture(GL_TEXTURE3);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gDepth"), 3);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_IBL_DEPTH));
        glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE4);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "BRDF"), 4);
        glBindTexture(GL_TEXTURE_2D, BRDFFramebuffer->getRenderTargetHandler(0));

        /*glActiveTexture(GL_TEXTURE5);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "noiseTexture"), 5);
        glBindTexture(GL_TEXTURE_2D, /*noiseTexture BlueNoiseTexture->getTextureID());
*/

        glActiveTexture(GL_TEXTURE5);
        ScreenSpaceReflectionShader->PushUniform("noiseTexture", 5);
        glBindTexture(GL_TEXTURE_2D, BlueNoiseTexture->getTextureID());

        glActiveTexture(GL_TEXTURE6);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "PreviousReflection"), 6);
        glBindTexture(GL_TEXTURE_2D, mSSRFramebuffer[!CurrentSSR]->getRenderTargetHandler(0));

        glActiveTexture(GL_TEXTURE7);
        glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gAlbedo"), 7);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_ALBEDO_SPEC));

        glm::mat4 proj = cam->getProjectionMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "projection"), 1, GL_FALSE, &proj[0][0]);
        glm::mat4 invproj = glm::inverse(proj);
        glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "invprojection"), 1, GL_FALSE, &invproj[0][0]);
        glm::mat4 invView = glm::inverse(cam->getViewMatrix());
        glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "invView"), 1, GL_FALSE, &invView[0][0]);
        glm::mat4 view = cam->getViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
        glUniform2f(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "Resolution"), width, height);
        glUniform3fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "camDir"), 1, &cam->getDirection()[0]);
        glUniform3fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "camPos"), 1, &cam->getPosition()[0]);
        ScreenSpaceReflectionShader->PushUniform("SSROn", mPostProcessData.ScreenSpaceReflections.Active);
        ScreenSpaceReflectionShader->PushUniform("LinMAD", LinMAD);
        ScreenSpaceReflectionShader->PushUniform("Time", (float)glfwGetTime());
        ScreenSpaceReflectionShader->PushUniform("ID", TotalFrames);
        ScreenSpaceReflectionShader->PushUniform("isMoving", cam->isMoving());

        
        ScreenSpaceReflectionShader->PushUniform("minRayStep", mPostProcessData.ScreenSpaceReflections.MinRayStep);
        ScreenSpaceReflectionShader->PushUniform("maxRayStep", mPostProcessData.ScreenSpaceReflections.MaxRayStep);
        ScreenSpaceReflectionShader->PushUniform("maxSteps", mPostProcessData.ScreenSpaceReflections.MaxSteps);
        ScreenSpaceReflectionShader->PushUniform("numBinarySearchSteps", mPostProcessData.ScreenSpaceReflections.MaxRefinementSteps);
        ScreenSpaceReflectionShader->PushUniform("roughnessCutoff", mPostProcessData.ScreenSpaceReflections.RoughnessCutoff);
        ScreenSpaceReflectionShader->PushUniform("depthCutoff", mPostProcessData.ScreenSpaceReflections.DepthCutoff);
  
        RenderQuad();
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
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);

        /** Denoise SSR*/

        //if (cam->isMoving())
        {
            mDenoiseFramebuffer->bindFramebuffer();
            mDenoiseFramebuffer->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            mDenoiseFramebuffer->setViewport();
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            DenoiseShader->Use();
            glActiveTexture(GL_TEXTURE0);
            glUniform1i(glGetUniformLocation(DenoiseShader->getProgramID(), "texture0"), 0);
            glBindTexture(GL_TEXTURE_2D, mSSRFramebuffer[CurrentSSR]->getRenderTargetHandler(0));

            glActiveTexture(GL_TEXTURE1);
            glUniform1i(glGetUniformLocation(DenoiseShader->getProgramID(), "roughnessTex"), 1);
            glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_NORMAL));
            //glGenerateMipmap(GL_TEXTURE_2D);

            glUniform2f(glGetUniformLocation(DenoiseShader->getProgramID(), "resolution"), width, height);
            glUniform1f(glGetUniformLocation(DenoiseShader->getProgramID(), "exponent"), 0.05);

            DenoiseShader->PushUniform("Sigma", mPostProcessData.DenoiseSettings.Sigma);
            DenoiseShader->PushUniform("kSigma", mPostProcessData.DenoiseSettings.kSigma);
            DenoiseShader->PushUniform("Threshold", mPostProcessData.DenoiseSettings.Threshold); 
            DenoiseShader->PushUniform("RoughnessCutoff", mPostProcessData.DenoiseSettings.RoughnessCutoff);
            RenderQuad();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glEnable(GL_BLEND);
            glViewport(0, 0, width, height);
        }
        CurrentSSR = !CurrentSSR;

        /*****************/
    }

    void PostProcess::MotionBlur(float frametime)
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, MotionBlurFBO);
        mMotionBlurFramebuffer->bindFramebuffer();
        mMotionBlurFramebuffer->setViewport();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        MotionBlurShader->Use();
        glViewport(0, 0, width, height);

        glActiveTexture(GL_TEXTURE0);
        MotionBlurShader->PushUniform("gFinalImage", 0);
        glBindTexture(GL_TEXTURE_2D, mTAAFramebuffer[TAACurrentBuffer]->getRenderTargetHandler(0) /* hdrFBO->getRenderTargetHandler("colorBuffer")*/);
        glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        MotionBlurShader->PushUniform("gExtraComponents", 1);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_MOTION_EXTRA));

        float currentFPS = (1.0 / frametime);
        float targetFPS = 1.0 / mPostProcessData.MotionBlur.FrameTimeTarget;
        float velocityScale = currentFPS / targetFPS;
        MotionBlurShader->PushUniform("uVelocityScale", velocityScale * mPostProcessData.MotionBlur.Strength);

        RenderQuad();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 1);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        MotionBlurShader->Free();
    }

    void PostProcess::DownSampleSSR(double frametime)
    {
        m_exposure = 10.0f;
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
            glBindTexture(GL_TEXTURE_2D, first_iteration ? DownSampledTexture : pingpongSSRT[!horizontal]); // bind texture of other framebuffer (or scene if first iteration)
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

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_1D, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (isMoving)
            TotalFrames = 0;
        else
            TotalFrames++;

        CompositeShader->Use();
        mCompositeImage->bindFramebuffer();
        mCompositeImage->setViewport();
        mCompositeImage->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        CompositeShader->PushUniform("gReflectionSampler", 0);

        if (isMoving)
        {
            glBindTexture(GL_TEXTURE_2D, mDenoiseFramebuffer->getRenderTargetHandler(0) /*SSRTexture[CurrentSSR]*/);
            ReflectionTexture = mDenoiseFramebuffer->getRenderTargetHandler(0);
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, mSSRFramebuffer[CurrentSSR]->getRenderTargetHandler(0));
            ReflectionTexture = mSSRFramebuffer[CurrentSSR]->getRenderTargetHandler(0);
            if (TotalFrames >= 250)
            {
                glBindTexture(GL_TEXTURE_2D, mSSRFramebuffer[!CurrentSSR]->getRenderTargetHandler(0));
                ReflectionTexture = mSSRFramebuffer[!CurrentSSR]->getRenderTargetHandler(0);
            }
            glGenerateMipmap(GL_TEXTURE_2D);
        }

        glActiveTexture(GL_TEXTURE1);
        CompositeShader->PushUniform("gColorSampler", 1);
        glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("colorBuffer"));
        //if (m_MotionBlur)
        //    glBindTexture(GL_TEXTURE_2D, MotionBlurBuffer);
        //else

        //glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE2);
        CompositeShader->PushUniform("ssaoColorBufferBlur", 2);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

        CompositeShader->PushUniform("TotalFrames", TotalFrames);
        CompositeShader->PushUniform("isMoving", isMoving);
        CompositeShader->PushUniform("HBAOOn", (int)mPostProcessData.HBAO.Active);

        RenderQuad();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        mCompositeImage->unbindFramebuffer();

        CopyTextureFBO->bindFramebuffer();
        CopyTextureFBO->setViewport();
        CopyTextureFBO->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        PassThroughShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(PassThroughShader->getProgramID(), "texture0"), 0);
        glBindTexture(GL_TEXTURE_2D, hdrFBO->getRenderTargetHandler("colorBuffer"));
        glGenerateMipmap(GL_TEXTURE_2D);

        CopyTextureFBO->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderQuad();

        PassThroughShader->Free();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        CopyTextureFBO->unbindFramebuffer();
        glViewport(0, 0, width, height);
    }

    void PostProcess::TAAPass() {

        mTAAFramebuffer[TAACurrentBuffer]->bindFramebuffer();
        mTAAFramebuffer[TAACurrentBuffer]->setViewport();
        glDisable(GL_BLEND);
        
        TAAShader->Use();

        glActiveTexture(GL_TEXTURE0);
        TAAShader->PushUniform("sCurrentFrame", 0);
        glBindTexture(GL_TEXTURE_2D, mCompositeImage->getRenderTargetHandler(0));
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glActiveTexture(GL_TEXTURE1);
        TAAShader->PushUniform("sLastFrame", 1);
        glBindTexture(GL_TEXTURE_2D, mTAAFramebuffer[!TAACurrentBuffer]->getRenderTargetHandler(0));
        
        glActiveTexture(GL_TEXTURE2);
        TAAShader->PushUniform("sVelocityBuffer", 2);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_MOTION_EXTRA));
        
        glActiveTexture(GL_TEXTURE3);
        TAAShader->PushUniform("sDepthBuffer", 3);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_IBL_DEPTH));

        TAAShader->PushUniform("iResolution", mTAAFramebuffer[TAACurrentBuffer]->Resolution());
        TAAShader->PushUniform("active", mPostProcessData.AntiAliasingSettings.Active);
        TAAShader->PushUniform("lerpAmount", mPostProcessData.AntiAliasingSettings.LerpAmount);
        TAAShader->PushUniform("clampingKernelSize", mPostProcessData.AntiAliasingSettings.ClampingKernelSize);
        TAAShader->PushUniform("clampingKernelSize", mPostProcessData.AntiAliasingSettings.ClampingKernelSize);
        TAAShader->PushUniform("_FeedbackMin", mPostProcessData.AntiAliasingSettings.FeedbackMin);
        TAAShader->PushUniform("_FeedbackMax", mPostProcessData.AntiAliasingSettings.FeedbackMax);
        
        RenderQuad();
        
        TAAShader->Free();

        mTAAFramebuffer[TAACurrentBuffer]->unbindFramebuffer();
        TAACurrentBuffer = !TAACurrentBuffer;

        /***Copy TAA texture for gamma correction***/

        mTAAFramebufferCopy->bindFramebuffer();
        mTAAFramebufferCopy->setViewport();
        mTAAFramebufferCopy->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        PassThroughShader->Use();

        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(PassThroughShader->getProgramID(), "texture0"), 0);
        glBindTexture(GL_TEXTURE_2D, mTAAFramebuffer[!TAACurrentBuffer]->getRenderTargetHandler(0));
        glGenerateMipmap(GL_TEXTURE_2D);

        mTAAFramebufferCopy->clearBuffer(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        RenderQuad();

        PassThroughShader->Free();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        mTAAFramebufferCopy->unbindFramebuffer();
        glViewport(0, 0, width, height);

    }

    void PostProcess::ShowPostProcessImage(float frametime, GLuint onmenu, glm::vec3 Sun, std::shared_ptr<Camera> &cam, std::shared_ptr<Epsilon::OpenGL::FrameBuffer<int>> framebuffer)
    {

        //hdrFBO->setToRead();
        hdrFBO->unbindFramebuffer();
        
        //if(SSROn) {
        SSRPass(cam);
        //}

        CompositeImage(cam->isMoving());
        

        TAAPass();

        if (mPostProcessData.MotionBlur.Active)
            MotionBlur(frametime);

        GLuint blurred = blurImage(hdrFBO->getRenderTargetHandler("brightColorBuffer"), false);

        //DownSampleSSR(frametime);

        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        framebuffer->bindFramebuffer();
        framebuffer->setViewport();
        //glViewport(0, 0, width, height);
        finalImage->Use();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::ARROW_UP])
            blurSize += 0.01;

        if (Input::KeyBoard::KEYS[Input::GLFW::Key::ARROW_DOWN])
            blurSize -= 0.01;

        glActiveTexture(GL_TEXTURE0);
        finalImage->PushUniform("compositeImage", 0);

        if (mPostProcessData.MotionBlur.Active)
            glBindTexture(GL_TEXTURE_2D, mMotionBlurFramebuffer->getRenderTargetHandler(0));
        else
            glBindTexture(GL_TEXTURE_2D, mTAAFramebuffer[TAACurrentBuffer]->getRenderTargetHandler(0));

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
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_IBL_DEPTH));

        glm::mat4 choppedView = glm::mat4(glm::mat3(cam->getViewMatrix()));
        finalImage->PushUniform("choppedView", choppedView);
        finalImage->PushUniform("projection", cam->getProjectionMatrix());
        finalImage->PushUniform("position", cam->getPosition());
        finalImage->PushUniform("sunPos", Sun);
        finalImage->PushUniform("blurSize", blurSize);
        finalImage->PushUniform("lightShafts", lightShafts);
        finalImage->PushUniform("onmenu", (int)0);
        finalImage->PushUniform("Resolution", glm::vec2(width, height));

        finalImage->PushUniform("BokehOn", mBokehDOF);
        finalImage->PushUniform("ChromaticAberration", mChromaticAberration);

        finalImage->PushUniform("exposure", m_exposure);
        finalImage->PushUniform("time", (float)glfwGetTime());

        
        finalImage->PushUniform("gamma", mPostProcessData.ImageSettings.Gamma);
        finalImage->PushUniform("contrast", mPostProcessData.ImageSettings.Contrast);
        finalImage->PushUniform("saturation", mPostProcessData.ImageSettings.Saturation);
        finalImage->PushUniform("exposure", mPostProcessData.ImageSettings.Exposure);
        finalImage->PushUniform("brightness", mPostProcessData.ImageSettings.Brightness);

        RenderQuad();

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void PostProcess::ShowFrame(glm::vec3 Sun, bool &hdr, std::shared_ptr<Camera> &cam, float exposure, std::shared_ptr<Renderer::ShadowMap> shadowMap, std::shared_ptr<Renderer::PointShadow> pointShadow)
    {

        hdrFBO->bindFramebuffer();
        hdrFBO->setViewport();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader->Use();

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
        //glViewport(0, 0, width, height);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "gDepth"), 0);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_IBL_DEPTH));
        glGenerateMipmap(GL_TEXTURE_2D);

        glActiveTexture(GL_TEXTURE1);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "gNormal"), 1);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_NORMAL));

        glActiveTexture(GL_TEXTURE2);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "gAlbedoSpec"), 2);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_ALBEDO_SPEC));

        glActiveTexture(GL_TEXTURE3);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "ssaoColorBufferBlur"), 3);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

        glActiveTexture(GL_TEXTURE4);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "shadowMap"), 4);
        glBindTexture(GL_TEXTURE_2D, shadowMap->getShadowTextureID());

        glActiveTexture(GL_TEXTURE5);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "gExtraComponents"), 5);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_MOTION_EXTRA));

        glActiveTexture(GL_TEXTURE6);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "gLightAccumulation"), 6);
        glBindTexture(GL_TEXTURE_2D, gBufferFramebuffer->getRenderTargetHandler(GBUFFER_GI));

        glActiveTexture(GL_TEXTURE7);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "gPointShadowMap"), 7);
        glBindTexture(GL_TEXTURE_CUBE_MAP, pointShadow->getTexture());
        
        glActiveTexture(GL_TEXTURE8);
        glUniform1i(glGetUniformLocation(shader->getProgramID(), "SSGI"), 8);
        glBindTexture(GL_TEXTURE_2D, HBILFramebuffer->getRenderTargetHandler(0));

        glm::mat4 invProj = glm::inverse(cam->getProjectionMatrix());
        glm::mat4 invView = glm::inverse(cam->getViewMatrix());
 
        shader->PushUniform("hdr", hdr);
        shader->PushUniform("time", (float)glfwGetTime());
        shader->PushUniform("exposure", exposure);
        shader->PushUniform("depthBias", shadowMap->getBiasMatrix());
        shader->PushUniform("lightSpaceMatrix", shadowMap->getLightSpaceMatrix());
        shader->PushUniform("projection", invProj);
        shader->PushUniform("view", invView);
        shader->PushUniform("uView", cam->getViewMatrix());
        shader->PushUniform("NUMLIGHTS", (int)m_Lights.size());
        shader->PushUniform("viewPos", glm::vec3(cam->getPosition().x, cam->getPosition().y, cam->getPosition().z));
        shader->PushUniform("uViewDir", glm::vec3(cam->getDirection().x, cam->getDirection().y, cam->getDirection().z));
        shader->PushUniform("lightDir", glm::vec3(Sun.x, Sun.y, Sun.z));
        shader->PushUniform("far_plane", (float)pointShadow->Far());

        RenderQuad();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE6);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glActiveTexture(GL_TEXTURE8);
        glBindTexture(GL_TEXTURE_2D, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glCache::glUseProgram(0);

        /** copy texture */

        /** end copy texture*/

        /**Fill mip maps begin*/
        /*
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
        RenderQuad();
    }
    blurBloom->Free();
    CopyTextureBlurredFBO->unbindFramebuffer();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glViewport(0, 0, width, height);

*/
        /**Fill mip maps end*/
        //glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        gBufferFramebuffer->setToRead();
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
} // namespace Epsilon