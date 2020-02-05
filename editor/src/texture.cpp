#include <include/texture.hpp>
#include <threadmanager.hpp>

template <>
ThreadManager<8>::ThreadManager_ptr ThreadManager<8>::_instance = ThreadManager::getInstance();
/*
template<>
ThreadManager<8>::ThreadManager_ptr ThreadManager<8>::loadedLastFrame(0);
*/

namespace OpenGL
{

void Texture::Load(std::string relative_path, std::string directory_path, std::string extension, std::string extra)
{
    Imageloaded = false;
    glGenTextures(1, &mTextureId);
    glBindTexture(GL_TEXTURE_2D, mTextureId);

    //std::cout << (directory_path + relative_path + extra + extension).c_str() << std::endl;
    mRelativePath = relative_path;
    mDirectoryPath = directory_path;
    mExtension = extension;
    mExtra = extra;

    if (mImage == nullptr)
    {
        mImage = new Image();
    }
    else
    {
        delete mImage;
        mImage = nullptr;
        mImage = new Image();
    }

    std::string completepath = directory_path + relative_path + extra + extension;
    isNonColorData = isNormal(completepath.c_str());

    auto loaded = [&]() -> void {
        int width;
        int height;
        int channels;
        unsigned char *image = mImage->getPixels();

        if (!image)
        {
            std::cout << "empty pixels" << std::endl;
        }

        // std::cout << image << std::endl;

        width = mImage->getWidth();
        height = mImage->getHeight();
        channels = mImage->getChannels();

        glBindTexture(GL_TEXTURE_2D, mTextureId);

        if (channels == 4)
        {
            if (isNonColorData)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
            else
            {
                std::cout << "Loading sRGBA\n";
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            }
        }

        else if (channels == 3)
        {
            if (isNonColorData)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            }
            else
            {
                std::cout << "Loading sRGB\n";
                glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            }
        }
        else if (channels == 2)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, width, height, 0, GL_RG, GL_UNSIGNED_BYTE, image);
        }
        else if (channels == 1)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image);
        }

        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        /*
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/

        //if(gltIsExtSupported("GL_EXT_texture_filter_anisotropic"))

        {
            GLfloat fLargest;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
            std::cout << fLargest << "x anisotropy supported" << std::endl;
        }

        mImage->Destroy();
        glBindTexture(GL_TEXTURE_2D, 0);

        ThreadManager<8>::getInstance()->loadedLastFrame++;
        Log::println("Info", std::string("Loaded: " + this->getAbsolutePath() + " " + std::to_string(this->mTextureId) + " " + std::to_string(width) + " " + std::to_string(height)));
        std::cout << "Loaded: " << this->getAbsolutePath() << " " << this->mTextureId << " " << width << " " << height << std::endl;
    };

    mImage->onLoadCallback(loaded);

    //std::thread* t =  mImage->AsyncLoad(directory_path + relative_path + extra + extension).get();
    //t->join();
    //mImage->Load(directory_path + relative_path + extra + extension);
    //const std::function<void()> ptr = &mImage->Load;
    std::cout << ThreadManager<8>::getInstance()->ActiveTasksCount() << std::endl;
    ThreadManager<8>::getInstance()->PushTask(Image::DispatchLoad, mImage, (completepath));
    //mLocalThread = (std::shared_ptr<std::thread>) new std::thread();
    //mLocalThread = (mImage->AsyncLoad(completepath));
    //mLocalThread->detach();
}

void Texture::LoadCubemap(std::vector<std::string> CubeMapPath)
{
    glGenTextures(1, &mTextureId);

    if(mImagesCubemap[0] == nullptr) {
        for(int i = 0; i < 6; i++)
            mImagesCubemap[i] = new Image();
    }
    else
    {
        for(int i = 0; i < 6; i++){
            delete mImagesCubemap[i];
            mImagesCubemap[i] = nullptr;
            mImagesCubemap[i] = new Image();
        }
    }

    auto loaded = [&](int id) -> void {
        if(mImagesCubemap[id] == nullptr) return;
        Log::println("Info", std::string("Cubemap texture id: ") + std::to_string(id) + std::string(" loaded"));
        glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureId);

        //for (GLuint i = 0; i < 6; i++)
        //{

            int width = mImagesCubemap[id]->getWidth();
            int height = mImagesCubemap[id]->getHeight();
            unsigned char *image = mImagesCubemap[id]->getPixels();
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + id, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        //}
        //std::cout << "CubeMapID" << texture << std::endl;
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        mImagesCubemap[id]->Destroy();
    };


    ThreadManager<8>::ThreadManager_ptr _instance = ThreadManager<8>::getInstance();

    for(int i = 0; i < 6; i++) {
        _instance->PushTask(Image::DispatchLoadId, mImagesCubemap[i], CubeMapPath.at(i), i);
        mImagesCubemap[i]->onLoadCallbackCubemap(loaded);
    }
}

} // namespace OpenGL
