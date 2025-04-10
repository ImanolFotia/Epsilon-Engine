#pragma once

#include "../audio_manager.hpp"
#include <apis/al/al.hpp>

namespace engine::audio {
class ALAudioManager : public AudioManager {
  Pool<AudioSource, al::OpenALSource> sourcesPool;
  Pool<AudioBuffer, al::OpenALBuffer> buffersPool;
  Pool<AudioListener, al::OpenALListener> listenersPool;

  al::OpenALData m_pAlData;

  std::set<int> m_FreeSources;
  std::set<std::size_t> m_PlayingSources;

  double m_LastCollection = 0.0;
  const double COLLECT_TIME = 100.0; //milliseconds
  const std::size_t MAX_SOURCES = 256;

  ALuint pGetFreeSource();


	void SetSourceData(al::OpenALSource& source, const SourceInfo& data);

public:
  virtual void Init() override;
  virtual void Init(AudioManagerInitParams) override {}
  virtual void CleanUp() override;
  virtual void Update() override;

  virtual Ref<AudioBuffer> createBuffer(const std::string &name, const BufferInfo &) override;
  virtual Ref<AudioSource> createSource(const std::string &name, const SourceInfo &) override;
  virtual Ref<AudioListener> createListener(const std::string &name, const ListenerInfo &) override;

  virtual void Play(Ref<AudioSource>) override;
  virtual void Pause(Ref<AudioSource>) override;
  virtual void Stop(Ref<AudioSource>) override;

  al::OpenALSource *getSource(Ref<AudioSource>);
  al::OpenALBuffer *getBuffer(Ref<AudioBuffer>);
  al::OpenALListener *getListener(Ref<AudioListener>);

  virtual void setSourcePosition(Ref<AudioSource>, glm::vec3) override;
  virtual void setSourceDirection(Ref<AudioSource>, glm::vec3) override;
  virtual void setSourceVelocity(Ref<AudioSource>, glm::vec3) override;
  virtual void setSourceGain(Ref<AudioSource>, float) override;
  virtual void setSourceAngle(Ref<AudioSource>, float) override;
  virtual void setSourcePitch(Ref<AudioSource>, float) override;
  virtual void setSourceState(Ref<AudioSource>, AudioState) override;
  virtual void pinSource(Ref<AudioSource>) override;

  virtual void setListenerPosition(Ref<AudioListener>, glm::vec3) override;
  virtual void setListenerDirection(Ref<AudioListener>, glm::vec3) override;

  virtual glm::vec3 getSourcePosition(Ref<AudioSource>) override;
  virtual glm::vec3 getSourceDirection(Ref<AudioSource>) override;
  virtual glm::vec3 getSourceVelocity(Ref<AudioSource>) override;
  virtual float getSourceGain(Ref<AudioSource>) override;
  virtual float getSourceAngle(Ref<AudioSource>) override;
  virtual float getSourcePitch(Ref<AudioSource>) override;
  virtual AudioState getSourceState(Ref<AudioSource>) override;
  virtual uint32_t getId(Ref<AudioSource>) override;


  virtual uint32_t releaseSource(Ref<AudioSource>) override;

  virtual void deleteBuffer(Ref<AudioBuffer>) override;
  virtual uint32_t deleteSource(Ref<AudioSource>) override;
  virtual void deleteListener(Ref<AudioListener>) override;
};
} // namespace engine::audio