#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <map>
#include <glm/glm.hpp>
#include <regex>

#include "bsp_definitions.hpp"
#include "utils/utils.hpp"

struct Face
{
	unsigned int m_pIndex;
	bool m_pVisible;
	unsigned int m_pNumVertices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;
	std::string material;
	bool tool = false;
	bool trigger = false;
	bool isDisplacement = false;
};

struct BSPFileData
{
	dheader_t header;
	std::vector<dedge_t> edges;
	std::vector<glm::vec3> vertices;
	std::vector<int> surfEdges;
	std::vector<dface_t> faces;
	std::vector<texinfo_t> texInfo;
	std::vector<dtexdata_t> texData;
	std::vector<int> stringTable;
	std::vector<ddispinfo_t> displacements;
	std::vector<dDispVert> vertDisps;
	std::vector<dnode_t> nodes;
	std::vector<char> entities;
	//std::vector<dleaf_t> leafs;

	char stringData[MAX_MAP_TEXDATA_STRING_DATA];
};
class SourceBSP
{
	std::string m_pFilename{};
	BSPFileData m_pBspFileData;
	std::ifstream m_pBspFile;
	std::unordered_map<std::string, std::vector<Face>> m_pFaces;
	uint32_t m_pNumMaterials = 0;

public:
	SourceBSP(std::string filename) : m_pFilename(filename)
	{
		m_pBspFile = std::ifstream(filename, std::ios::binary);

		if (!m_pBspFile.is_open()) {

			std::cerr << "Couldn't open BSP file" << std::endl;
			exit(9);
		}

		m_pBspFile.read((char*)&m_pBspFileData.header, sizeof(dheader_t));

		if (m_pBspFileData.header.ident != IDBSPHEADER)
		{
			std::cerr << "Not a valid Source BSP file" << std::endl;
			exit(9);
		}

		//parseLump(0, m_pBspFileData.entities);
		parseLump(LUMP_VERTEXES, m_pBspFileData.vertices);
		parseLump(LUMP_EDGES, m_pBspFileData.edges);
		parseLump(LUMP_SURFEDGES, m_pBspFileData.surfEdges);
		parseLump(LUMP_FACES, m_pBspFileData.faces);
		parseLump(LUMP_TEXINFO, m_pBspFileData.texInfo);
		parseLump(LUMP_TEXDATA, m_pBspFileData.texData);
		parseLump(LUMP_STRINGTABLE, m_pBspFileData.stringTable);
		parseLump(LUMP_DISPINFO, m_pBspFileData.displacements);
		parseLump(LUMP_DISPVERTS, m_pBspFileData.vertDisps);
		parseLump(LUMP_NODES, m_pBspFileData.nodes);
		//parseLump(LUMP_LEAFS, m_pBspFileData.leafs);

		auto& stringLump = m_pBspFileData.header.lumps[LUMP_STRINGDATA];
		m_pBspFile.seekg(stringLump.fileofs);
		m_pBspFile.read((char*)m_pBspFileData.stringData, stringLump.filelen);

		parseGeometry();

		/*for (auto& c : m_pBspFileData.entities) {
			std::cout << c;
		}

		std::cout << "\n";
		*/
		m_pNumMaterials = m_pFaces.size();

		std::cout << "there are " << m_pNumMaterials << " materials\n";

		m_pBspFile.close();
	}

	const std::vector<glm::vec3>& Vertices() const
	{
		return m_pBspFileData.vertices;
	}

	const std::unordered_map<std::string, std::vector<Face>>& Faces()
	{
		return m_pFaces;
	}

	auto numMaterials() -> uint32_t {
		return m_pNumMaterials;
	}

	std::vector<unsigned int> generateIndices(std::vector<unsigned int> inIndices)
	{
		std::map<unsigned short, unsigned short> indices;
		std::vector<unsigned int> listIndices;
		std::vector<unsigned int> outIndices;

		for (auto& index : inIndices)
		{
			if (!indices.contains(index))
			{
				indices[index] = indices.size();
			}

			listIndices.push_back(indices[index]);
			outIndices.push_back(indices[index]);
		}

		return outIndices;
	}

	void parseGeometry()
	{

		unsigned int index = 0;
		std::string material;
		for (auto& face : m_pBspFileData.faces)
		{
			// if(face.numPrims > 0) continue;
			bool isTool = false;
			bool isTrigger = false;
			bool isDisp = false;
			std::vector<glm::vec3> vtxs;
			std::vector<glm::vec2> uv;
			std::vector<unsigned int> triangles;

			material = std::string_view(
				&m_pBspFileData.stringData[m_pBspFileData.stringTable[m_pBspFileData.texData[m_pBspFileData.texInfo[face.texinfo].texdata].nameStringTableID]]);

			try {
				material = std::regex_replace(material, std::regex("\\maps/background01/"), "");
				material = std::regex_replace(material, std::regex("(_-?\\d+)+"), "");
				auto pos0 = material.find_last_of('/');
				pos0 = pos0 == std::string::npos ? 0 : pos0;
				auto tmp_str = material.substr(pos0);
				auto pos1 = tmp_str.find_first_of('_');
				pos1 = pos1 == std::string::npos ? 0 : pos1;
				// if(pos1 != 0)
				 //material = material.substr(0, pos0 + pos1);
			}
			catch (std::exception& e) {
				std::cout << e.what() << "\n";
			}

			std::vector<unsigned int> indices;


			{
				for (int i = 0; i < face.numedges; i++)
				{
					// if(face.firstedge+i >= edges.size() ) continue;
					if (material.find("TOOL") != std::string::npos)
					{
						//continue;
						isTool = true;
					}

					if (material.find("TRIGGER") != std::string::npos)
					{
						continue;
						isTool = false;
						isTrigger = true;
					}
					//auto str1 = 


					auto edgeIndex = m_pBspFileData.surfEdges.at(face.firstedge + i);
					auto vertexIndex = m_pBspFileData.edges.at(abs(edgeIndex)).v[edgeIndex >= 0 ? 0 : 1];

					if (i > 2)
					{
						triangles.insert(triangles.end(), triangles.begin(), triangles.begin() + 1);
						triangles.insert(triangles.end(), triangles.end() - 2, triangles.end() - 1);
					}

					glm::vec3 vertex = m_pBspFileData.vertices.at(vertexIndex);
					auto& tv = m_pBspFileData.texInfo[face.texinfo].textureVecs;

					float u = (tv[0][0] * vertex.x) + (tv[0][1] * vertex.y) + (tv[0][2] * vertex.z) + tv[0][3];
					float v = (tv[1][0] * vertex.x) + (tv[1][1] * vertex.y) + (tv[1][2] * vertex.z) + tv[1][3];
					uv.emplace_back(u, v);

					// Rearrange vector to match Vulkan coordinates
					vtxs.emplace_back(-vertex.x, vertex.z, vertex.y);
					triangles.push_back(vertexIndex);
				}


				if (triangles.empty())
					continue;

				indices = generateIndices(triangles);


				if (face.dispinfo != -1) {

					isDisp = true;
					std::vector<glm::vec3> newVertices{};
					std::vector<glm::vec2> newUvs{};
					std::vector<unsigned int> newIndices{};

					auto& tv = m_pBspFileData.texInfo[face.texinfo].textureVecs;
					generatePatch(vtxs, indices, newVertices, newUvs, newIndices, m_pBspFileData.displacements[face.dispinfo], tv, face);

					vtxs.assign(newVertices.begin(), newVertices.end());
					uv.assign(newUvs.begin(), newUvs.end());
					indices.assign(newIndices.begin(), newIndices.end());
				}
			}

			if (indices.size() == 0 || vtxs.size() == 0) continue;



			m_pFaces[material].push_back({ .m_pIndex = index,
										  .m_pVisible = true,
										  .m_pNumVertices = (unsigned int)vtxs.size(),
										  .vertices = vtxs,
										  .uvs = uv,
										  .indices = indices,
										  .material = material,
										  .tool = isTool,
										  .trigger = isTrigger,
										  .isDisplacement = isDisp });
			index++;
		}
	}

	template <typename cType>
	void parseLump(const unsigned lumpId, std::vector<cType>& container)
	{
		auto& lump = m_pBspFileData.header.lumps[lumpId];
		container.resize(lump.filelen / sizeof(cType));
		m_pBspFile.seekg(lump.fileofs);
		m_pBspFile.read((char*)container.data(), lump.filelen);
	}

	void generatePatch(const std::vector<glm::vec3>& inVertices, const std::vector<unsigned int>& inIndices, std::vector<glm::vec3>& outVertices, std::vector<glm::vec2>& uvs, std::vector<unsigned int>& indices, ddispinfo_t dispInfo, float tv[2][4], dface_t face) {

		glm::vec3 min{}, max{};

		auto faceBoundingBox = [this](auto& face, auto& min, auto& max) -> void {
			for (int i = 0; i < face.numedges; i++) {

				auto edgeIndex = m_pBspFileData.surfEdges.at(face.firstedge + i);
				auto vertexIndex = m_pBspFileData.edges.at(abs(edgeIndex)).v[edgeIndex >= 0 ? 0 : 1];
				glm::vec3 vertex = m_pBspFileData.vertices.at(vertexIndex);

				if (vertex.x < min.x || vertex.y < min.y || vertex.z < min.z) min = vertex;
				if (vertex.x > max.x || vertex.y > max.y || vertex.z > max.z) max = vertex;
			}
		};

		faceBoundingBox(m_pBspFileData.faces[dispInfo.MapFace], min, max);

		std::vector<glm::vec3> v = {
			glm::vec3(-inVertices[0].x, inVertices[0].z, inVertices[0].y),
			glm::vec3(-inVertices[1].x, inVertices[1].z, inVertices[1].y),
			glm::vec3(-inVertices[2].x, inVertices[2].z, inVertices[2].y),
			glm::vec3(-inVertices[3].x, inVertices[3].z, inVertices[3].y)
		};

		//check which corner has the minimum value, to use it as starting point
		float startDist = 10000000000000;
		int startIndex = -1;
		for (int j = 0; j < 4; j++) {
			const float dist = glm::distance(v[j], dispInfo.startPosition);
			if (dist < startDist) {
				startIndex = j;
				startDist = dist;
			}
		}

		//rotate the vector to put the lowest point first
		if (startIndex != 0)
		{
			std::rotate(v.begin(), v.begin() + startIndex, v.end());
		}


		int power = dispInfo.power;
		int numVerts = glm::pow((glm::pow(2, power) + 1), 2);
		int length = (1 << power) + 1;

		outVertices.resize(numVerts);

		auto lerp = [](glm::vec3 a, glm::vec3 b, float t) -> glm::vec3 {
			return a + t * (b - a);
		};

		glm::vec3 v0{};
		glm::vec3 v1{};

		for (int i = 0; i < length; i++) {

			const float ty = (float)i / (float)(length - 1);
			v0 = lerp(v[0], v[1], ty);
			v1 = lerp(v[3], v[2], ty);

			for (int j = 0; j < length; j++) {

				int index = dispInfo.DispVertStart + (i * length) + j;

				auto& dispVertex = m_pBspFileData.vertDisps[index];

				const float tx = (float)j / (float)(length - 1.0f);

				glm::vec3 vtx = lerp(v0, v1, tx);

				float u = (tv[0][0] * vtx.x) + (tv[0][1] * vtx.y) + (tv[0][2] * vtx.z) + tv[0][3];
				float v = (tv[1][0] * vtx.x) + (tv[1][1] * vtx.y) + (tv[1][2] * vtx.z) + tv[1][3];

				uvs.push_back(glm::vec2(u, v));

				//displace
				vtx += dispVertex.vec * dispVertex.dist;

				outVertices[i * length + j] = glm::vec3(-vtx.x, vtx.z, vtx.y);
			}
		}

		//Triangulate
		int edge = 0;
		for (int i = 0; i < numVerts - length; i++) {
			if (edge == length - 1) {
				edge = 0;
				continue;
			}

			indices.push_back(i + 1);
			indices.push_back(i);
			indices.push_back(i + length);

			indices.push_back(i + 1);
			indices.push_back(i + length);
			indices.push_back(i + length + 1);

			edge++;
		}
	}


};