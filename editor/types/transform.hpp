#pragma once

#include <glm/glm.hpp>

namespace Editor {

	struct Transform
	{
		float position[3] = { 1.0, 2.0, 3.0 };
		float rotation[4] = { 7.0, 8.0, 9.0, 1.0 };
		float scale[3] = { 4.0, 5.0, 6.0 };

		glm::mat4 toMat4() {

			glm::mat4 t = glm::mat4(1.0f);
			t = glm::translate(glm::mat4(1.0f), glm::vec3(position[0], position[1], position[2]));
			t *= glm::toMat4(glm::quat(rotation[3], rotation[0], rotation[1], rotation[2])); 
			t *= glm::scale(glm::mat4(1.0f), glm::vec3(scale[0], scale[1], scale[2]));

			return t;
		}
	};
}