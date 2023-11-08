//
// Created by solaire on 1/20/23.
//

#ifndef EPSILON_AL_DATA_HPP
#define EPSILON_AL_DATA_HPP


#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>
#include <AL/efx.h>
#include <AL/efx-creative.h>
#include <AL/efx-presets.h>

#include <glm/glm.hpp>

namespace al {

	struct OpenALBuffer {
		ALuint id{};
	};

	struct OpenALSource {
		ALuint id{};
		OpenALBuffer buffer{};
		glm::vec3 position {};
		glm::vec3 direction{};
		glm::vec3 velocity{};
		float angle{};
		float pitch = 1.0f;
		float gain = 1.0f;
		float inner_angle = 1.0;
		float outer_angle = 1.0;
		float cutoff_distance = 5.0;

		bool looping = false;
		bool playing = false;
		bool relative = false;
	};
	struct OpenALListener {
		glm::vec3 position{};
		glm::vec3 direction{};
		glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 velocity{};
		float gain = 0.9f;
	};
	struct OpenALData
	{
		ALCdevice* device;
		ALCcontext* context;
		bool shouldReloadDevice = false;
	};
}

#endif // EPSILON_AL_DATA_HPP
