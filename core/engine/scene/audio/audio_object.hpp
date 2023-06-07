#pragma once

#include <core/framework/audio/WAVFile.h>
#include <core/engine/audio/audio_manager.hpp>

namespace engine {

	struct AudioBuffer
	{
		Ref<audio::AudioBuffer> buffer;
		std::vector<Ref<audio::AudioSource>> sources;
	};

	class AudioObject
	{

		Ref<audio::AudioSource> source{};
		AudioBuffer buffer{};
		float angle{};
		float gain{};
		glm::vec3 position{};
		glm::vec3 direction{};
		glm::vec3 velocity{};
		float pitch{};

		friend class AssetManager;

	public:
		// Setters
		void Position(glm::vec3 p)
		{
			position = p;
			//auto audioManager = Context::getSingleton().AudioManager();
			//audioManager->setSourcePosition(source, p);
		}

		void Direction(glm::vec3 d)
		{
			direction = d;
			//auto audioManager = Context::getSingleton().AudioManager();
			//audioManager->setSourceDirection(source, d);
		}

		void Velocity(glm::vec3 v)
		{
			velocity = v;
			//auto audioManager = Context::getSingleton().AudioManager();
			//audioManager->setSourceVelocity(source, v);
		}

		void Angle(float a)
		{
			angle = a;
			//auto audioManager = Context::getSingleton().AudioManager();
			//audioManager->setSourceAngle(source, a);
		}

		void Gain(float g)
		{
			gain = g;
			//auto audioManager = Context::getSingleton().AudioManager();
			//audioManager->setSourceGain(source, g);
		}

		void Pitch(float p)
		{
			pitch = p;
			//auto audioManager = Context::getSingleton().AudioManager();
			//audioManager->setSourcePitch(source, p);
		}

		// Getters
		glm::vec3 Position()
		{
			return position;
			//auto audioManager = Context::getSingleton().AudioManager();
			//return audioManager->getSourcePosition(source);
		}

		glm::vec3 Direction()
		{
			return direction;
			//auto audioManager = Context::getSingleton().AudioManager();
			//return audioManager->getSourceDirection(source);
		}

		glm::vec3 Velocity()
		{
			return velocity;
			//auto audioManager = Context::getSingleton().AudioManager();
			//return audioManager->getSourceVelocity(source);
		}

		float Angle()
		{
			return angle;
			//auto audioManager = Context::getSingleton().AudioManager();
			//return audioManager->getSourceAngle(source);
		}

		float Gain()
		{
			return gain;
			//auto audioManager = Context::getSingleton().AudioManager();
			//return audioManager->getSourceGain(source);
		}

		float Pitch()
		{
			return pitch;
			//auto audioManager = Context::getSingleton().AudioManager();
			//return audioManager->getSourcePitch(source);
		}
	};
}