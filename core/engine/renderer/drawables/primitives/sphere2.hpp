#pragma once

#include "primitive.hpp"

namespace engine
{
	//a rounded cube
	class Sphere2 : public Primitive
	{
		enum SIDE {
			BOTTOM = 0,
			RIGHT,
			FRONT,
			TOP,
			LEFT,
			BACK
		};
	public:
		Sphere2(uint32_t subdivisions) : mNumSubdivisions(subdivisions)
		{
			m_pType = DrawableType::SPHERE;
			createGeometry();
		}

		Sphere2() = default;

		void createGeometry() {


			for (int i = 0; i < 6; i++) {
				createPatch(SIDE(i));
			}


			for (int i = 0; i < 6; i++) {
				int edge = 0;
				int baseIndex = (m_pMesh.Vertices.size() / 6) * i;
				for (int j = 0; j < (mNumSubdivisions * mNumSubdivisions) - mNumSubdivisions; j++) {
					if (edge == mNumSubdivisions - 1) {
						edge = 0;
						continue;
					}
					if (i <= 2) {
						m_pMesh.Indices.push_back(baseIndex + j + 1);
						m_pMesh.Indices.push_back(baseIndex + j);
						m_pMesh.Indices.push_back(baseIndex + j + mNumSubdivisions);

						m_pMesh.Indices.push_back(baseIndex + j + 1);
						m_pMesh.Indices.push_back(baseIndex + j + mNumSubdivisions);
						m_pMesh.Indices.push_back(baseIndex + j + mNumSubdivisions + 1);
					}
					else {
						m_pMesh.Indices.push_back(baseIndex + j);
						m_pMesh.Indices.push_back(baseIndex + j + 1);
						m_pMesh.Indices.push_back(baseIndex + j + mNumSubdivisions);
						
						m_pMesh.Indices.push_back(baseIndex + j + mNumSubdivisions);
						m_pMesh.Indices.push_back(baseIndex + j + 1);
						m_pMesh.Indices.push_back(baseIndex + j + mNumSubdivisions + 1);

					}

					edge++;
				}
			}

			for (auto& vtx : m_pMesh.Vertices) {
				float l = glm::length(vtx.position);
				vtx.position = glm::normalize(vtx.position);
				vtx.normal = glm::normalize(vtx.position - glm::vec3(0.0));
				//vtx.texCoords *= 0.5f;
			}

			generateTangentSpaceVectors();
		}

		void createPatch(SIDE side) {
			for (int x = 0; x < mNumSubdivisions; x++) {
				for (int y = 0; y < mNumSubdivisions; y++) {
					glm::vec3 vtx;
					switch (int(side))
					{
					case TOP: {
						vtx = glm::vec3((float(x) / float(mNumSubdivisions - 1))  - 0.5f, 0.5f, (float(y) / float(mNumSubdivisions - 1)) - 0.5f);
						break;
					}
					case BOTTOM: {
						vtx = glm::vec3((float(x) / float(mNumSubdivisions - 1))  - 0.5f, -0.5f, (float(y) / float(mNumSubdivisions - 1))  - 0.5f);
						break;
					}
					case LEFT: {
						vtx = glm::vec3((float(x) / float(mNumSubdivisions - 1))  - 0.5f, (float(y) / float(mNumSubdivisions - 1))  - 0.5f, -0.5);
						break;
					}
					case RIGHT: {
						vtx = glm::vec3((float(x) / float(mNumSubdivisions - 1))  - 0.5f, (float(y) / float(mNumSubdivisions - 1)) - 0.5f, 0.5);
						break;
					}
					case FRONT: {
						vtx = glm::vec3(0.5f, (float(x) / float(mNumSubdivisions - 1))  - 0.5f, (float(y) / float(mNumSubdivisions - 1))  - 0.5f);
						break;
					}
					case BACK: {
						vtx = glm::vec3(-0.5f, (float(x) / float(mNumSubdivisions - 1))  - 0.5f, (float(y) / float(mNumSubdivisions - 1))  - 0.5f);
						break;
					}
					default:
						assert(false && "Impossible!");
						std::exit(-1);
						break;
					}

					glm::vec2 uv = glm::vec2((float(x) / float(mNumSubdivisions - 1)), float(y) / float(mNumSubdivisions - 1));
					m_pMesh.Vertices.push_back({ vtx, uv, glm::vec3(0.0, 1.0, 0.0) });
					vertexCount++;
				}
			}

		}


	private:
		uint32_t vertexCount = 0;
		uint32_t mNumSubdivisions = 4;
	};
}