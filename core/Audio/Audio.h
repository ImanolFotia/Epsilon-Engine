#pragma once
#include <pch.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>
//#include <AL/alBufferSOFT.h>

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
namespace Epsilon
{
    namespace IO
    {
        namespace Audio
        {
            class AudioElement;
            class Audio
            {
            public:
                Audio();
                ~Audio();
                void list_audio_devices(const ALCchar *devices, const char *message);

            public:
                void addAudioElement(int, std::shared_ptr<AudioElement> element);

                void PlayAudio();
                void PlayByID(int);

                bool setMasterVolume(float);
                bool setMusicVolume(float);
                bool setGameVolume(float);

            private:
                float m_MasterVolume;
                float m_MusicVolume;
                float m_GameVolume;

                ALCdevice *device;
                ALCcontext *context;

                std::unordered_map<int, std::shared_ptr<AudioElement>> m_AudioElementsCollection;
            };
        } // namespace Audio
    }     // namespace IO
} // namespace Epsilon