#pragma once

#include "al_data.hpp"
#include <exception>

namespace al {
	[[nodiscard]] static OpenALBuffer
		createBuffer(int numChannels, std::size_t size, std::size_t bps, std::size_t bitrate, unsigned char* data) {

		OpenALBuffer buffer = {};
		ALenum format;
		alGenBuffers(1, &buffer.id);

		switch (bps) {
		case 8:
			format = (numChannels == 1) ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
			break;
		case 16:
			format = (numChannels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
			break;
		default:
			throw std::runtime_error("Audio format could not be decoded. abort.");
		}
		alBufferData(buffer.id, format, data, size, bitrate);
		return buffer;
	}

	[[nodiscard]] static OpenALSource createSource(OpenALBuffer buffer) {

		OpenALSource source;
		alGenSources(1, &source.id);
		alSourcei(source.id, AL_BUFFER, (ALint)buffer.id);

		alSourcei(source.id, AL_SOURCE_RELATIVE, AL_FALSE);
		alSourcef(source.id, AL_GAIN, 1.0);
		alSourcef(source.id, AL_PITCH, 1.0);
		alSource3f(source.id, AL_POSITION, 0, 0, 0);
		alSource3f(source.id, AL_DIRECTION, 0, 0, 0);
		alSourcef(source.id, AL_CONE_INNER_ANGLE, 360);
		alSourcef(source.id, AL_CONE_OUTER_ANGLE, 360);
		alSource3f(source.id, AL_VELOCITY, 0.0, 0.0, 0.0);


		return source;
	}

	static void deleteSource(OpenALSource source) {

		ALuint s = source.id;
		alSourceStop(source.id);
		if (alIsSource(source.id)) {
			alSourcei(source.id, AL_BUFFER, NULL);
			alDeleteSources(1, &s);
		}
	}

	static void deleteBuffer(OpenALBuffer buffer) {

		ALuint b = buffer.id;
		if (alIsBuffer(buffer.id))
			alDeleteBuffers(1, &b);
	}

	static int getSourceState(OpenALSource source) {

		ALint source_state;
		alGetSourcei(source.id, AL_SOURCE_STATE, &source_state);

		return source_state;
	}


	static void playSource(OpenALSource source) {

		//if (getSourceState(source) == AL_PLAYING) return;

		alSourcei(source.id, AL_SOURCE_RELATIVE, source.relative ? AL_TRUE : AL_FALSE);
		alSourcei(source.id, AL_LOOPING, source.looping ? AL_TRUE : AL_FALSE);
		alSourcef(source.id, AL_GAIN, source.gain);
		alSourcef(source.id, AL_PITCH, source.pitch);
		alSourcef(source.id, AL_MIN_GAIN, 0.01);
		alSourcef(source.id, AL_MAX_DISTANCE, 50.0);
		
		alSourcef(source.id, AL_REFERENCE_DISTANCE, 2.0);
		alSourcef(source.id, AL_CONE_OUTER_GAIN, 0.0);
		alSourcef(source.id, AL_ROLLOFF_FACTOR, 1.2);
		
		//alEffecti(source, )
		alSource3f(source.id, AL_POSITION, source.position.x, source.position.y, source.position.z);
		alSource3f(source.id, AL_DIRECTION, source.direction.x, source.direction.y, source.direction.z);
		alSourcePlay(source.id);
	}

	static void pauseSource(OpenALSource source) {

		alSourcePause(source.id);
	}

	static void stopSource(OpenALSource source) {

		alSourceStop(source.id);
	}
	/*
	class AudioSource : public Epsilon::Audio::AudioSource
	{
	public:
		AudioSource(const char *fileName, AUDIO_TYPE type, glm::vec3 AudioPosition, glm::vec3 AudioDirection) : Audio::AudioSource(fileName, type, AudioPosition, AudioDirection)
		{

			try
			{

				this->m_Type = type;
				this->m_Position = AudioPosition;
				this->m_Direction = AudioDirection;

				mAudioResource = std::make_shared<Resources::Audio>(fileName);
				data = mAudioResource->Data(); //(std::shared_ptr<char>)SoundFile.Load(fileName);


				this->m_Channels = mAudioResource->Channels(); //SoundFile.getNumberOfChannels();
				std::cout << "Number of Channels: " << this->m_Channels << std::endl;

				this->m_BytesPerSecond = mAudioResource->BitsPerSecond();
				std::cout << "Bytes per Second: " << this->m_BytesPerSecond << std::endl;

				if (this->m_Channels == 1)
				{
					this->m_Format = this->m_BytesPerSecond == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
				}
				else
				{
					this->m_Format = this->m_BytesPerSecond == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
				}

				//alBufferSamplesSOFT(this->m_BufferID, SoundFile.getSampleRate(), this->m_Format, SoundFile.getFileSize(),this->m_Channels, AL_UNSIGNED_BYTE_SOFT, data.get());
				alBufferData(m_BufferID, m_Format, data.get(), mAudioResource->FileSize(), mAudioResource->BitRate());

				alGenSources(1, &this->m_AudioID);

				if (this->m_Type == MUSIC)
				{
					alSourcei(m_AudioID, AL_SOURCE_RELATIVE, AL_TRUE);
					alSourcef(m_AudioID, AL_GAIN, 1);
					alSourcef(m_AudioID, AL_PITCH, 1);
					alSource3f(m_AudioID, AL_POSITION, 0, 0, 0);
					alSource3f(m_AudioID, AL_DIRECTION, 0, 0, 0);
					alSourcef(m_AudioID, AL_CONE_INNER_ANGLE, 360);
					alSourcef(m_AudioID, AL_CONE_OUTER_ANGLE, 360);
					alSource3f(m_AudioID, AL_VELOCITY, 0.0, 0.0, 0.0);
				}
				else if (this->m_Type == STATIC_SOUND)
				{

					alSourcei(m_AudioID, AL_SOURCE_RELATIVE, AL_FALSE);
					alSourcef(m_AudioID, AL_GAIN, 1);
					alSourcef(m_AudioID, AL_PITCH, 1);
					alSource3f(m_AudioID, AL_POSITION, m_Position.x, m_Position.y, m_Position.z);
					alSource3f(m_AudioID, AL_DIRECTION, m_Direction.x, m_Direction.y, m_Direction.z);
					alSourcef(m_AudioID, AL_CONE_INNER_ANGLE, 360);
					alSourcef(m_AudioID, AL_CONE_OUTER_ANGLE, 360);
					alSource3f(m_AudioID, AL_VELOCITY, 0.0, 0.0, 0.0);
				}
				else if (this->m_Type == DYNAMIC_SOUND)
				{
					alSourcei(m_AudioID, AL_SOURCE_RELATIVE, AL_FALSE);
					alSourcef(m_AudioID, AL_GAIN, 1);
					alSourcef(m_AudioID, AL_PITCH, 1);
					alSource3f(m_AudioID, AL_POSITION, m_Position.x, m_Position.y, m_Position.z);
					alSource3f(m_AudioID, AL_DIRECTION, m_Direction.x, m_Direction.y, m_Direction.z);
					alSourcef(m_AudioID, AL_CONE_INNER_ANGLE, 360);
					alSourcef(m_AudioID, AL_CONE_OUTER_ANGLE, 360);
					alSource3f(m_AudioID, AL_VELOCITY, 0.0, 0.0, 0.0);
				}

				alSourcei(m_AudioID, AL_BUFFER, m_BufferID);
			}
			catch (std::exception &e)
			{
				std::cout << "Exception while loading: " << fileName << "\nwhat(): " << e.what() << std::endl;
			}
		}

		void Destroy() override
		{

#if !defined(ANDROID) && !defined(__ANDROID__)
			std::cout << "Deleted AudioSource" << std::endl;
			//char* tmpData = data.reset();
			//delete tmpData;
			//tmpData = nullptr;
			alDeleteSources(1, &m_AudioID);
			alDeleteBuffers(1, &m_BufferID);
#endif
		}

		~AudioSource()
		{
		}

	public:
		void Play() override
		{

#if !defined(ANDROID) && !defined(__ANDROID__)
			ALint source_state;
			alGetSourcei(m_AudioID, AL_SOURCE_STATE, &source_state);

			if (source_state != AL_PLAYING)
			{

				if (this->m_Type == DYNAMIC_SOUND)
				{
					alSourcei(m_AudioID, AL_LOOPING, AL_TRUE);
					alSourcei(m_AudioID, AL_SOURCE_RELATIVE, AL_FALSE);
					alSourcef(m_AudioID, AL_GAIN, 1);
					alSourcef(m_AudioID, AL_PITCH, 1);
					alSource3f(m_AudioID, AL_POSITION, m_Position.x, m_Position.y, m_Position.z);
					alSource3f(m_AudioID, AL_DIRECTION, m_Direction.x, m_Direction.y, m_Direction.z);
					alSourcef(m_AudioID, AL_CONE_INNER_ANGLE, 360);
					alSourcef(m_AudioID, AL_CONE_OUTER_ANGLE, 360);
					alSource3f(m_AudioID, AL_VELOCITY, 0.0, 0.0, 0.0);
					alSourcePlay(m_AudioID);
				}
				else if (this->m_Type == STATIC_SOUND)
				{
					alSourcei(m_AudioID, AL_LOOPING, AL_TRUE);
					alSourcePlay(m_AudioID);
				}
				else if (this->m_Type == MUSIC)
				{
					alSourcei(m_AudioID, AL_LOOPING, AL_TRUE);
					alSourcePlay(m_AudioID);
				}
			}

			if (this->m_Type == MENU_SOUND)
			{
				//if(source_state != AL_PLAYING){
				alSourcef(m_AudioID, AL_GAIN, 0);
				alSourceStop(m_AudioID);
				//}
				//else{
				alSourcei(m_AudioID, AL_LOOPING, AL_FALSE);
				alSourcePlay(m_AudioID);
				alSourcef(m_AudioID, AL_GAIN, 1);
				//}
			}
		}

		void Pause() override
		{
#if !defined(ANDROID) && !defined(__ANDROID__)
			ALint source_state;
			alGetSourcei(m_AudioID, AL_SOURCE_STATE, &source_state);

			if (source_state == AL_PLAYING)
			{
				alSourcePause(m_AudioID);
			}
		}

		void Stop() override
		{
#if !defined(ANDROID) && !defined(__ANDROID__)
			ALint source_state;
			alGetSourcei(m_AudioID, AL_SOURCE_STATE, &source_state);

			if (source_state == AL_PLAYING)
			{
				alSourceStop(m_AudioID);
			}
		}

		AUDIO_TYPE getType()
		{
			return m_Type;
		}

		void setPosition(glm::vec3 position) override
		{
#if !defined(ANDROID) && !defined(__ANDROID__)
			m_Position = position;
			alSource3f(m_AudioID, AL_POSITION, m_Position.x, m_Position.y, m_Position.z);
			//alSourcePlay(m_AudioID);
		}

		glm::vec3 getPosition() override
		{
			return m_Position;
		}

		void setDirection(glm::vec3 dir) override {
			m_Direction = dir;
		}

		virtual glm::vec3 getDirection() override {
			return m_Direction;
		}

	private:
		unsigned int m_AudioID;
		unsigned int m_BufferID;
		float m_Volume;
		int m_Channels;
		unsigned int m_Format;
		int m_BytesPerSecond;
		std::shared_ptr<unsigned char> data;

		AUDIO_TYPE m_Type;

		glm::vec3 m_Position;
		glm::vec3 m_Direction;
		glm::vec3 m_Velocity;
		float m_Radius;

		std::shared_ptr<Resources::Audio> mAudioResource;
	};*/
} // namespace Epsilon