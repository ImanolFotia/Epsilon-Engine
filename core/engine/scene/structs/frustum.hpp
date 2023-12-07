#pragma once

#include "box.hpp"

#include <list>

#include <glm/glm.hpp>
#include <array>

#include <core/common/common.hpp>
#include "sphere.hpp"

#include <core/framework/containers/static_array.hpp>

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

		std::array<glm::vec4, 8> getFrustumPoints() {
			corners;
		}

		BoundingSphere calculateSphere() {
			glm::vec3 vmin{}, vmax{};

			std::array<glm::vec3, 8> vertices;
			int i = 0;
			for (auto& vertex : vertices) { vertex = corners[i]; i++; }

			vmin = vmax = vertices[0];  // assign first vertex as minimum and maximum
			int num_vertices = vertices.size();

			for (int i = 1; i < num_vertices;
				i++)  // find min and max x, y, and z values
			{
				if (vertices[i].x < vmin.x) vmin.x = vertices[i].x;
				if (vertices[i].y < vmin.y) vmin.y = vertices[i].y;
				if (vertices[i].z < vmin.z) vmin.z = vertices[i].z;

				if (vertices[i].x > vmax.x) vmax.x = vertices[i].x;
				if (vertices[i].y > vmax.y) vmax.y = vertices[i].y;
				if (vertices[i].z > vmax.z) vmax.z = vertices[i].z;
			}
			float xdiff = vmax.x - vmin.x;
			float ydiff = vmax.y - vmin.y;
			float zdiff = vmax.z - vmin.z;

			float diameter =
				glm::max(xdiff, glm::max(ydiff, zdiff));  // take max as diameter
			glm::vec3 centre = (vmax + vmin) * (0.5f);
			float radius = diameter / 2;
			float sq_radius = radius * radius;

			for (int i = 0; i < num_vertices; i++) {
				glm::vec3 point = vertices[i];

				glm::vec3 direction = glm::normalize(point - centre);
				float sq_distance = glm::length(direction);
				sq_distance = sq_distance * sq_distance;

				if (sq_distance > sq_radius) {
					float distance = sqrt(sq_distance);

					float difference = distance - radius;

					float new_diameter = 2 * radius + difference;
					radius = new_diameter / 2;
					sq_radius = radius * radius;

					difference /= 2;

					centre += difference * direction;
				}
			}

			return BoundingSphere(radius, centre);
		}

		/////// * /////////// * /////////// * NEW * /////// * /////////// *
		////////////// *

	private:
		// This holds the A B C and D values for each side of our frustum.
		float m_Frustum[6][4] = { {1}, {1} };
		glm::vec3 min{};
		glm::vec3 max{};
		std::array<glm::vec4, 8> corners;
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