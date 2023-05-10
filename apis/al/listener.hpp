#pragma once

#include <glm/glm.hpp>
#include "al_data.hpp"

namespace al {
	static const float* toOrientation(glm::vec3 dir, glm::vec3 up) {

	}

	static void setListenerPosition(const OpenALListener& listener) {

#if !defined(ANDROID) && !defined(__ANDROID__)
		alListener3f(AL_POSITION, listener.position.x, listener.position.y, listener.position.z);
#endif
	}


	static void setListenerVelocity(const OpenALListener& listener) {

#if !defined(ANDROID) && !defined(__ANDROID__)
		alListener3f(AL_VELOCITY, listener.velocity.x, listener.velocity.y, listener.velocity.z);
#endif
	}

	static void setListenerGain(const OpenALListener& listener) {

#if !defined(ANDROID) && !defined(__ANDROID__)
		alListenerf(AL_GAIN, listener.gain);
#endif
	}

	static void setListenerOrientation(const OpenALListener& listener) {
#if !defined(ANDROID) && !defined(__ANDROID__)
		ALfloat orientation[6] = { 0 };
		orientation[0] = listener.direction.x;
		orientation[1] = listener.direction.y;
		orientation[2] = listener.direction.z;
		orientation[3] = listener.up.x;
		orientation[4] = listener.up.y;
		orientation[5] = listener.up.z;
		alListenerfv(AL_ORIENTATION, orientation);
#endif
	}


}