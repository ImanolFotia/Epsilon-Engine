///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#ifndef SKYBOX_H_INCLUDED
#define SKYBOX_H_INCLUDED
#include <string>
#include <GL/glew.h>
#include <camera.h>
#include <Model.h>
#include <Shader.h>
#include <memory>
class Skybox
{
public:
    Skybox(std::string);
    virtual ~Skybox(){
    }

public:

    void Render(std::unique_ptr<Camera>&, Shader*, float, bool);
    GLuint CubeMapID, CubeMapNormalID;

private:

    GLuint skyboxVAO, skyboxVBO, EBO;
    std::unique_ptr<Model> skydome;
    vector<string> Textures;

};

#endif /// SKYBOX_H_INCLUDED
