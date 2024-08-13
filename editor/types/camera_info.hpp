#pragma once

namespace Editor {
	struct CameraCreationInfo {
		glm::vec3 position = glm::vec3(5.0f, 5.0f, 5.0f);
		glm::vec3 direction = glm::vec3(0.0f);
		float nearClip = 0.1f;
		float farClip = 500.0f;
		float Fov = 70.0f;
	};
}