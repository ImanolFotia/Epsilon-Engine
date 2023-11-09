#pragma once
#include <glm/glm.hpp>
#include <iostream>
#include "box.hpp"
/** @brief Box structure with some helper functions
 * This box structure is the building block of the octree container
 * The box consists of a size and a position, the later representinf the bottom left corner
 * */
namespace engine
{
	struct BoundingSphere {
		float radius = 0.0f;
		glm::vec3 position{};
		BoundingSphere(float rad, glm::vec3 pos) : radius{ rad }, position{pos} {}

		bool contains(Box& box) {
			if (box.halfSize().z > radius || box.halfSize().z > radius || box.halfSize().z > radius) return false;

			if (!containsPoint(box.position, position)) return false;
			if (!containsPoint(box.position + box.size, position)) return false;
			if (!containsPoint(box.position + glm::vec3(box.position.x + box.size.x, box.position.y, box.position.z), position)) return false;
			if (!containsPoint(box.position + glm::vec3(box.position.x , box.position.y + box.size.y, box.position.z ), position)) return false;
			if (!containsPoint(box.position + glm::vec3(box.position.x , box.position.y, box.position.z + box.size.z), position)) return false;
			if (!containsPoint(box.position + glm::vec3(box.position.x + box.size.x, box.position.y + box.size.y, box.position.z), position)) return false;
			if (!containsPoint(box.position + glm::vec3(box.position.x + box.size.x, box.position.y, box.position.z + box.size.z), position)) return false;
			if (!containsPoint(box.position + glm::vec3(box.position.x, box.position.y + box.size.y, box.position.z + box.size.z), position)) return false;

			return true;
		}

		bool containsPoint(glm::vec3 a, glm::vec3 b) {
			return glm::distance(a, b) <= radius;
		}

		bool overlaps(Box& box) {

			if (containsPoint(box.position, position)) return true;
			if (containsPoint(box.position + box.size, position)) return true;
			if (containsPoint(box.position + glm::vec3(box.position.x + box.size.x, box.position.y, box.position.z), position)) return true;
			if (containsPoint(box.position + glm::vec3(box.position.x, box.position.y + box.size.y, box.position.z), position)) return true;
			if (containsPoint(box.position + glm::vec3(box.position.x, box.position.y, box.position.z + box.size.z), position)) return true;
			if (containsPoint(box.position + glm::vec3(box.position.x + box.size.x, box.position.y + box.size.y, box.position.z), position)) return true;
			if (containsPoint(box.position + glm::vec3(box.position.x + box.size.x, box.position.y, box.position.z + box.size.z), position)) return true;
			if (containsPoint(box.position + glm::vec3(box.position.x, box.position.y + box.size.y, box.position.z + box.size.z), position)) return true;
		}
	};
}