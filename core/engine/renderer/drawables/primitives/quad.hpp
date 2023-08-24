//
// Created by solaire on 12/14/22.
//

#ifndef LEARNINGVULKAN_QUAD_HPP
#define LEARNINGVULKAN_QUAD_HPP

#include "primitive.hpp"
namespace engine
{
	class Quad : public Primitive
	{
		using vtx = common::Vertex;
		using v4 = glm::vec4;
		using v3 = glm::vec3;
		using v2 = glm::vec2;
		
		struct {
			glm::vec3 min;
			glm::vec3 max;
			glm::vec3 center;
		} bounds;

	public:

		Quad(uint16_t tesselation = 2, glm::vec2 uvMultiplier = glm::vec2(1.f)) : m_pTesselation(tesselation) {

			m_pType = DrawableType::QUAD;

			if (tesselation <= 2) {
				pInit();
				return;
			}

			float step = 1.0f / ((float)tesselation - 1.0f);
			int numVertices = tesselation * tesselation;
			bounds.min = glm::vec3(100000.0f);
			bounds.max = glm::vec3(-100000.0f);

			for (int i = 0; i < tesselation; i++) {
				for (int j = 0; j < tesselation; j++) {
					float x = (float)i;
					float y = (float)j;
					v3 position = v3(x * step, 0.0f, y * step);
					v2 uv = v2(x, y) * uvMultiplier;
					v3 normal = v3(0.0f, 1.0f, 0.0f);

					position -= v3(0.5f, 0.0f, 0.5f);

					if (position.x < bounds.min.x) bounds.min.x = position.x;
					if (position.x > bounds.max.x) bounds.max.x = position.x;

					if (position.z < bounds.min.z) bounds.min.z = position.z;
					if (position.z > bounds.max.z) bounds.max.z = position.z;

					m_pMesh.Vertices.emplace_back(vtx(position, uv, normal, v4(0.0f), v3(0.0f), v3(0.0f)));
				}
			}
			bounds.min.y = 0.0f;
			bounds.max.y = 1.0f;

			bounds.center = bounds.max - bounds.min;

			int edge = 0;
			for (int i = 0; i < numVertices - tesselation; i++) {
				if (edge >= tesselation - 1) {
					edge = 0;
					continue;
				}
				m_pMesh.addTriangle(i, i + 1, i + tesselation);
				m_pMesh.addTriangle(i + tesselation, i + 1, i + tesselation + 1);
				edge++;
			}

			generateTangentSpaceVectors();
		}

		float getHeight(glm::vec3 position, glm::vec3 scale) {
			glm::ivec3 pos = glm::floor(position-1.0f);

			int x_a = pos.x + m_pTesselation / 2;
			int x_b = 0;

			int y_a = pos.z + m_pTesselation / 2;
			int y_b = 0;

			if (x_a >= m_pTesselation) {
				x_b = x_a;
				x_a = x_a - 1;
			}
			else {
				x_b = x_a + 1;
			}

			if (y_a >= m_pTesselation) {
				y_b = y_a;
				y_a = y_a - 1;
			}
			else {
				y_b = y_a + 1;
			}

			if (y_a <= 0) {
				y_a = 0;
				y_b = 1;
			}

			if (x_a <= 0) {
				x_a = 0;
				x_b = 1;
			}

			glm::vec3 a, b, c, d;
			a = m_pMesh.Vertices.at(x_a * m_pTesselation + y_a).position * scale;// a.x = a.x * scale.x; a.z = a.z * scale.z;
			b = m_pMesh.Vertices.at(x_b * m_pTesselation + y_a).position * scale;// b.x = b.x * scale.x; b.z = b.z * scale.z;
			c = m_pMesh.Vertices.at(x_a * m_pTesselation + y_b).position * scale;// c.x = c.x * scale.x; c.z = c.z * scale.z;
			d = m_pMesh.Vertices.at(x_b * m_pTesselation + y_b).position * scale;// d.x = d.x * scale.x; d.z = d.z * scale.z;

			float la = glm::distance(position, a);
			float lb = glm::distance(position, b);
			float lc = glm::distance(position, c);
			float ld = glm::distance(position, d);

			float tx = lb - la;
			float ty = ld - lc;

			return ((a.y * tx + b.y * (1.0 - tx) + c.y * ty + d.y * (1.0- ty)) * 0.5f)-0.15;
		}

		void calculateNormals() {

			std::vector<std::vector<float>> Grid;
			std::vector<float> row;
			for (int i = 0; i < m_pTesselation; i++)
			{
				for (int j = 0; j < m_pTesselation; j++)
				{
					row.push_back(m_pMesh.Vertices.at(i * m_pTesselation + j).position.y);
				}
				Grid.push_back(row);
				row.clear();
			}

			std::vector<glm::vec3> Normals;
			for (int i = 0; i < m_pTesselation; i++)
			{
				for (int j = 0; j < m_pTesselation; j++)
				{
					if (i > 0 && j > 0 && i < m_pTesselation - 1 && j < m_pTesselation - 1)
					{
						float HL = Grid.at(i - 1).at(j);
						float HR = Grid.at(i + 1).at(j);
						float HD = Grid.at(i).at(j - 1);
						float HU = Grid.at(i).at(j + 1);


						glm::vec3 norm = glm::normalize(glm::vec3(HL - HR, 0.1, HD - HU));
						Normals.push_back(norm);
						//m_pMesh.Vertices.at(i * m_pTesselation + j).normal = norm;

					}
					else
					{

						Normals.push_back(glm::vec3(0, 1, 0));
					}
				}
			}
			int index = 0;
			for (auto& vtx : m_pMesh.Vertices) {
				vtx.normal = glm::normalize(Normals[index]);
				index++;
			}

			generateTangentSpaceVectors();
		}

	private:
		void pInit() {
			m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v2(-1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f))); // bottom-left
			m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v2(0.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));  // bottom-right
			m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));   // top-right

			m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));   // top-right
			m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v2(-1.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));  // top-left
			m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v2(-1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 0.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f))); // bottom-left

			for (int i = 0; i < m_pMesh.Vertices.size(); i += 3)
			{
				m_pMesh.addTriangle(i, i + 1, i + 2);
			}

			generateTangentSpaceVectors();
		}


		uint32_t m_pTesselation = 2;
	};
}

#endif // LEARNINGVULKAN_QUAD_HPP
