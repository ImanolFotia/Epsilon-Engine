#pragma once

namespace IO{
    namespace Image{

        enum COLOR_TYPE {
            NO_COLOR = 0,
            COLOR_PRESENT,
        };

        enum IMAGE_TYPE {
            NO_IMAGE = 0,
            UNCOMP_COLOR_MAP = 1,
            UNCOMP_TRUE_COLOR = 2,
            UNCOMP_BW = 3,
            RUN_LEN_ENC_COLOR_MAP = 9,
            RUN_LEN_ENC_TRUE_COLOR = 10,
            RUN_LEN_ENC_BW = 11
        };

        const uint8_t TGA_HEADER = 0x12;
        struct TGA_HEADER {
            uint8_t ID_Length;
            uint8_t Color_Map_Type;
            uint8_t Image_Type;
            TGA_COLOR_MAP_SPEC Color_Map_Spec;
        };

        const uint8_t TGA_COLOR_MAP_SPEC_SIZE = 0x5;
        struct TGA_COLOR_MAP_SPEC {
            uint16_t First_entry;
            uint16_t Length;
            uint8_t Entry_size;
        };

        const uint8_t TGA_IMAGE_SPECIFICATION_SIZE = 0xA;
        struct TGA_IMAGE_SPECIFICATION {
            uint16_t x_origin;
            uint16_t y_origin;
            uint16_t width;
            uint16_t height;
            uint8_t pixel_depth;
            uint8_t descriptor;
        };

        class TGA{
        public:
            TGA(){
                mErrorStr = "No error";
            }
            
            TGA(const char* path) : mPath(path){
                mErrorStr = "No error";
            }

            const unsigned char* LoadFile(const char* path){

            }

            bool Destroy(){
                if(mData != nullptr){
                    delete mData;
                    mData = nullptr;
                    return true;
                } else {
                    mErrorStr = "Trying to delete nullptr";
                }
            }

            const char* getError() {
                    return mErrorStr;
            }

        private:
            unsigned char* mData = nullptr;
            const char* mPath = nullptr;
            const char* mErrorStr = nullptr;
        };
    }
}
