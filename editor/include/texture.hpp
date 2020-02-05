#pragma once
#include <GL/glad.h>
#include <string>
#include <iostream>

#include <image.hpp>

#include <include/Log.hpp>



namespace OpenGL {
class Texture
{
public:
    Texture(){
        mImage = new Image();
    }
    ~Texture(){
        //std::cout << "Deleted Texture" << std::endl;
    }

    void Load(std::string relative_path, std::string directory_path = "", std::string extension = "", std::string extra = "");

    void LoadCubemap(std::vector<std::string> CubeMapPath);

    void LoadFromMemory(int w, int h, int c, unsigned char* data){
        glGenTextures(1, &mTextureId);
        glBindTexture(GL_TEXTURE_2D, mTextureId);


       // std::cout << mTextureId << std::endl;

        glBindTexture(GL_TEXTURE_2D, mTextureId);

        if(c == 4){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        else if(c == 3){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else if(c == 2){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, w, h, 0, GL_RG, GL_UNSIGNED_BYTE, data);
        }
        else if(c == 1){
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        }

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);


        //std::cout << mTextureId << std::endl;
    }

    GLuint getTextureId()
    {
        //std::cout << (mImage.isLoaded() ? "loaded" : "notloaded") << std::endl;
        //std::cout << "texture loaded" << std::endl;
        try{
/*
            if(this->mLocalThread->joinable()){
                //std::cout << "thread joined" << std::endl;
                this->mLocalThread->join();
            }*/

            //if(!mImage->getPixels())
              //  Reload();

            if(mImage == nullptr) return mTextureId;
            if(mImage->isLoaded() && Imageloaded == false)
            {
                //std::cout << "texture loaded" << std::endl;
                Imageloaded = true;
                mImage->mOnLoad();
                delete mImage;

            }
            return mTextureId;
        }
        catch(...)
        {
            std::cout << getAbsolutePath() << std::endl;
            std::cout << "Exception" <<std::endl;
        }
    }


    GLuint getCubemapTextureId()
    {
        try{
        for(int i = 0; i < 6; i++){
            if(mImagesCubemap[i] == nullptr) continue;
            if(mImagesCubemap[i]->isLoaded() && loadedTextures[i] == false)
            {
                loadedTextures[i] = true;
                mImagesCubemap[i]->mOnLoadCubemap(i);

                glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
                glGenerateTextureMipmap (mTextureId);
                glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
                delete mImagesCubemap[i];
            }
        }
            return mTextureId;
        }
        catch(...)
        {
            std::cout << getAbsolutePath() << std::endl;
            std::cout << "Exception" <<std::endl;
        }
    }

    void Reload()
    {
        //Load(mRelativePath, mDirectoryPath, mExtension, mExtra);
    }

    bool isNormal(const char* TexName) {
        std::string name = std::string(TexName);
        std::size_t found2 = name.find("_s");
        std::size_t found1 = name.find("_n");
        std::size_t found3 = name.find("specular");
        std::size_t found4 = name.find("normal");
        std::size_t found5 = name.find("ddn");
        std::size_t found6 = name.find("nrm");
        std::size_t found7 = name.find("Normal");
        std::size_t found8 = name.find("_m");
        std::size_t found9 = name.find("roughness");
        std::size_t found10 = name.find("metallic");
        std::size_t found11 = name.find("_metalness");
        std::size_t found12 = name.find("_gloss");
        std::size_t found13 = name.find("_r");

        std::size_t npos = std::string::npos;

        return         found1 != npos ||
                found2 != npos ||
                found3 != npos ||
                found4 != npos ||
                found5 != npos ||
                found6 != npos ||
                found7 != npos ||
                found8 != npos ||
                found9 != npos ||
                found10 != npos ||
                found11 != npos ||
                found12 != npos ||
                found13 != npos;
    }

    int getWidth()
    {
        return mWidth;
    }

    int getHeight()
    {
        return mHeight;
    }

    int getChannels()
    {
        return mChannels;
    }

    std::string getAbsolutePath()
    {
        return mDirectoryPath + mRelativePath + mExtra + mExtension;
    }

    std::string getRelativePath()
    {
        return mRelativePath;
    }

    void Destroy()
    {
        glDeleteTextures(1, &mTextureId);
    }

private:
    bool isNonColorData = false;
    bool Imageloaded = false;
    GLuint mTextureId = 0;
    Image *mImage = nullptr;
    Image* mImagesCubemap[6] = {nullptr};
    std::shared_ptr<std::thread> mLocalThread;
    std::string mRelativePath;
    std::string mDirectoryPath;
    std::string mExtension;
    std::string mExtra;
    bool loadedTextures[6] = {false};

    int mWidth = 0, mHeight = 0, mChannels = 0;
};
}
