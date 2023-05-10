//
// Created by solaire on 1/20/23.
//

#ifndef EPSILON_AL_DATA_HPP
#define EPSILON_AL_DATA_HPP

#if (!defined(ANDROID) && !defined(__ANDROID__))

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>

#include <glm/glm.hpp>

namespace al {

	struct OpenALBuffer {
		ALuint id;
	};

	struct OpenALSource {
		ALuint id;
		OpenALBuffer buffer;
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 velocity;
		float angle;
		float pitch;
		float gain;
		bool looping;
		bool playing;
		bool relative;
	};
	struct OpenALListener {
		glm::vec3 position;
		glm::vec3 direction;
		glm::vec3 up;
		glm::vec3 velocity;
		float gain;
	};
	struct OpenALData
	{
		ALCdevice* device;
		ALCcontext* context;
	};
}
#endif

#endif // EPSILON_AL_DATA_HPP
