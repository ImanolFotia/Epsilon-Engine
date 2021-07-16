#ifndef HELPERS_H
#define HELPERS_H

#include <glm/glm.hpp>
#include <stb_image.h>
#include <GL/glad.h>
#include <iostream>
#include <string>
#include <image.hpp>
#include <thread>

static float hash11( float n ) {
    return glm::fract(glm::sin(n)*43758.5453123);
}

static float noise12( glm::vec2 x ) {
    glm::vec2 p = glm::floor(x);
    glm::vec2 f = glm::fract(x);

    f = f*f*(3.0f-2.0f*f);

    float n = p.x + p.y*57.0f;

    return glm::mix(glm::mix( hash11(n + 0.0f), hash11(n + 1.0f),f.x),glm::mix( hash11(n + 57.0f), hash11(n+ 58.0f),f.x),f.y);
}

static float fbm(glm::vec2 p) {

    auto hash11 = [&]( float n )->float {
        return glm::fract(glm::sin(n)*43758.5453123);
    };

    auto noise12 = [&]( glm::vec2 x ) ->float {
        glm::vec2 q = glm::floor(x);
        glm::vec2 f = glm::fract(x);

        f = f*f*(3.0f-2.0f*f);

        float n = q.x + q.y*57.0f;

        return glm::mix(glm::mix( hash11(n + 0.0f), hash11(n + 1.0f),f.x),glm::mix( hash11(n + 57.0f), hash11(n+ 58.0f),f.x),f.y);
    };


    float f = 0.0;
    f+=0.5 * noise12(p);
    p*=2.02;
    f+=0.25 * noise12(p);
    p*=2.03;
    f+=0.125 * noise12(p);
    p*=2.04;
    f+=0.0625 * noise12(p);
    p*=2.05;
    return f/0.9375;


}

static GLuint loadTexture(std::string path)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    std::cout << ("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/" + path).c_str() << std::endl;
    //unsigned char* image = SOIL_load_image(("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/" + path).c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

    //std::thread

    Image* img = new Image();
    //img.Load("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/" + path);



    //img->onLoadCallback(loaded);

    //std::thread t = img->AsyncLoad("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/" + path);
   // t.join();

    //auto loaded = [&]() -> void {

            int width; int height; int channels;
            unsigned char* image = img->getPixels();
            //std::cout << "Llega" << std::endl;
            width = img->getWidth();
            height = img->getHeight();
            channels = img->getChannels();

            std::cout << texture << std::endl;

            glBindTexture(GL_TEXTURE_2D, texture);

            if(channels == 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            else if(channels == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            else if(channels == 2)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, image);
            else if(channels == 1)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //SOIL_free_image_data(image);
            img->Destroy();
            glBindTexture(GL_TEXTURE_2D, 0);
//};

    std::cout << texture << std::endl;
    return texture;
}


static GLuint loadTextureAbsolute(std::string path)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int width; int height; int channels;
    std::cout << (path).c_str() << std::endl;
    //unsigned char* image = SOIL_load_image((path).c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

    Image img(path);
    unsigned char* image = img.getPixels();
    width = img.getWidth();
    height = img.getHeight();
    channels = img.getChannels();

    auto loaded = [&]() -> void {

            glBindTexture(GL_TEXTURE_2D, texture);

            if(channels == 4)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            else if(channels == 3)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            else if(channels == 2)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, image);
            else if(channels == 1)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            //SOIL_free_image_data(image);
            img.Destroy();
            glBindTexture(GL_TEXTURE_2D, 0);
};

    img.onLoadCallback(loaded);

    std::cout << channels << std::endl;


    return texture;
}

static std::string replace(std::string in,char chf, char cht){
    for(auto& c: in){
        if(c == chf)
            c = cht;
    }
    return in;
}
#endif // HELPERS_H
