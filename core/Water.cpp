///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <Water.hpp>

namespace Epsilon {
Water::Water(glm::vec3 position, float scale)
{
    this->scale = scale;
    this->position = position;

    this->ReflectionResoulution = 512;
    this->RefractionResoulution = 512;

    this->LoadShaders();
    this->LoadTextures();

    GeneratevertexArray();
}

void Water::RenderWater(std::shared_ptr<Camera> cam, GLuint colorBuffer, glm::vec3 lightDir, GLuint depthTexture, GLuint reflectionTex)
{
    shader->Use();
    glDisable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "ColorBuffer"), 0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "normalSampler"), 1);
    glBindTexture(GL_TEXTURE_2D, this->normalTexture);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "DuDvSampler"), 2);
    glBindTexture(GL_TEXTURE_2D, this->DuDvTexture);

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "gDepth"), 3);
    glBindTexture(GL_TEXTURE_2D, depthTexture);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "Reflection"), 4);
    glBindTexture(GL_TEXTURE_2D, reflectionTex);


    glm::mat4 model = glm::mat4(1.0);
    //glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0), glm::vec3(scale,1,scale));
    model = glm::translate(model, this->position);
    this->MVP = cam->getProjectionMatrix() * cam->getViewMatrix() * model;

    shader->PushUniform("lightDir", lightDir);
    shader->PushUniform("viewDir", cam->getDirection());
    shader->PushUniform("viewPos", cam->getPosition());
    shader->PushUniform("ResX", cam->winx);
    shader->PushUniform("ResY", cam->winy);
    glUniformMatrix4fv(shader->MVP_Location, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(shader->WorldTransform_Location, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(shader->View_Location, 1, GL_FALSE, &cam->getViewMatrix()[0][0]);
    glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    shader->PushUniform("time", (float)glfwGetTime());

    glBindVertexArray(this->VAO);
    glCache::glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader->Free();

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
    eTexture* tex = new eTexture("Wavy_Water_n.png", GL_REPEAT, GL_TEXTURE_2D, -1);
    normalTexture = tex->getTextureID();

    delete tex;

    eTexture* tex2 = new eTexture("waterDUDV.png");
    DuDvTexture = tex2->getTextureID();

    delete tex2;

    eTexture* tex3 = new eTexture("fog_texture_by_finding_the_time-d637xrm.png");
    foamTexture = tex3->getTextureID();

    delete tex3;

    eTexture* tex4 = new eTexture("4141-diffuse.jpg");
    diffuseTexture = tex4->getTextureID();

    delete tex4;
}

void Water::LoadShaders(void)
{
    shader = (std::shared_ptr<Shader>) new Shader("shaders/water.vglsl", "shaders/water.fglsl");
}

void Water::CreateReflectionFBO(void)
{/*
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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
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


void Water::GenerateReflection(std::shared_ptr<Camera> cam)
{
    GenerateModelViewProjection(cam);
    glBindFramebuffer(GL_FRAMEBUFFER, reflectionFBO);
    glViewport(0,0,ReflectionResoulution, ReflectionResoulution);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::GenerateRefraction(std::shared_ptr<Camera> cam)
{
    GenerateModelViewProjection(cam);
    glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
    glViewport(0,0,RefractionResoulution, RefractionResoulution);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Water::GeneratevertexArray()
{
    glm::vec3 verts[] =
    {
        glm::vec3(84.0f*this->scale,  0.0,  115.0f*this->scale),
        glm::vec3(84.0f*this->scale,  0.0, -115.0f*this->scale),
        glm::vec3(-115.0f*this->scale,  0.0, -115.0f*this->scale),
        glm::vec3(-115.0f*this->scale,  0.0,  115.0f*this->scale)

    };

    glm::vec2 texcoords[] =
    {
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(0.0f, 1.0f)
    };

    GLuint indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    glm::vec3 tangent, binormal;


    {
        glm::vec3 v0 = verts[ indices[0] ];
        glm::vec3 v1 = verts[ indices[1] ];
        glm::vec3 v2 = verts[ indices[2] ];

        glm::vec3 edge1 = v2 - v0;
        glm::vec3 edge2 = v1 - v0;

        glm::vec2 t0 = texcoords[ indices[0] ];
        glm::vec2 t1 = texcoords[ indices[1] ];
        glm::vec2 t2 = texcoords[ indices[2] ];

        glm::vec2 deltaUV1 = t1 - t0;
        glm::vec2 deltaUV2 = t2 - t0;

        float f = (deltaUV2.x * deltaUV1.x - deltaUV2.y * deltaUV1.y) == 0.0f ? -1.0f : 1.0f;
        if ( 0 == deltaUV2.x && 0 ==deltaUV2.y && 0 == deltaUV1.x && 0 == deltaUV1.y )
        {
            deltaUV1.x = 0.0;
            deltaUV1.y = 1.0;
            deltaUV2.y = 1.0;
            deltaUV2.y = 0.0;
        }

        tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        binormal.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        binormal.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        binormal.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    }
/*
    tangent = glm::normalize(tangent - glm::dot(tangent, glm::vec3(0,1,0)) * glm::vec3(0,1,0));
    // then retrieve perpendicular vector B with the cross product of T and N
    binormal = glm::cross(tangent, glm::vec3(0,1,0));

    if (glm::dot(glm::cross(glm::vec3(0,1,0), tangent), binormal) < 0.0f)
    {
        tangent = tangent * -1.0f;
    }
*/
    glm::vec3 tangents[] =
    {
        tangent,
        tangent,
        tangent,
        tangent

    };
    std::cout << tangent.x << " " << tangent.y << " " << tangent.z << std::endl;

    glm::vec3 binormals[] =
    {
        binormal,
        binormal,
        binormal,
        binormal
    };

    glGenVertexArrays(1, &this->VAO);
    GLuint UVBO, TBO, BBO;
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &BBO);
    glGenBuffers(1, &TBO);
    glGenBuffers(1, &UVBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, &verts[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, UVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, &texcoords[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, &tangents[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, BBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, &binormals[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

}

void Water::FinishWatercomputation(void)
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Water::GenerateModelViewProjection(std::shared_ptr<Camera> cam)
{
    glm::mat4 model = glm::mat4(1.0);
    MVP = cam->getProjectionMatrix() * cam->getViewMatrix() * model;
}
}