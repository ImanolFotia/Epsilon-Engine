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
		virtual Ref<AudioSource> createSource(const std::string& name) override;
		virtual Ref<AudioListener> createListener(const std::string& name) override;

		al::OpenALSource* getSource(Ref<AudioSource>);
		al::OpenALBuffer* getBuffer(Ref<AudioBuffer>);
		al::OpenALListener* getListener(Ref<AudioListener>);

		virtual void deleteBuffer(Ref<AudioBuffer>) override;
		virtual void deleteSource(Ref<AudioSource>) override;
		virtual void deleteListener(Ref<AudioListener>) override;


	};
}