#pragma once

#include "al_data.hpp"

#include <iostream>
#include <cstring>

//#include <AL/alBufferSOFT.h>


#if !defined(ANDROID) && !defined(__ANDROID__)
static LPALCLOOPBACKOPENDEVICESOFT alcLoopbackOpenDeviceSOFT;
static LPALCISRENDERFORMATSUPPORTEDSOFT alcIsRenderFormatSupportedSOFT;
static LPALCRENDERSAMPLESSOFT alcRenderSamplesSOFT;

//static LPALBUFFERSAMPLESSOFT alBufferSamplesSOFT = wrap_BufferSamples;
static LPALISBUFFERFORMATSUPPORTEDSOFT alIsBufferFormatSupportedSOFT;
/* Effect object functions */
static LPALGENEFFECTS alGenEffects;
static LPALDELETEEFFECTS alDeleteEffects;
static LPALISEFFECT alIsEffect;
static LPALEFFECTI alEffecti;
static LPALEFFECTIV alEffectiv;
static LPALEFFECTF alEffectf;
static LPALEFFECTFV alEffectfv;
static LPALGETEFFECTI alGetEffecti;
static LPALGETEFFECTIV alGetEffectiv;
static LPALGETEFFECTF alGetEffectf;
static LPALGETEFFECTFV alGetEffectfv;
/* Auxiliary Effect Slot object functions */
static LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
static LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
static LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
static LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
static LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
static LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
static LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
static LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
static LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
static LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
static LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;

static char g_szALC_EXT_EFX[256] = {'\0'};
#endif
namespace al {

    static void list_audio_devices(const char *devices, const char *message)
    {

#if !defined(ANDROID) && !defined(__ANDROID__)
        const ALCchar *device = devices, *next = devices + 1;
        size_t len = 0;

        std::cout << message << "\n";
        while (device && *device != '\0' && next && *next != '\0')
        {
            std::cout << device << "\n";
            len = std::strlen(device);
            device += (len + 1);
            next += (len + 2);
        }
        std::cout << "----------\n";
#endif
    }

#if defined(ANDROID) || defined(__ANDROID__)
struct OpenALData;
#endif
    static bool initDevice(OpenALData* al_data) {

        auto device_name = alcGetString(al_data->device, ALC_DEFAULT_DEVICE_SPECIFIER);
        al_data->device = alcOpenDevice(device_name);
        if (al_data->device == NULL)
        {
            std::cout << "cannot open sound card" << std::endl;
            return false;
        }
        al_data->context = alcCreateContext(al_data->device, NULL);
        if (al_data->context == NULL)
        {
            std::cout << "cannot open context" << std::endl;
            return false;
        }
        alcMakeContextCurrent(al_data->context);

#if !defined(ANDROID) && !defined(__ANDROID__)
        if (!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX"))
        {
            std::cout << g_szALC_EXT_EFX << "`xff0000Error: EFX not supported" << std::endl;
            return false;
        }

#define LOAD_PROC(T, x) ((x) = (T)alcGetProcAddress(NULL, #x))
        LOAD_PROC(LPALCLOOPBACKOPENDEVICESOFT, alcLoopbackOpenDeviceSOFT);
        LOAD_PROC(LPALCISRENDERFORMATSUPPORTEDSOFT, alcIsRenderFormatSupportedSOFT);
        LOAD_PROC(LPALCRENDERSAMPLESSOFT, alcRenderSamplesSOFT);
#undef LOAD_PROC

        /* Define a macro to help load the function pointers. */
#define LOAD_PROC(x, y) ((x) = (y)alGetProcAddress(#x))
        LOAD_PROC(alGenEffects, LPALGENEFFECTS);
        LOAD_PROC(alDeleteEffects, LPALDELETEEFFECTS);
        LOAD_PROC(alIsEffect, LPALISEFFECT);
        LOAD_PROC(alEffecti, LPALEFFECTI);
        LOAD_PROC(alEffectiv, LPALEFFECTIV);
        LOAD_PROC(alEffectf, LPALEFFECTF);
        LOAD_PROC(alEffectfv, LPALEFFECTFV);
        LOAD_PROC(alGetEffecti, LPALGETEFFECTI);
        LOAD_PROC(alGetEffectiv, LPALGETEFFECTIV);
        LOAD_PROC(alGetEffectf, LPALGETEFFECTF);
        LOAD_PROC(alGetEffectfv, LPALGETEFFECTFV);
        LOAD_PROC(alGenAuxiliaryEffectSlots, LPALGENAUXILIARYEFFECTSLOTS);
        LOAD_PROC(alDeleteAuxiliaryEffectSlots, LPALDELETEAUXILIARYEFFECTSLOTS);
        LOAD_PROC(alIsAuxiliaryEffectSlot, LPALISAUXILIARYEFFECTSLOT);
        LOAD_PROC(alAuxiliaryEffectSloti, LPALAUXILIARYEFFECTSLOTI);
        LOAD_PROC(alAuxiliaryEffectSlotiv, LPALAUXILIARYEFFECTSLOTIV);
        LOAD_PROC(alAuxiliaryEffectSlotf, LPALAUXILIARYEFFECTSLOTF);
        LOAD_PROC(alAuxiliaryEffectSlotfv, LPALAUXILIARYEFFECTSLOTFV);
        LOAD_PROC(alGetAuxiliaryEffectSloti, LPALGETAUXILIARYEFFECTSLOTI);
        LOAD_PROC(alGetAuxiliaryEffectSlotiv, LPALGETAUXILIARYEFFECTSLOTIV);
        LOAD_PROC(alGetAuxiliaryEffectSlotf, LPALGETAUXILIARYEFFECTSLOTF);
        LOAD_PROC(alGetAuxiliaryEffectSlotfv, LPALGETAUXILIARYEFFECTSLOTFV);
        if (alIsExtensionPresent("AL_SOFT_buffer_samples"))
        {
            //LOAD_PROC(alBufferSamplesSOFT, LPALBUFFERSAMPLESSOFT);
            LOAD_PROC(alIsBufferFormatSupportedSOFT, LPALISBUFFERFORMATSUPPORTEDSOFT);
        }
#undef LOAD_PROC
#endif

        ALboolean enumeration;

        ALenum eBufferFormat = 0;
        eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");

        enumeration = alcIsExtensionPresent(al_data->device, "ALC_ENUMERATION_EXT");

        if (enumeration != AL_FALSE)
        {
            list_audio_devices(alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER), "Available playback devices: ");
            list_audio_devices(alcGetString(al_data->device, ALC_DEFAULT_DEVICE_SPECIFIER), "Default playback device: ");
            list_audio_devices(alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER), "Available capture devices: ");
            list_audio_devices(alcGetString(al_data->device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER), "Default capture device: ");
        }
        alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);

        const char *name = NULL;
        if (alcIsExtensionPresent(al_data->device, "ALC_ENUMERATE_ALL_EXT"))
            name = alcGetString(al_data->device, ALC_ALL_DEVICES_SPECIFIER);
        if (!name || alcGetError(al_data->device) != AL_NO_ERROR)
            name = alcGetString(al_data->device, ALC_DEVICE_SPECIFIER);

        printf("Opened \"%s\"\n", name);

        return true;
    }

    static void destroyDevice(OpenALData* al_data) {

#if !defined(ANDROID) && !defined(__ANDROID__)
        ALCdevice* device;
        ALCcontext* ctx;
        ctx = alcGetCurrentContext();
        if (ctx == NULL)
            return;

        device = alcGetContextsDevice(ctx);

        alcMakeContextCurrent(NULL);
        alcDestroyContext(ctx);
        alcCloseDevice(device);
#endif
    }
}