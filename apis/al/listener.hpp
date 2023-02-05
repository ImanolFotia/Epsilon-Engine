#pragma once

#include <glm/glm.hpp>
#include "al_data.hpp"

namespace al {
    static const float *toOrientation(glm::vec3 dir, glm::vec3 up) {

    }

    static void setListenerPosition(glm::vec3 position) {

#if !defined(ANDROID) && !defined(__ANDROID__)
        alListener3f(AL_POSITION, position.x, position.y, position.z);
#endif
    }

    static void setListenerOrientation(glm::vec3 dir, glm::vec3 up) {
#if !defined(ANDROID) && !defined(__ANDROID__)
        ALfloat orientation[6] = {0};
        orientation[0] = dir.x;
        orientation[1] = dir.y;
        orientation[2] = dir.z;
        orientation[3] = up.x;
        orientation[4] = up.y;
        orientation[5] = up.z;
        alListenerfv(AL_ORIENTATION, orientation);
#endif
    }


}