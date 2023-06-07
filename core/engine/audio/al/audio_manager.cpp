#include "audio_manager.hpp"

namespace engine::audio
{
	void ALAudioManager::Init()
	{
		bool res = al::initDevice(&m_pAlData);

		if (!res)
			std::cout << "Couldn't start audio device" << std::endl;
	}

	void ALAudioManager::CleanUp()
	{
		al::destroyDevice(&m_pAlData);
	}

	Ref<AudioBuffer> ALAudioManager::createBuffer(const std::string& name, const BufferInfo& buffer)
	{
		al::OpenALBuffer alBuffer;
		alBuffer = al::createBuffer(buffer.numChannels, buffer.size, buffer.bps, buffer.bitrate, buffer.data);
		auto ref = buffersPool.insert(name, alBuffer);
		return ref;
	}

	Ref<AudioSource> ALAudioManager::createSource(const std::string& name, const SourceInfo& sourceInfo)
	{

		al::OpenALSource alSource;
		alSource = al::createSource(*buffersPool.get(sourceInfo.buffer));
		auto ref = sourcesPool.insert(name, alSource);
		return ref;
	}
	Ref<AudioListener> ALAudioManager::createListener(const std::string& name, const ListenerInfo& listenerInfo)
	{

		auto ref = listenersPool.emplace(name);
		auto listener = listenersPool.get(ref);
		al::setListenerGain(*listener);
		al::setListenerOrientation(*listener);
		al::setListenerPosition(*listener);
		al::setListenerVelocity(*listener);
		return ref;
	}


	void ALAudioManager::Play(Ref<AudioSource> source) {
		al::playSource(*getSource(source));
	}

	void ALAudioManager::Pause(Ref<AudioSource> source) {
		al::pauseSource(*getSource(source));
	}

	void ALAudioManager::Stop(Ref<AudioSource> source) {
		al::stopSource(*getSource(source));
	}

	al::OpenALSource* ALAudioManager::getSource(Ref<AudioSource> source)
	{
		return sourcesPool.get(source);
	}
	al::OpenALBuffer* ALAudioManager::getBuffer(Ref<AudioBuffer> buffer)
	{
		return buffersPool.get(buffer);
	}
	al::OpenALListener* ALAudioManager::getListener(Ref<AudioListener> listener)
	{
		return listenersPool.get(listener);
	}

	void ALAudioManager::setSourcePosition(Ref<AudioSource> source_ref, glm::vec3 position)
	{
		auto source = sourcesPool.get(source_ref);
		source->position = position;
	}
	void ALAudioManager::setSourceDirection(Ref<AudioSource> source_ref, glm::vec3 direction)
	{
		auto source = sourcesPool.get(source_ref);
		source->direction = direction;
	}
	void ALAudioManager::setSourceVelocity(Ref<AudioSource> source_ref, glm::vec3 velocity)
	{
		auto source = sourcesPool.get(source_ref);
		source->velocity = velocity;
	}
	void ALAudioManager::setSourceGain(Ref<AudioSource> source_ref, float gain)
	{
		auto source = sourcesPool.get(source_ref);
		source->gain = gain;
	}
	void ALAudioManager::setSourceAngle(Ref<AudioSource> source_ref, float angle)
	{
		auto source = sourcesPool.get(source_ref);
		source->angle = angle;
	}
	void ALAudioManager::setSourcePitch(Ref<AudioSource> source_ref, float pitch)
	{
		auto source = sourcesPool.get(source_ref);
		source->pitch = pitch;
	}

	void ALAudioManager::setSourceState(Ref<AudioSource> source_ref, AudioState state)
	{
		auto source = sourcesPool.get(source_ref);

		if (state == AudioState::PLAYING)
			al::playSource(*source);
		if (state == AudioState::PAUSED)
			al::pauseSource(*source);
		if (state == AudioState::STOPPED)
			al::stopSource(*source);
	}

	AudioState ALAudioManager::getSourceState(Ref<AudioSource> source_ref)
	{
		auto source = *sourcesPool.get(source_ref);
		auto state = al::getSourceState(source);
		if (state == AL_PLAYING)
			return AudioState::PLAYING;
		if (state == AL_STOPPED)
			return AudioState::STOPPED;
		if (state == AL_PAUSED)
			return AudioState::PAUSED;
	}

	glm::vec3 ALAudioManager::getSourcePosition(Ref<AudioSource> source_ref)
	{
		auto source = sourcesPool.get(source_ref);
		return source->position;
	}
	glm::vec3 ALAudioManager::getSourceDirection(Ref<AudioSource> source_ref)
	{
		auto source = sourcesPool.get(source_ref);
		return source->direction;
	}
	glm::vec3 ALAudioManager::getSourceVelocity(Ref<AudioSource> source_ref)
	{
		auto source = sourcesPool.get(source_ref);
		return source->velocity;
	}
	float ALAudioManager::getSourceGain(Ref<AudioSource> source_ref)
	{
		auto source = sourcesPool.get(source_ref);
		return source->gain;
	}
	float ALAudioManager::getSourceAngle(Ref<AudioSource> source_ref)
	{
		auto source = sourcesPool.get(source_ref);
		return source->angle;
	}
	float ALAudioManager::getSourcePitch(Ref<AudioSource> source_ref)
	{
		auto source = sourcesPool.get(source_ref);
		return source->pitch;
	}

	void ALAudioManager::deleteBuffer(Ref<AudioBuffer> buffer) {}
	void ALAudioManager::deleteSource(Ref<AudioSource> source) {}
	void ALAudioManager::deleteListener(Ref<AudioListener> listener) {}
}