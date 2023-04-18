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
		using vtx = Vertex;
		using v4 = glm::vec4;
		using v3 = glm::vec3;
		using v2 = glm::vec2;

	public:

		Quad(uint16_t tesselation = 2) {

			m_pType = DrawableType::QUAD;

			if (tesselation <= 2) {
				pInit();
				return;
			}

			float step = 1.0f / ((float)tesselation - 1.0f);
			int numVertices = tesselation * tesselation;

			for (int i = 0; i < tesselation; i++) {
				for (int j = 0; j < tesselation; j++) {
					float x = (float)i;
					float y = (float)j;
					v3 position = v3(x * step, 0.0f, y * step);
					v2 uv = v2(x, y);
					v3 normal = v3(0.0f, 1.0f, 0.0f);

					position -= v3(0.5f, 0.0f, 0.5f);

					m_pMesh.Vertices.emplace_back(vtx(position, uv, normal, v4(0.0f), v3(0.0f), v3(0.0f)));
				}
			}

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

	private:
		void pInit() {
			m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f))); // bottom-left
			m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, -1.0f, 1.0f), v2(1.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));  // bottom-right
			m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v2(1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));   // top-right

			m_pMesh.Vertices.emplace_back(vtx(v3(1.0f, 1.0f, 1.0f), v2(1.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));   // top-right
			m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, 1.0f, 1.0f), v2(0.0f, 1.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f)));  // top-left
			m_pMesh.Vertices.emplace_back(vtx(v3(-1.0f, -1.0f, 1.0f), v2(0.0f, 0.0f), v3(0.0f, 0.0f, 1.0f), v4(0.0f, 1.0f, 0.0f, 1.0f), v3(0.0f), v3(0.0f))); // bottom-left

			for (int i = 0; i < m_pMesh.Vertices.size(); i += 3)
			{
				m_pMesh.addTriangle(i, i + 1, i + 2);
			}

			generateTangentSpaceVectors();
		}
	};
}

#endif // LEARNINGVULKAN_QUAD_HPP
