#pragma once

#include "audio_manager.hpp"

namespace engine::audio {
	void ALAudioManager::Init() {
		bool res = al::initDevice(&m_pAlData);

		if (!res)
			std::cout << "Couldn't start audio device" << std::endl;
	}

	void ALAudioManager::CleanUp() {
		al::destroyDevice(&m_pAlData);
	}

	Ref<AudioBuffer> ALAudioManager::createBuffer(const std::string& name, const BufferInfo& buffer) {
		al::OpenALBuffer alBuffer;
		alBuffer = al::createBuffer(buffer.numChannels, buffer.size, buffer.bps, buffer.bitrate, buffer.data);
		auto ref = buffersPool.insert(name, alBuffer);
		return ref;
	}

	Ref<AudioSource> ALAudioManager::createSource(const std::string& name) {

		auto ref = sourcesPool.emplace(name);
		return ref;
	}
	Ref<AudioListener> ALAudioManager::createListener(const std::string& name) {

		auto ref = listenersPool.emplace(name);
		return ref;
	}

	al::OpenALSource* ALAudioManager::getSource(Ref<AudioSource> source) {
		return sourcesPool.get(source);
	}
	al::OpenALBuffer* ALAudioManager::getBuffer(Ref<AudioBuffer> buffer) {
		return buffersPool.get(buffer);
	}
	al::OpenALListener* ALAudioManager::getListener(Ref<AudioListener> listener) {
		return listenersPool.get(listener);
	}

	void ALAudioManager::deleteBuffer(Ref<AudioBuffer> buffer) {}
	void ALAudioManager::deleteSource(Ref<AudioSource> source) {}
	void ALAudioManager::deleteListener(Ref<AudioListener> listener) {}
}