#pragma once

#include <vector>
#include <Log.h>

#include "File.hpp"
#include "FilesystemArgs.hpp"
#include "FilesystemEventHandler.hpp"
#include "MountPoint.hpp"

namespace Epsilon::Filesystem
{
    class Filesystem final
    {
    public:
        Filesystem(Filesystem &&) = delete;
        Filesystem(const Filesystem &) = delete;

        static Filesystem &const Instance()
        {
            return mInstance;
        }

        virtual void GetFile() {
        }

        virtual void GetAllFiles() {}

        virtual void Find() {

        }

        virtual void Seek(){
        }

        virtual void Read(){
        }

        virtual void Watch(){
        }

        virtual void Close(){
        }
        
        virtual void Unmount(){
        }


        virtual void Mount(){
        }

    private:
        Filesystem() = default;
        static Filesystem mInstance;
    };
}