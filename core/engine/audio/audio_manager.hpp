#pragma once
#include "../object_pool.hpp"

#include <glm/glm.hpp>

namespace engine::audio {
class AudioSource;
class AudioBuffer;
class AudioListener;

struct BufferInfo {
  size_t numChannels{};
  std::size_t size{};
  std::size_t bps{};
  std::size_t bitrate{};
  unsigned char *data = nullptr;
};

struct SourceInfo {
  Ref<AudioBuffer> buffer;
  glm::vec3 position{};
  glm::vec3 direction{};
  glm::vec3 velocity{};
  float gain = 1.0;
  float angle{};
  float pitch{};

  std::string name;
};

struct ListenerInfo {
  glm::vec3 position{};
  glm::vec3 direction{};
  glm::vec3 velocity{};
  float gain = 1.0;
};

enum class AudioState { PLAYING = 0, PAUSED, STOPPED, SIZE };

enum class AudioKind { MUSIC = 0, SFX, SIZE };

struct AudioManagerInitParams {
  int pool_reserve[(int)AudioKind::SIZE] = {0};
  float initial_gain = 1.0;
};

class AudioManager {
public:
  virtual void Init() = 0;
  virtual void Init(AudioManagerInitParams) = 0;
  virtual void CleanUp() = 0;

  virtual void Update() = 0;

  virtual Ref<AudioBuffer> createBuffer(const std::string &, const BufferInfo &) = 0;
  virtual Ref<AudioSource> createSource(const std::string &, const SourceInfo &) = 0;
  virtual Ref<AudioListener> createListener(const std::string &, const ListenerInfo &) = 0;

  virtual void Play(Ref<AudioSource>) = 0;
  virtual void Pause(Ref<AudioSource>) = 0;
  virtual void Stop(Ref<AudioSource>) = 0;

  virtual void setSourcePosition(Ref<AudioSource>, glm::vec3) = 0;
  virtual void setSourceDirection(Ref<AudioSource>, glm::vec3) = 0;
  virtual void setSourceVelocity(Ref<AudioSource>, glm::vec3) = 0;
  virtual void setSourceGain(Ref<AudioSource>, float) = 0;
  virtual void setSourceAngle(Ref<AudioSource>, float) = 0;
  virtual void setSourcePitch(Ref<AudioSource>, float) = 0;
  virtual void setSourceState(Ref<AudioSource>, AudioState) = 0;

  virtual void pinSource(Ref<AudioSource>) = 0;

  virtual void setListenerPosition(Ref<AudioListener>, glm::vec3) = 0;
  virtual void setListenerDirection(Ref<AudioListener>, glm::vec3) = 0;

  virtual glm::vec3 getSourcePosition(Ref<AudioSource>) = 0;
  virtual glm::vec3 getSourceDirection(Ref<AudioSource>) = 0;
  virtual glm::vec3 getSourceVelocity(Ref<AudioSource>) = 0;
  virtual float getSourceGain(Ref<AudioSource>) = 0;
  virtual float getSourceAngle(Ref<AudioSource>) = 0;
  virtual float getSourcePitch(Ref<AudioSource>) = 0;
  virtual AudioState getSourceState(Ref<AudioSource>) = 0;
  virtual uint32_t getId(Ref<AudioSource>) = 0;

  virtual uint32_t releaseSource(Ref<AudioSource>) = 0;

  virtual void deleteBuffer(Ref<AudioBuffer>) = 0;
  virtual uint32_t deleteSource(Ref<AudioSource>) = 0;
  virtual void deleteListener(Ref<AudioListener>) = 0;
};
} // namespace engine::audio