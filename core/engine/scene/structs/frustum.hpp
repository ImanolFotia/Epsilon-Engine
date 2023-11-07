#pragma once

#include "box.hpp"

#include <list>

#include <glm/glm.hpp>

#include <core/common/common.hpp>

namespace engine
{
	class Frustum
	{
	public:
		// Call this every time the camera moves to update the frustum
		void CalculateFrustum(glm::mat4 ProjectionMatrix, glm::mat4 ModelMatrix);

		// This takes a 3D point and returns TRUE if it's inside of the frustum
		bool PointInFrustum(float x, float y, float z);

		// This takes a 3D point and a radius and returns TRUE if the sphere is
		// inside of the frustum
		bool SphereInFrustum(float x, float y, float z, float radius);

		/**
		 * @brief  This takes the center and half the length of the cube.
		 */
		bool CubeInFrustum(float x, float y, float z, glm::vec3 size);
		bool CubeInFrustum(float x, float y, float z, float size);
		bool BoxInFrustum(float x, float y, float z, float x2, float y2, float z2) const;
		bool FrustumContainsCube(float x, float y, float z, glm::vec3 size);

		/////// * /////////// * /////////// * NEW * /////// * /////////// *
		////////////// *

		// This checks if a box is in the frustum
		bool BoxInFrustum(common::MIN_MAX_POINTS points) const;

		bool contains(Box box) {
			auto center = box.center();
			return FrustumContainsCube(center.x, center.y, center.z, box.halfSize());
		}

		bool overlaps(Box box) {
			auto center = box.center();
			return CubeInFrustum(center.x, center.y, center.z, box.halfSize());
		}

		/////// * /////////// * /////////// * NEW * /////// * /////////// *
		////////////// *

	private:
		// This holds the A B C and D values for each side of our frustum.
		float m_Frustum[6][4] = { {1}, {1} };
	};

	/**
	 * @brief Array of cuadrants offsets, used for calculatin where to spawn new children
	 *
	 */
	const glm::vec3 CUADRANT[] = {
		glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0.0),
		glm::vec3(0.0, 1.0, 1.0), glm::vec3(0.0, 1.0, 0.0),
		glm::vec3(1.0, 0.0, 1.0), glm::vec3(1.0, 0.0, 0.0),
		glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, 0.0, 0.0) };

	/**
	 * @brief Max number of children the Octree can have
	 *
	 */
	const int MAX_DEPTH = 4;

	/**
	 * @brief struct to hold the location of an item in the octree, in the octree container
	 *        object's list.
	 *        Read that as many times as you need
	 *
	 * @tparam T the data type of objects the octree holds
	 */
	template <typename T>
	struct OctreeItemLocation
	{
		using underlying_type = std::list<std::pair<Box, T>>;
		typename std::list<std::pair<Box, T>>* container;
		typename underlying_type::iterator iterator;
	};
}