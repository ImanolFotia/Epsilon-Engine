#pragma once

#include "../audio_manager.hpp"
#include <apis/al/al.hpp>

namespace engine::audio {
	class ALAudioManager : public AudioManager {
		Pool<AudioSource, al::OpenALSource> sourcesPool;
		Pool<AudioBuffer, al::OpenALBuffer> buffersPool;
		Pool<AudioListener, al::OpenALListener> listenersPool;

		al::OpenALData m_pAlData;
	public:
		virtual void Init() override;
		virtual void CleanUp() override;

		virtual Ref<AudioBuffer> createBuffer(const std::string& name, const BufferInfo&) override;
		virtual Ref<AudioSource> createSource(const std::string& name, const SourceInfo&) override;
		virtual Ref<AudioListener> createListener(const std::string& name, const ListenerInfo&) override;

		al::OpenALSource* getSource(Ref<AudioSource>);
		al::OpenALBuffer* getBuffer(Ref<AudioBuffer>);
		al::OpenALListener* getListener(Ref<AudioListener>);


		virtual void setSourcePosition(Ref<AudioSource>, glm::vec3) override;
		virtual void setSourceDirection(Ref<AudioSource>, glm::vec3) override;
		virtual void setSourceVelocity(Ref<AudioSource>, glm::vec3) override;
		virtual void setSourceGain(Ref<AudioSource>, float) override;
		virtual void setSourceAngle(Ref<AudioSource>, float) override;
		virtual void setSourcePitch(Ref<AudioSource>, float) override;
		virtual void setSourceState(Ref<AudioSource>, AudioState) override;


		virtual glm::vec3 getSourcePosition(Ref<AudioSource>) override;
		virtual glm::vec3 getSourceDirection(Ref<AudioSource>)override;
		virtual glm::vec3 getSourceVelocity(Ref<AudioSource>) override;
		virtual float getSourceGain(Ref<AudioSource>) override;
		virtual float getSourceAngle(Ref<AudioSource>) override;
		virtual float getSourcePitch(Ref<AudioSource>) override;
		virtual AudioState getSourceState(Ref<AudioSource>) override;

		virtual void deleteBuffer(Ref<AudioBuffer>) override;
		virtual void deleteSource(Ref<AudioSource>) override;
		virtual void deleteListener(Ref<AudioListener>) override;


	};
}