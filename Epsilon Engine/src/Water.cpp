///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Water.h>

Water::Water(glm::vec3 position, float scale)
{
    this->scale = scale;
    this->position = position;

    this->ReflectionResoulution = 512;
    this->RefractionResoulution = 512;


    m_Model = (std::shared_ptr<Model>) new Model("models/plane.eml");

    this->LoadShaders();
    this->LoadTextures();
}

void Water::RenderWater(std::unique_ptr<Camera>& cam, glm::vec3 lightDir)
{
    shader->Use();
    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "normalSampler"), 0);
    glBindTexture(GL_TEXTURE_2D, this->normalTexture);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "DuDvSampler"), 1);
    glBindTexture(GL_TEXTURE_2D, this->DuDvTexture);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "foamSampler"), 2);
    glBindTexture(GL_TEXTURE_2D, this->foamTexture);

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "diffuseSampler"), 3);
    glBindTexture(GL_TEXTURE_2D, this->diffuseTexture);


    glm::mat4 model = glm::mat4();
    /// glm::mat4 ScaleMatrix = glm::scale(glm::mat4(), glm::vec3(scale,1,scale));
    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), this->position);
    model = TranslationMatrix;
    model = glm::scale(model, glm::vec3(scale, 1.0f, scale));
    this->MVP = cam->getProjectionMatrix() * cam->getViewMatrix() * model;
    glm::mat4 view = glm::mat4(cam->getViewMatrix());

    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "projection"), 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "MVP"), 1, GL_FALSE, &this->MVP[0][0]);
    glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"),  glfwGetTime());
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "cameraPosition"), cam->getPosition().x, cam->getPosition().y, cam->getPosition().z);
    glUniform3f(glGetUniformLocation(shader->getProgramID(), "LightDirection"), lightDir.x, lightDir.y, lightDir.z);

    m_Model->DrawNoTexture();

    glEnable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);


}

void Water::LoadTextures(void)
{
    eTexture* tex = new eTexture("Wavy_Water - Height (Normal Map 4).png");
    normalTexture = tex->getTextureID();

    delete tex;

    eTexture* tex2 = new eTexture("Well Preserved Chesterfield - (Normal Map_2).png");
    DuDvTexture = tex2->getTextureID();

    delete tex2;

    eTexture* tex3 = new eTexture("fog_texture_by_finding_the_time-d637xrm.png");
    foamTexture = tex3->getTextureID();

    delete tex3;

    eTexture* tex4 = new eTexture("Wavy_Water - Color Map.png");
    diffuseTexture = tex4->getTextureID();

    delete tex4;
}

void Water::LoadShaders(void)
{
    shader = (std::shared_ptr<Shader>) new Shader("shaders/water.vglsl", "shaders/water.fglsl");
}

void Water::CreateReflectionFBO(void)
{
    glGenFramebuffers(1, &reflectionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    glGenTextures(1, &reflectionTexture);
    glBindTexture(GL_TEXTURE_2D, reflectionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB16F, ReflectionResoulution, ReflectionResoulution, 0,GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glGenRenderbuffers(1, &reflectionRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, reflectionRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ReflectionResoulution, ReflectionResoulution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, reflectionRBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflectionTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::CreateRefractionFBO(void)
{
    glGenFramebuffers(1, &refractionFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);

    glGenTextures(1, &refractionTexture);
    glBindTexture(GL_TEXTURE_2D, refractionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB16F, RefractionResoulution, RefractionResoulution, 0,GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, refractionTexture, 0);

    glGenRenderbuffers(1, &refractionRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, refractionRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RefractionResoulution, RefractionResoulution);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, refractionRBO);

    glGenTextures(1, &refractionDepthTexture);
    glBindTexture(GL_TEXTURE_2D, refractionDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH24_STENCIL8, RefractionResoulution, RefractionResoulution, 0,GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, refractionDepthTexture, 0);

    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Water::GenerateReflection(std::unique_ptr<Camera>& cam)
{
    GenerateModelViewProjection(cam);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    glViewport(0,0,ReflectionResoulution, ReflectionResoulution);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::GenerateRefraction(std::unique_ptr<Camera>& cam)
{
    GenerateModelViewProjection(cam);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
    glViewport(0,0,RefractionResoulution, RefractionResoulution);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::GeneratevertexArray()
{
    GLfloat plane[] =
    {
        1.0f * scale,  0,  1.0f* scale, 1.0f, 1.0f,
        1.0f*  scale,  0, -1.0f* scale, 1.0f, 0.0f,
        -1.0f*  scale,  0, -1.0f* scale, 0.0f, 0.0f,
        -1.0f*  scale,  0,  1.0f* scale, 0.0f, 1.0f,

    };
    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(plane), &plane[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

    glBindVertexArray(0);

}

void Water::FinishWatercomputation(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::GenerateModelViewProjection(std::unique_ptr<Camera>& cam)
{
    glm::mat4 model = glm::mat4();
    MVP = cam->getProjectionMatrix() * cam->getViewMatrix() * model;
}
