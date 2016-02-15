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

class Skybox
{
public:
    Skybox(std::string);
    virtual ~Skybox(){
        delete skydome;
    }

public:

    void Render(Camera*&, Shader*);
    GLuint CubeMapID;

private:

    GLuint skyboxVAO, skyboxVBO, EBO;
    Model* skydome;
    vector<string> Textures;

};

#endif /// SKYBOX_H_INCLUDED
