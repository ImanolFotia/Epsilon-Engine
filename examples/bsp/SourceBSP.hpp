#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <glm/glm.hpp>

#include "bsp_definitions.hpp"

class SourceBSP {
    struct Face {
        unsigned int m_pIndex;
        bool m_pVisible;
        unsigned int m_pNumVertices;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<unsigned int> indices;
        std::string material;
        bool tool = false;
        bool trigger = false;
    };
    std::vector<unsigned short> m_pIndices;
    std::vector<Face> m_pFaces;

    struct BSPFileData {
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
        std::vector<dleaf_t> leafs;

        char stringData[MAX_MAP_TEXDATA_STRING_DATA];
    };

    BSPFileData m_pBspFileData;
    std::ifstream m_pBspFile;

public:
    SourceBSP(const char *filename) {
        m_pBspFile = std::ifstream(filename, std::ios::binary);

        m_pBspFile.read((char *) &m_pBspFileData.header, sizeof(dheader_t));

        if (m_pBspFileData.header.ident != IDBSPHEADER) {
            std::cerr << "Not a valid Source BSP file" << std::endl;
            exit(9);
        }

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
        parseLump(LUMP_LEAFS, m_pBspFileData.leafs);

        auto &stringLump = m_pBspFileData.header.lumps[LUMP_STRINGDATA];
        m_pBspFile.seekg(stringLump.fileofs);
        m_pBspFile.read((char *) m_pBspFileData.stringData, stringLump.filelen);

        for (auto &edge: m_pBspFileData.edges) {
            m_pIndices.push_back(edge.v[0]);
            m_pIndices.push_back(edge.v[1]);
        }

        parseGeometry();

        std::unordered_map<std::string, int> materialTable;

        for (auto &face: m_pFaces) {
            materialTable[face.material]++;
        }

        std::cout << "there are " << materialTable.size() << " materials\n";

        m_pBspFile.close();
    }

    const std::vector<glm::vec3> &Vertices() const {
        return m_pBspFileData.vertices;
    }

    const std::vector<unsigned short> &Indices() {
        return m_pIndices;
    }

    const std::vector<Face> &Faces() {
        return m_pFaces;
    }

    std::vector<unsigned int> generateIndices(std::vector<unsigned int> inIndices) {
        std::map<unsigned short, unsigned short> indices;
        std::vector<unsigned int> listIndices;
        std::vector<unsigned int> outIndices;

        for (auto &index: inIndices) {
            if (!indices.contains(index)) {
                indices[index] = indices.size();
            }

            listIndices.push_back(indices[index]);
            outIndices.push_back(indices[index]);
        }

        return outIndices;
    }

    void parseGeometry() {

        unsigned int index = 0;
        std::string material;
        for (auto &face: m_pBspFileData.faces) {
            //if(face.numPrims > 0) continue;
            bool isTool = false;
            bool isTrigger = false;
            std::vector<glm::vec3> vtxs;
            std::vector<glm::vec2> uv;
            std::vector<unsigned int> triangles;

            for (int i = 0; i < face.numedges; i++) {
                if (face.dispinfo != -1) continue;
                //if(face.firstedge+i >= edges.size() ) continue;
                material = std::string(
                        &m_pBspFileData.stringData[m_pBspFileData.stringTable[m_pBspFileData.texData[m_pBspFileData.texInfo[face.texinfo].texdata].nameStringTableID]]);

                if (material.find("TOOLS") != std::string::npos) {
                    continue;
                    isTool = true;
                }

                if (material.find("TRIGGER") != std::string::npos) {
                    continue;
                    isTool = false;
                    isTrigger = true;
                }

                auto edgeIndex = m_pBspFileData.surfEdges.at(face.firstedge + i);
                auto vertexIndex= m_pBspFileData.edges.at(abs(edgeIndex)).v[edgeIndex >= 0 ? 0 : 1];

                if (i > 2) {
                    triangles.insert(triangles.end(), triangles.begin(), triangles.begin() + 1);
                    triangles.insert(triangles.end(), triangles.end() - 2, triangles.end() - 1);
                }

                glm::vec3 vertex = m_pBspFileData.vertices.at(vertexIndex);
                auto& tv = m_pBspFileData.texInfo[face.texinfo].textureVecs;

                float u = (tv[0][0] * vertex.x) + (tv[0][1] * vertex.y) + (tv[0][2] * vertex.z) + tv[0][3];
                float v = (tv[1][0] * vertex.x) + (tv[1][1]* vertex.y) + (tv[1][2]* vertex.z) + tv[1][3];

                uv.emplace_back(u, v);

                //Rearrange vector to match Vulkan coordinates
                vtxs.emplace_back(-vertex.x, vertex.z, vertex.y);
                triangles.push_back(vertexIndex);

            }

            if (triangles.empty()) continue;

            std::vector<unsigned int> indices = generateIndices(triangles);

            m_pFaces.push_back({
                                       .m_pIndex = index,
                                       .m_pVisible = true,
                                       .m_pNumVertices = (unsigned int) vtxs.size(),
                                       .vertices = vtxs,
                                       .uvs = uv,
                                       .indices = indices,
                                       .material = material,
                                       .tool = isTool,
                                       .trigger = isTrigger
                               });
            index++;
        }

    }

    template<typename cType>
    void parseLump(const unsigned lumpId, std::vector<cType> &container) {
        auto &lump = m_pBspFileData.header.lumps[lumpId];
        container.resize(lump.filelen / sizeof(cType));
        m_pBspFile.seekg(lump.fileofs);
        m_pBspFile.read((char *) container.data(), lump.filelen);
    }


    std::vector<glm::vec3> createDisplacement(std::vector<glm::vec3> vecs, int tess, int size, glm::vec3 position) {

        std::vector<std::vector<float>> Grid;
        std::vector<glm::vec3> vertices;
        std::vector<float> row;
        int counter = 0;

        for (int i = 0; i < tess; i++) {
            for (int j = 0; j < tess; j++) {
                glm::vec3 vert;
                vert.x = ((float) i + position.x) * size;
                vert.y = (float) vecs[j + i * size].y;
                vert.z = ((float) j + position.z) * size;
                row.push_back(vert.y);
                float fScaleC = float(j) / float(size - 1);
                float fScaleR = float(i) / float(size - 1);
                vertices.push_back(vert);
                counter++;
            }
            Grid.push_back(row);
            row.clear();
        }

    }
};