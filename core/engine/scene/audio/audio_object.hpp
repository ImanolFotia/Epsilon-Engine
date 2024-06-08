#pragma once

#include <core/engine/audio/audio_manager.hpp>
#include <core/framework/audio/WAVFile.h>

namespace engine {

struct AudioBuffer {
  Ref<audio::AudioBuffer> buffer;
  std::vector<Ref<audio::AudioSource>> sources;
};

class AudioObject {

  std::vector<Ref<audio::AudioSource>> source{};
  AudioBuffer buffer{};
  float angle = 360.0;
  float gain = 1.0;
  glm::vec3 position{};
  glm::vec3 direction{};
  glm::vec3 velocity{};
  float pitch = 1.0;

  friend class AssetManager;

public:
  // Setters
  void Position(glm::vec3 p) { position = p; }

  void Direction(glm::vec3 d) { direction = d; }

  void Velocity(glm::vec3 v) { velocity = v; }

  void Angle(float a) { angle = a; }

  void Gain(float g) { gain = g; }

  void Pitch(float p) { pitch = p; }

  // Getters
  glm::vec3 Position() { return position; }

  glm::vec3 Direction() { return direction; }

  glm::vec3 Velocity() { return velocity; }

  float Angle() { return angle; }

  float Gain() { return gain; }

  float Pitch() { return pitch; }

  Ref<audio::AudioSource> Source(int i) { return source[i]; }

  AudioBuffer Buffer() { return buffer; }
};
} // namespace engine