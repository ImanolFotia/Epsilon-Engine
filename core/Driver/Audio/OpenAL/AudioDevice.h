#pragma once
#include <pch.hpp>
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>

#include "../AudioDevice.hpp"
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
namespace Epsilon::Audio::OpenAL
{
    class AudioSource;
    class AudioDevice : public Epsilon::Audio::AudioDevice
    {
    public:
        AudioDevice();
        ~AudioDevice();
        void list_audio_devices(const ALCchar *devices, const char *message);

    public:
        void addAudioElement(int, std::shared_ptr<AudioSource> element);

        void PlayAudio();
        void PlayByID(int);
        void Destroy();

        bool setMasterVolume(float);
        bool setMusicVolume(float);
        bool setGameVolume(float);
        const std::unordered_map<int, std::shared_ptr<AudioSource>> &getAudioElements();

    private:
        float m_MasterVolume;
        float m_MusicVolume;
        float m_GameVolume;

        ALCdevice *device;
        ALCcontext *context;

        std::unordered_map<int, std::shared_ptr<AudioSource>> m_AudioElementsCollection;
    };
} // namespace Audio