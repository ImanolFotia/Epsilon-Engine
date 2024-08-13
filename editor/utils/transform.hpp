#pragma once

#include <glm/glm.hpp>
#include "../types/transform.hpp"

namespace Editor {
	static Transform toTransform(glm::mat4 t) {
		Transform transform;
		glm::vec3 scale, position;
		glm::quat rotation;
		glm::vec3 a;
		glm::vec4 b;
		glm::decompose(t, scale, rotation, position, a, b);
		transform.scale[0] = scale.x;
		transform.scale[1] = scale.y;
		transform.scale[2] = scale.z;

		transform.position[0] = position.x;
		transform.position[1] = position.y;
		transform.position[2] = position.z;

		transform.rotation[0] = rotation.x;
		transform.rotation[1] = rotation.y;
		transform.rotation[2] = rotation.z;
		transform.rotation[3] = rotation.w;
		return transform;
	}
}