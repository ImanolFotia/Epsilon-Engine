#include <Audio/Audio.h>
#include <Audio/AudioElement.h>
#include <cstring>


namespace IO
{
namespace Audio
{

Audio::Audio()
{

    device = alcOpenDevice(alcGetString(device, ALC_DEFAULT_DEVICE_SPECIFIER));
    if (device == NULL)
    {
        std::cout << "cannot open sound card" << std::endl;
        return;
    }
    context = alcCreateContext(device, NULL);
    if (context == NULL)
    {
        std::cout << "cannot open context" << std::endl;
        return;
    }
    alcMakeContextCurrent(context);
    
    if(!alcIsExtensionPresent(alcGetContextsDevice(alcGetCurrentContext()), "ALC_EXT_EFX"))
    {
        sprintf(g_szALC_EXT_EFX, "`xff0000Error: EFX not supported");
        return;
    }

#define LOAD_PROC(T, x) ((x) = (T)alcGetProcAddress(NULL, #x))
    LOAD_PROC(LPALCLOOPBACKOPENDEVICESOFT, alcLoopbackOpenDeviceSOFT);
    LOAD_PROC(LPALCISRENDERFORMATSUPPORTEDSOFT, alcIsRenderFormatSupportedSOFT);
    LOAD_PROC(LPALCRENDERSAMPLESSOFT, alcRenderSamplesSOFT);
#undef LOAD_PROC


    /* Define a macro to help load the function pointers. */
#define LOAD_PROC(x, y)  ((x) = (y)alGetProcAddress(#x))
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
    if(alIsExtensionPresent("AL_SOFT_buffer_samples"))
    {
        //LOAD_PROC(alBufferSamplesSOFT, LPALBUFFERSAMPLESSOFT);
        LOAD_PROC(alIsBufferFormatSupportedSOFT, LPALISBUFFERFORMATSUPPORTEDSOFT);
    }
#undef LOAD_PROC

    ALboolean enumeration;

    ALenum eBufferFormat = 0;
    eBufferFormat = alGetEnumValue("AL_FORMAT_51CHN16");

    enumeration = alcIsExtensionPresent(device, "ALC_ENUMERATION_EXT");

    if (enumeration != AL_FALSE)
    {
        list_audio_devices(alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER), "Available playback devices: ");
        list_audio_devices(alcGetString(device, ALC_DEFAULT_DEVICE_SPECIFIER), "Default playback device: ");
        list_audio_devices(alcGetString(NULL, ALC_CAPTURE_DEVICE_SPECIFIER), "Available capture devices: ");
        list_audio_devices(alcGetString(device, ALC_CAPTURE_DEFAULT_DEVICE_SPECIFIER), "Default capture device: ");
    }

    const char *name = NULL;
    if (alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
    if (!name || alcGetError(device) != AL_NO_ERROR)
        name = alcGetString(device, ALC_DEVICE_SPECIFIER);
    printf("Opened \"%s\"\n", name);

    alDopplerFactor(1.0f);
    alDopplerVelocity(340.29f);

    m_MasterVolume = 1.0f;
    m_MusicVolume = 1.0f;
    m_GameVolume = 1.0f;
}
Audio::~Audio()
{

    alcMakeContextCurrent(NULL);
    alcDestroyContext(context);
    alcCloseDevice(device);

    context = nullptr;
    device = nullptr;
    std::cout << "Deleted Audio" << std::endl;
}

void Audio::list_audio_devices(const ALCchar *devices, const char *message)
{
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
}

void Audio::addAudioElement(int ID, std::shared_ptr<AudioElement> element)
{
    m_AudioElementsCollection[ID] = element;
}

void Audio::PlayAudio()
{
    for (auto i : m_AudioElementsCollection)
    {
        if (i.second->getType() != MENU_SOUND)
            i.second->Play();
    }
}

void Audio::PlayByID(int ID)
{
    m_AudioElementsCollection[ID]->Play();
}

bool Audio::setMasterVolume(float) { return false; }
bool Audio::setMusicVolume(float) { return false; }
bool Audio::setGameVolume(float) { return false; }

} // namespace Audio
} // namespace IO
