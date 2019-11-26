#pragma once

namespace IO{
    namespace Image{
        class TGA{
        public:
            TGA(){}
            TGA(const char* path) : mPath(path){}

            LoadFile(const char* path){

            }

            Destroy(){
                delete mData;
                mData = nullptr;
            }

        private:
            unsigned char* mData;
            char* mPath;
        };
    }
}
