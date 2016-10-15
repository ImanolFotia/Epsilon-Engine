#include <PostProcess.h>
#include <iostream>
#include <ProgramData.h>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <string>
#include <Includes.h>

PostProcess::PostProcess()
{
    this->LoadOffscreensShaders();
    this->SetupFramebuffer();
    this->lastDepth = 0.2;

}

void PostProcess::SetupFramebuffer()
{
    ProgramData PG;
    width = PG.WINDOW_WIDTH;
    height = PG.WINDOW_HEIGHT;
    SSROn = PG.SSR;
    this->SSAOwidth = width*0.5;
    this->SSAOheight = height*0.5;
    this->lightShafts = PG.LIGHTSHAFTS;
    m_exposure = 1.5;

    std::uniform_real_distribution<GLfloat> randomFloatsz(-20.0f, 45.0f); // generates random floats between 0.0 and 1.0
    std::uniform_real_distribution<GLfloat> randomFloatsx(50.0f, 130.0f); // generates random floats between 0.0 and 1.0
    //std::random_device ;
    std::default_random_engine generator(glfwGetTime());

/*
    LightPositions.push_back(glm::vec3(125, 20.5, -80));
    LightPositions.push_back(glm::vec3(70, 13.3, -63));
    LightPositions.push_back(glm::vec3(96.2, 13.3, -63));
    LightPositions.push_back(glm::vec3(123, 13.3, -63));
    LightPositions.push_back(glm::vec3(106.7, 30.63, -59.3));
    LightPositions.push_back(glm::vec3(82, 30.63, -59.3));
    LightPositions.push_back(glm::vec3(95.5, 15.21, -40.57));
*/

    LightPositions.push_back(glm::vec3(4, 15, -8));
    LightPositions.push_back(glm::vec3(20, 15, 7));
    LightPositions.push_back(glm::vec3(20, 15, -20));
    LightPositions.push_back(glm::vec3(45, 15, -22));
    LightPositions.push_back(glm::vec3(65, 15, -22));
    LightPositions.push_back(glm::vec3(65, 15, 8));
    LightPositions.push_back(glm::vec3(45, 15, 8));

    lensColor = (std::shared_ptr<eTexture>) new eTexture("effects/lenscolor.png", GL_REPEAT, GL_TEXTURE_1D);
    lensDirt = (std::shared_ptr<eTexture>) new eTexture("effects/lensdirt.png");
    lensStar = (std::shared_ptr<eTexture>) new eTexture("effects/lensstar.png");

/*
            LightPositions.push_back(glm::vec3(-41, 12.0, -23));
            LightPositions.push_back(glm::vec3(-39, 10.3, 2));
            LightPositions.push_back(glm::vec3(17.2, 17.3, -17));
            LightPositions.push_back(glm::vec3(17, 17.3, 27));
            LightPositions.push_back(glm::vec3(122.7, 13.63, 5.3));
            LightPositions.push_back(glm::vec3(-14, 16.63, 32.3));
            LightPositions.push_back(glm::vec3(80.5, 17.21, 1.57));
*/
    /* MSAA buffer
        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

        GLuint textureColorBufferMultiSampled;
        glGenTextures(1, &textureColorBufferMultiSampled);

        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, PG.MSAA_SAMPLES, GL_RGB16F, width, height, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);

        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glGenerateMipmap(GL_TEXTURE_2D);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, PG.MSAA_SAMPLES, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        attachment_type = GL_RGB16F;
        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, width, height, 0, GL_RGB, GL_FLOAT, NULL);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        glGenFramebuffers(1, &intermediateFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    */

    glGenFramebuffers(1, &hdrFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

    glGenTextures(1, &colorBuffer);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB16F, width, height, 0,GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glGenerateMipmap(GL_TEXTURE_2D);

    glGenTextures(1, &brightColorBuffer);
    glBindTexture(GL_TEXTURE_2D, brightColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0,GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightColorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &rDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rDepth);
    GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, DrawBuffers);

    GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (Status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "FB error, status: " << Status << std::endl;
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    SetupGBuffer();
    setupSSAO();
    SetupPingPongFBO();
    setupSSR();

}

void PostProcess::LoadOffscreensShaders()
{
    shader = (std::unique_ptr<Shader>)(new Shader("shaders/Lighting.vglsl", "shaders/Lighting.fglsl"));
    SSAO = (std::unique_ptr<Shader>)(new Shader("shaders/SSAO.vglsl", "shaders/SSAO.fglsl"));
    blurSSAO = (std::unique_ptr<Shader>)(new Shader("shaders/blurSSAO.vglsl", "shaders/blurSSAO.fglsl"));
    finalImage = (std::unique_ptr<Shader>)(new Shader("shaders/hdr.vglsl", "shaders/hdr.fglsl"));
    blurBloom = (std::unique_ptr<Shader>)(new Shader("shaders/blurBloom.vglsl", "shaders/blurBloom.fglsl"));
    blurSSRShader= (std::unique_ptr<Shader>)(new Shader("shaders/blurSSR.vglsl", "shaders/blurSSR.fglsl"));
    ScreenSpaceReflectionShader = (std::unique_ptr<Shader>)(new Shader("shaders/SSR.vglsl", "shaders/SSR.fglsl"));
    PassThroughShader = (std::unique_ptr<Shader>)(new Shader("shaders/PassThrough.vglsl", "shaders/PassThrough.fglsl"));
}

void PostProcess::beginOffScreenrendering()
{
    glDisable(GL_BLEND);
    glViewport(0,0,width, height);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedoSpec, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    /// - Normal color buffer

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /// - Position color buffer
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA32F, width, height, 0,GL_RGBA, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gPositionDepth, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    /// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    glGenTextures(1, &gExpensiveNormal);
    glBindTexture(GL_TEXTURE_2D, gExpensiveNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gExpensiveNormal, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &gDepth);
    glBindTexture(GL_TEXTURE_2D, gDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RED, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gDepth, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    /// - Create and attach depth buffer (renderbuffer)

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4};
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Framebuffer not complete!" << std::endl;
    /// - and blur stage
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, this->SSAOwidth, this->SSAOheight, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Sample kernel
    std::uniform_real_distribution<GLfloat> randomFloatsClamped(0.2, 1.0); // generates random floats between 0.0 and 1.0
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for (GLuint i = 0; i < 32; ++i)
    {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloatsClamped(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 32.0;

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
}

void PostProcess::applySSAO(std::shared_ptr<Camera>& cam)
{
    DownSampleSSR();
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    SSAO->Use();
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(SSAO->getProgramID(), "gPositionDepth"), 0);
    glBindTexture(GL_TEXTURE_2D, this->DownSampledTexture);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(SSAO->getProgramID(), "gNormal"), 1);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(SSAO->getProgramID(), "texNoise"), 2);
    glBindTexture(GL_TEXTURE_2D, this->noiseTexture);

    glUniform3fv(glGetUniformLocation(SSAO->getProgramID(), "samples"), 32, &ssaoKernel[0][0]);

    glUniformMatrix4fv(glGetUniformLocation(SSAO->getProgramID(), "projection"), 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(SSAO->getProgramID(), "invprojection"), 1, GL_FALSE, &glm::inverse(cam->getProjectionMatrix())[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(SSAO->getProgramID(), "invview"), 1, GL_FALSE, &glm::inverse(cam->getViewMatrix())[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(SSAO->getProgramID(), "view"), 1, GL_FALSE, &cam->getViewMatrix()[0][0]);
    glUniform1i(glGetUniformLocation(this->shader->getProgramID(), "width"), (float)width);
    glUniform1i(glGetUniformLocation(this->shader->getProgramID(), "height"), (float)height);
    glUniform3f(glGetUniformLocation(this->shader->getProgramID(), "viewRay"), cam->getDirection().x, cam->getDirection().y,cam->getDirection().z);
    glViewport(0,0,SSAOwidth, SSAOheight);
    this->RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,width, height);


    // 3. Blur SSAO texture to remove noise
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    glClear(GL_COLOR_BUFFER_BIT);
    blurSSAO->Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glViewport(0,0,SSAOwidth, SSAOheight);
    this->RenderQuad();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,width, height);
}

GLuint PostProcess::blurImage(GLuint Buffer)
{

    GLboolean horizontal = true, first_iteration = true, direction = true;
    GLuint amount = 5;
    blurBloom->Use();

    for(int i = 0 ; i < amount ; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        glViewport(0,0,320, 240);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glUniform1i(glGetUniformLocation(blurBloom->getProgramID(), "horizontal"), horizontal);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? Buffer : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        RenderQuad();

        if(i >= 4)
            direction = !direction;

        horizontal = !horizontal;

        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0,0,width, height);

    return pingpongColorbuffers[horizontal];
}

GLfloat PostProcess::applyAutoAxposure(GLuint Buffer)
{

    GLboolean horizontal = true, first_iteration = true;
    blurBloom->Use();

    float Ewidth = 640;
    float Eheight = 480;

    while(Ewidth > 1)
    {
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
    glReadPixels(0, 0, 1, 1, GL_RGB, GL_FLOAT, &luminescence);
    glm::vec3 rgbColor = glm::vec3(luminescence[0], luminescence[1], luminescence[2]);
    rgbColor = rgbColor * glm::vec3(0.2126, 0.7152, 0.0722);
    float lum = rgbColor.x + rgbColor.y + rgbColor.z;
    m_exposure = lerp(m_exposure, 2.0 / lum, 0.005); // slowly adjust exposure based on average brightness


    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0,0,width, height);

    return m_exposure;
}

void PostProcess::SetupPingPongFBO()
{
    glGenFramebuffers(2, pingpongFBO);
    glGenTextures(2, pingpongColorbuffers);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 320, 240, 0, GL_RGB, GL_FLOAT, NULL);
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

void PostProcess::SetupPingPongDOF()
{
    glGenFramebuffers(2, pingpongDOF);
    glGenTextures(2, pingpongColorbuffersDOF);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongDOF[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongColorbuffersDOF[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 640, 480, 0, GL_RGB, GL_FLOAT, NULL);
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
    glGenFramebuffers(1, &SSRFBO);
    glGenTextures(1, &SSRTexture);
    glBindFramebuffer(GL_FRAMEBUFFER, SSRFBO);
    glBindTexture(GL_TEXTURE_2D, SSRTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->width, this->height, 0, GL_RGB, GL_FLOAT, NULL);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSRTexture, 0);
    // Also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    setupPingPongSSR();
    setupDownSampledSSR();

}

void PostProcess::setupPingPongSSR()
{
    glGenFramebuffers(2, pingpongSSRFBO);
    glGenTextures(2, pingpongSSRT);
    for (GLuint i = 0; i < 2; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongSSRFBO[i]);
        glBindTexture(GL_TEXTURE_2D, pingpongSSRT[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 640, 480, 0, GL_RGB, GL_FLOAT, NULL);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->width/2, this->height/2, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // We clamp to the edge as the blur filter would otherwise sample repeated texture values!
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, DownSampledTexture, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    // Also check if framebuffers are complete (no need for depth buffer)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::SSRPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, SSRFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    ScreenSpaceReflectionShader->Use();
    glViewport(0,0, this->width, this->height);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gFinalImage"), 0);
    glBindTexture(GL_TEXTURE_2D, this->colorBuffer);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gPositionDepth"), 1);
    glBindTexture(GL_TEXTURE_2D, this->gPositionDepth);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gNormal"), 2);
    glBindTexture(GL_TEXTURE_2D, this->gNormal);

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "gColor"), 3);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

    glm::mat4 proj = glm::perspective( glm::radians(75.0f) , (float)this->width/(float)this->height , 0.1f , 3000.0f );
    glUniformMatrix4fv(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "projection"), 1, GL_FALSE, &proj[0][0]);
    glUniform2f(glGetUniformLocation(ScreenSpaceReflectionShader->getProgramID(), "Resolution"), this->width, this->height);

    this->RenderQuad();
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcess::DownSampleSSR()
{
    glBindFramebuffer(GL_FRAMEBUFFER, DownSamplerFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    PassThroughShader->Use();
    glViewport(0,0, this->width/2, this->height/2);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(PassThroughShader->getProgramID(), "texture0"), 0);
    glBindTexture(GL_TEXTURE_2D, this->gPositionDepth);

    this->RenderQuad();
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    PassThroughShader->Free();
}

GLuint PostProcess::blurSSR(GLuint Buffer)
{

    GLboolean horizontal = true, first_iteration = true, direction = true;
    GLuint amount = 5;
    blurSSRShader->Use();

    for(int i = 0 ; i < amount ; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongSSRFBO[horizontal]);
        glViewport(0,0, 640, 480);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glUniform1i(glGetUniformLocation(blurSSRShader->getProgramID(), "horizontal"), horizontal);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(glGetUniformLocation(blurSSRShader->getProgramID(), "image"), 0);
        glBindTexture(GL_TEXTURE_2D, first_iteration ? this->DownSampledTexture : pingpongSSRT[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)
        RenderQuad();

        if(i >= 4)
            direction = !direction;

        horizontal = !horizontal;

        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0,0,width, height);

    return pingpongSSRT[horizontal];


}

void PostProcess::ShowPostProcessImage(float exposure, GLuint ShadowMapID, glm::vec3 Sun, std::shared_ptr<Camera>& cam)
{

    GLuint blurred = this->blurImage(this->brightColorBuffer);
    if(SSROn) {
        DownSampleSSR();
        //blurSSR(1);
        SSRPass();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glBindTexture(GL_TEXTURE_2D, this->colorBuffer);

    glViewport(0,0,this->width, this->height);
    finalImage->Use();

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "sampler"), 0);
    glBindTexture(GL_TEXTURE_2D, this->colorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "blurredSampler"), 1);
    glBindTexture(GL_TEXTURE_2D, blurred);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "uLensColor"), 2);
    glBindTexture(GL_TEXTURE_1D, lensColor->getTextureID());

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "uLensDirtTex"), 3);
    glBindTexture(GL_TEXTURE_2D, lensDirt->getTextureID());

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "uLensStarTex"), 4);
    glBindTexture(GL_TEXTURE_2D, lensStar->getTextureID());

    glActiveTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "gReflectionSampler"), 5);
    glBindTexture(GL_TEXTURE_2D, SSRTexture);
    glGenerateMipmap(GL_TEXTURE_2D);

    glm::mat4 choppedView = glm::mat4(glm::mat3(cam->getViewMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(finalImage->getProgramID(), "choppedView"), 1, GL_FALSE, &choppedView[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(finalImage->getProgramID(), "projection"), 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniform3f(glGetUniformLocation(finalImage->getProgramID(), "sunPos"), Sun.x, Sun.y, Sun.z);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "lightShafts"), this->lightShafts);
    glUniform2f(glGetUniformLocation(finalImage->getProgramID(), "Resolution"), this->width, this->height);

    glUniform1f(glGetUniformLocation(finalImage->getProgramID(), "exposure"), exposure);
    glUniform1f(glGetUniformLocation(finalImage->getProgramID(), "time"), glfwGetTime());

    this->RenderQuad();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void PostProcess::ShowFrame(glm::vec3 Sun, bool & hdr, std::shared_ptr<Camera>& cam, float exposure, std::unique_ptr<ShadowMap>& shadowMap)
{

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    this->shader->Use();


    glViewport(0,0,this->width, this->height);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gDepth"), 0);
    glBindTexture(GL_TEXTURE_2D, this->gDepth);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gNormal"), 1);
    glBindTexture(GL_TEXTURE_2D, this->gExpensiveNormal);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gAlbedoSpec"), 2);
    glBindTexture(GL_TEXTURE_2D, this->gAlbedoSpec);

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(finalImage->getProgramID(), "ssaoColorBufferBlur"), 3);
    glBindTexture(GL_TEXTURE_2D, this->ssaoColorBufferBlur);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "shadowMap"), 4);
    glBindTexture(GL_TEXTURE_2D, shadowMap->getShadowTextureID());

    glUniform1i(glGetUniformLocation(this->shader->getProgramID(), "hdr"), hdr);
    glUniform1f(glGetUniformLocation(this->shader->getProgramID(), "exposure"), exposure);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "lightSpaceMatrix"), 1, GL_FALSE, &shadowMap->getLightSpaceMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "depthBias"), 1, GL_FALSE, &shadowMap->getBiasMatrix()[0][0]);

    glm::mat4 invProj = glm::inverse(cam->getProjectionMatrix());
    glm::mat4 invView = glm::inverse(cam->getViewMatrix());
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, &invProj[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "view"), 1, GL_FALSE, &invView[0][0]);
    glUniform3fv(glGetUniformLocation(shader->getProgramID(), "LightPositions"), LightPositions.size(), &LightPositions[0][0]);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "viewPos"),  cam->getPosition().x, cam->getPosition().y, cam->getPosition().z);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "uViewDir"),  cam->getDirection().x, cam->getDirection().y, cam->getDirection().z);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "lightDir"),  Sun.x, Sun.y, Sun.z);

    this->RenderQuad();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, hdrFBO);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glEnable(GL_BLEND);

}

float PostProcess::lerp(float v0, float v1, float t)
{
    return (1-t)*v0 + t*v1;
}

void PostProcess::RenderQuad()
{
    if (quadVAO == 0)
    {
        GLfloat quadVertices[] =
        {
            // Positions         //Texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f
        };
        // Setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

}
