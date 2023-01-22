//
// Created by solaire on 1/20/23.
//

#ifndef EPSILON_AL_DATA_HPP
#define EPSILON_AL_DATA_HPP


#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>

struct OpenALData {
    ALCdevice *device;
    ALCcontext *context;
};

#endif //EPSILON_AL_DATA_HPP
