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
        //std::cout << "SkyBox Destroyed" << std::endl;
    }

public:

    void Render(std::shared_ptr<Camera>&, Shader*, float, bool);
    GLuint CubeMapID, CubeMapNormalID;

private:

    GLuint skyboxVAO, skyboxVBO, EBO;
    std::unique_ptr<Model> skydome;
    vector<string> Textures;

};

#endif /// SKYBOX_H_INCLUDED
