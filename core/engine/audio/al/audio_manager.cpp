#include "audio_manager.hpp"
#include "apis/al/al_data.hpp"
#include "apis/al/source.hpp"
#include "core/engine/audio/audio_manager.hpp"
#include "core/framework/clock.hpp"
#include <core/framework/log.hpp>
#include <string>

#if defined(ANDROID) || defined(__ANDROID__)

#include <android/log.h>

#define LOG_TAG "Epsilon"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#endif

namespace engine::audio {
void ALAudioManager::Init() {
  bool res = al::initDevice(&m_pAlData);

  if (!res) {

    Log::Error("Couldn't start audio device");
  }

  listenersPool.Initialize();
  sourcesPool.Initialize();
  buffersPool.Initialize();

  for (int i = 0; i < MAX_SOURCES; i++) {
    al::OpenALSource alSource = al::createSource();
    m_FreeSources.insert(alSource.id);
  }
  Log::Success("Audio subsystem initialized");
}

void ALAudioManager::CleanUp() {

  for (ALuint i = 1; i <= MAX_SOURCES; i++) {
    al::stopSource({.id = i});
    al::deleteSource({.id = i});
  }

  for (auto &buffer : buffersPool) {
    if (buffer.id > 0)
      al::deleteBuffer(buffer);
  }
  al::destroyDevice(&m_pAlData);
}

void ALAudioManager::Update() {
  if (m_pAlData.shouldReloadDevice) {
    m_pAlData.shouldReloadDevice = false;
    const char *deviceName = alcGetString(m_pAlData.device, ALC_ALL_DEVICES_SPECIFIER);

    std::cout << "device: " << deviceName << std::endl;

    if (nullptr == m_pAlData.device) {
      Log::Error("device reopen failed: \ncause: device is null");
      return;
    }

    // if (ALC_EVENT_TYPE_DEFAULT_DEVICE_CHANGED_SOFT == eventType) {
    bool res = alcReopenDeviceSOFT(m_pAlData.device, NULL, NULL);

    if (res == ALC_FALSE) {
      auto error_code = alcGetError(m_pAlData.device);
      Log::Error("device reopen failed: ", deviceName);
      Log::Error(error_code);
    }
    //}
  }

  if (framework::Clock::Time() >= m_LastCollection + COLLECT_TIME) {
    for (auto &source : sourcesPool) {
      if (source.id == 0 || m_FreeSources.contains(source.id))
        continue;
      if (!al::isPlaying(source) && !al::isPaused(source)) {
        m_FreeSources.insert(source.id);
        m_PlayingSources.extract(std::hash<std::string>{}(source.name));
        Log::Info("Freed audio source: ", source.id);
      }
    }
    m_LastCollection = framework::Clock::Time();
  }
}

Ref<AudioBuffer> ALAudioManager::createBuffer(const std::string &name, const BufferInfo &buffer) {
  al::OpenALBuffer alBuffer;
  alBuffer = al::createBuffer(buffer.numChannels, buffer.size, buffer.bps, buffer.bitrate, buffer.data);
  auto ref = buffersPool.insert(name, alBuffer);
  return ref;
}

ALuint ALAudioManager::pGetFreeSource() {
  if (m_FreeSources.size() <= 0) {
    Log::Warning("There was no free audio source reference, try again...");
    return 0;
  }
  auto it = std::begin(m_FreeSources);
  std::advance(it, 0);
  auto id = *it;
  Log::Info("Creating new source: ", id);
  m_FreeSources.extract(id);

  return id;
}

void ALAudioManager::pinSource(Ref<AudioSource> sourceRef) {
  
}

void ALAudioManager::SetSourceData(al::OpenALSource &source, const SourceInfo &data) {
  source.name = data.name;
  source.angle = data.angle;
  source.direction = data.direction;
  source.gain = data.gain;
  source.velocity = data.velocity;
  source.position = data.position;
  source.buffer = *buffersPool.get(data.buffer);
}

Ref<AudioSource> ALAudioManager::createSource(const std::string &name, const SourceInfo &sourceInfo) {

  al::OpenALSource alSource;

  SetSourceData(alSource, sourceInfo);

  alSource.name = name;

  auto sourceRef = sourcesPool.insert(name, alSource);

  Log::Info("Source successfully created");

  return sourceRef;
}

uint32_t ALAudioManager::releaseSource(Ref<AudioSource> source) {
  auto sourceRef = sourcesPool.get(source);
  m_FreeSources.insert(sourceRef->id);
  m_PlayingSources.extract(std::hash<std::string>{}(sourceRef->name));
  return sourceRef->id;
}

Ref<AudioListener> ALAudioManager::createListener(const std::string &name, const ListenerInfo &listenerInfo) {

  auto ref = listenersPool.emplace(name);
  auto listener = listenersPool.get(ref);
  al::setListenerGain(*listener);
  al::setListenerOrientation(*listener);
  al::setListenerPosition(*listener);
  al::setListenerVelocity(*listener);
  return ref;
}

void ALAudioManager::Play(Ref<AudioSource> source) {

  al::OpenALSource *alSource = sourcesPool.get(source);

  Log::Info("Trying to play: ", alSource->name);

  if (m_PlayingSources.contains(std::hash<std::string>{}(alSource->name)))
    return;

  if (alSource->id == 0) {
    ALuint free_id = pGetFreeSource();
    if (free_id == 0)
      return;
    alSource->id = free_id;
    al::setBuffer(*alSource, alSource->buffer);
  }

  Log::Success("Playing source: ", alSource->id);
  al::playSource(*alSource);

  m_PlayingSources.insert(std::hash<std::string>{}(alSource->name));
}

void ALAudioManager::Pause(Ref<AudioSource> source) { al::pauseSource(*getSource(source)); }

void ALAudioManager::Stop(Ref<AudioSource> source) { al::stopSource(*getSource(source)); }

al::OpenALSource *ALAudioManager::getSource(Ref<AudioSource> source) { return sourcesPool.get(source); }
al::OpenALBuffer *ALAudioManager::getBuffer(Ref<AudioBuffer> buffer) { return buffersPool.get(buffer); }
al::OpenALListener *ALAudioManager::getListener(Ref<AudioListener> listener) { return listenersPool.get(listener); }

void ALAudioManager::setSourcePosition(Ref<AudioSource> source_ref, glm::vec3 position) {
  auto source = sourcesPool.get(source_ref);
  source->position = position;
}
void ALAudioManager::setSourceDirection(Ref<AudioSource> source_ref, glm::vec3 direction) {
  auto source = sourcesPool.get(source_ref);
  source->direction = direction;
}
void ALAudioManager::setSourceVelocity(Ref<AudioSource> source_ref, glm::vec3 velocity) {
  auto source = sourcesPool.get(source_ref);
  source->velocity = velocity;
}
void ALAudioManager::setSourceGain(Ref<AudioSource> source_ref, float gain) {
  auto source = sourcesPool.get(source_ref);
  source->gain = gain;
}
void ALAudioManager::setSourceAngle(Ref<AudioSource> source_ref, float angle) {
  auto source = sourcesPool.get(source_ref);
  source->angle = angle;
}
void ALAudioManager::setSourcePitch(Ref<AudioSource> source_ref, float pitch) {
  auto source = sourcesPool.get(source_ref);
  source->pitch = pitch;
}

void ALAudioManager::setSourceState(Ref<AudioSource> source_ref, AudioState state) {
  auto source = sourcesPool.get(source_ref);

  if (state == AudioState::PLAYING)
    al::playSource(*source);
  if (state == AudioState::PAUSED)
    al::pauseSource(*source);
  if (state == AudioState::STOPPED)
    al::stopSource(*source);
}

void ALAudioManager::setListenerPosition(Ref<AudioListener> listener_ref, glm::vec3 position) {

  auto listener = listenersPool.get(listener_ref);
  listener->position = position;
  al::setListenerPosition(*listener);
}

void ALAudioManager::setListenerDirection(Ref<AudioListener> listener_ref, glm::vec3 direction) {

  auto listener = listenersPool.get(listener_ref);
  listener->direction = direction;
  al::setListenerOrientation(*listener);
}

AudioState ALAudioManager::getSourceState(Ref<AudioSource> source_ref) {
  auto source = *sourcesPool.get(source_ref);
  if (source.id == 0)
    return AudioState::STOPPED;
  auto state = al::getSourceState(source);
  if (state == AL_PLAYING)
    return AudioState::PLAYING;
  if (state == AL_STOPPED)
    return AudioState::STOPPED;
  if (state == AL_PAUSED)
    return AudioState::PAUSED;

  return AudioState::STOPPED;
}

glm::vec3 ALAudioManager::getSourcePosition(Ref<AudioSource> source_ref) {
  auto source = sourcesPool.get(source_ref);
  return source->position;
}
glm::vec3 ALAudioManager::getSourceDirection(Ref<AudioSource> source_ref) {
  auto source = sourcesPool.get(source_ref);
  return source->direction;
}
glm::vec3 ALAudioManager::getSourceVelocity(Ref<AudioSource> source_ref) {
  auto source = sourcesPool.get(source_ref);
  return source->velocity;
}
float ALAudioManager::getSourceGain(Ref<AudioSource> source_ref) {
  auto source = sourcesPool.get(source_ref);
  return source->gain;
}
float ALAudioManager::getSourceAngle(Ref<AudioSource> source_ref) {
  auto source = sourcesPool.get(source_ref);
  return source->angle;
}
float ALAudioManager::getSourcePitch(Ref<AudioSource> source_ref) {
  auto source = sourcesPool.get(source_ref);
  return source->pitch;
}
uint32_t ALAudioManager::getId(Ref<AudioSource> source_ref) { return sourcesPool.get(source_ref)->id; }

void ALAudioManager::deleteBuffer(Ref<AudioBuffer> buffer_ref) {
  auto buffer = *buffersPool.get(buffer_ref);

  al::deleteBuffer(buffer);
  buffersPool.destroy(buffer_ref);
}

uint32_t ALAudioManager::deleteSource(Ref<AudioSource> source_ref) {
  sourcesPool.destroy(source_ref);
}
void ALAudioManager::deleteListener(Ref<AudioListener> listener) {}
} // namespace engine::audio