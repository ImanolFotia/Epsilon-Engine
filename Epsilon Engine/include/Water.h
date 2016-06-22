///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef WATER_H_INCLUDED
#define WATER_H_INCLUDED

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Shader.h>
#include <Texture.h>
#include <Camera.h>

#include <vector>
#include <memory>

class Water{

public:

    Water(glm::vec3 position, float scale);
    virtual ~Water(){
        std::cout << "Deleted Water" << std::endl;
    }

public:

    void GenerateReflection(std::unique_ptr<Camera>& cam);
    void GenerateRefraction(std::unique_ptr<Camera>& cam);
    void FinishWatercomputation(void);
    void RenderWater(std::unique_ptr<Camera>& cam, glm::vec3 lightDir);

public:
    float scale;
    glm::vec3 position;

    double getActiveClip()
    {
        return ActiveClip;
    }

    void setActiveClip(double t)
    {
        ActiveClip = t;
    }

private:

    void LoadTextures(void);
    void LoadShaders(void);
    void CreateReflectionFBO(void);
    void CreateRefractionFBO(void);
    void GeneratePlane(void);
    void GenerateModelViewProjection(std::unique_ptr<Camera>& cam);
    void GeneratevertexArray();


private:

    GLuint colorTexture;
    GLuint normalTexture;
    GLuint dispTexture;
    GLuint DuDvTexture;
    GLuint foamTexture;
    GLuint diffuseTexture;

    GLuint reflectionTexture;
    GLuint refractionTexture;
    GLuint refractionDepthTexture;

    GLuint refractionRBO;
    GLuint reflectionRBO;

    GLuint refractionFBO;
    GLuint reflectionFBO;

    GLint refractionWidth;
    GLint refractionHeight;

    GLint reflectionWidth;
    GLint reflectionHeight;

    GLuint ReflectionResoulution;
    GLuint RefractionResoulution;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    std::shared_ptr<Shader> shader;

    glm::mat4 MVP;
    double ActiveClip;

};

#endif /// WATER_H_INCLUDED
