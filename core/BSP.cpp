///========= Copyright Imanol Fotia, All rights reserved. ============//
///
/// Purpose: Load and render Quake 3 BSP map file
///
///=============================================================================

#include <Core.hpp>
#include <omp.h>
#include <BSP.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <Frustum.h>
#include <ResourceManager.h>
#include <Texture.h>

namespace Epsilon
{

    CQuake3BSP::CQuake3BSP()
    {
        m_numOfVerts = 0;
        m_numOfFaces = 0;
        m_numOfIndices = 0;
        m_numOfTextures = 0;

        m_pVerts = NULL;
        m_pFaces = NULL;
        m_pIndices = NULL;
    }

    bool CQuake3BSP::LoadBSP(const char *strFileName)
    {
        try
        {
            FILE *fp = NULL;
            int i = 0;

            tBSPHeader header = {0};
            tBSPLump lumps[kMaxLumps] = {0};

            fp = fopen(strFileName, "rb");

            fread(&header, 1, sizeof(tBSPHeader), fp);
            fread(&lumps, kMaxLumps, sizeof(tBSPLump), fp);

            m_numOfVerts = lumps[kVertices].length / sizeof(tBSPVertex);
            m_pVerts = new tBSPVertex[m_numOfVerts];

            m_numOfFaces = lumps[kFaces].length / sizeof(tBSPFace);
            m_pFaces = new tBSPFace[m_numOfFaces];

            m_numOfIndices = lumps[kIndices].length / sizeof(int);
            m_pIndices = new int[m_numOfIndices];

            m_numOfTextures = lumps[kTextures].length / sizeof(tBSPTexture);
            tBSPTexture *pTextures = new tBSPTexture[m_numOfTextures];

            m_numOfLightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
            tBSPLightmap *pLightmaps = new tBSPLightmap[m_numOfLightmaps];

            fseek(fp, lumps[kVertices].offset, SEEK_SET);

            for (i = 0; i < m_numOfVerts; i++)
            {
                fread(&m_pVerts[i], 1, sizeof(tBSPVertex), fp);

                float temp = m_pVerts[i].vPosition.y;
                m_pVerts[i].vPosition.y = m_pVerts[i].vPosition.z;
                m_pVerts[i].vPosition.z = -temp;

                temp = m_pVerts[i].vNormal.y;
                m_pVerts[i].vNormal.y = m_pVerts[i].vNormal.z;
                m_pVerts[i].vNormal.z = -temp;
            }

            fseek(fp, lumps[kLightmaps].offset, SEEK_SET);

            for (i = 0; i < m_numOfLightmaps; i++)
            {
                fread(&pLightmaps[i], 1, sizeof(tBSPLightmap), fp);
            }

            fseek(fp, lumps[kIndices].offset, SEEK_SET);

            fread(m_pIndices, m_numOfIndices, sizeof(int), fp);

            fseek(fp, lumps[kFaces].offset, SEEK_SET);

            fread(m_pFaces, m_numOfFaces, sizeof(tBSPFace), fp);

            fseek(fp, lumps[kTextures].offset, SEEK_SET);

            fread(pTextures, m_numOfTextures, sizeof(tBSPTexture), fp);

            m_numOfNodes = lumps[kNodes].length / sizeof(tBSPNode);
            m_pNodes = new tBSPNode[m_numOfNodes];

            fseek(fp, lumps[kNodes].offset, SEEK_SET);
            fread(m_pNodes, m_numOfNodes, sizeof(tBSPNode), fp);

            m_numOfLeafs = lumps[kLeafs].length / sizeof(tBSPLeaf);
            m_pLeafs = new tBSPLeaf[m_numOfLeafs];

            fseek(fp, lumps[kLeafs].offset, SEEK_SET);
            fread(m_pLeafs, m_numOfLeafs, sizeof(tBSPLeaf), fp);

            for (i = 0; i < m_numOfLeafs; i++)
            {
                int temp = m_pLeafs[i].min.y;
                m_pLeafs[i].min.y = m_pLeafs[i].min.z;
                m_pLeafs[i].min.z = -temp;

                temp = m_pLeafs[i].max.y;
                m_pLeafs[i].max.y = m_pLeafs[i].max.z;
                m_pLeafs[i].max.z = -temp;
            }

            m_numOfLeafFaces = lumps[kLeafFaces].length / sizeof(int);
            m_pLeafFaces = new int[m_numOfLeafFaces];

            fseek(fp, lumps[kLeafFaces].offset, SEEK_SET);
            fread(m_pLeafFaces, m_numOfLeafFaces, sizeof(int), fp);

            m_numOfPlanes = lumps[kPlanes].length / sizeof(tBSPPlane);
            m_pPlanes = new tBSPPlane[m_numOfPlanes];

            fseek(fp, lumps[kPlanes].offset, SEEK_SET);
            fread(m_pPlanes, m_numOfPlanes, sizeof(tBSPPlane), fp);

            for (i = 0; i < m_numOfPlanes; i++)
            {
                float temp = m_pPlanes[i].vNormal.y;
                m_pPlanes[i].vNormal.y = m_pPlanes[i].vNormal.z;
                m_pPlanes[i].vNormal.z = -temp;
            }

            fseek(fp, lumps[kVisData].offset, SEEK_SET);

            if (lumps[kVisData].length)
            {
                fread(&(m_clusters.numOfClusters), 1, sizeof(int), fp);
                fread(&(m_clusters.bytesPerCluster), 1, sizeof(int), fp);

                int size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
                m_clusters.pBitsets = new char[size];

                fread(m_clusters.pBitsets, 1, sizeof(char) * size, fp);
            }
            else
                m_clusters.pBitsets = NULL;

            fseek(fp, lumps[kEntities].offset, SEEK_SET);

            std::vector<char> ents(lumps[kEntities].length);

            fread(&ents[0], lumps[kEntities].length, sizeof(char), fp);

            fclose(fp);

            m_FacesDrawn.Resize(m_numOfFaces);

            /// Used to Smooth the normals of the whole BSP map
            vector<glm::vec3> Normals;
            for (int i = 0; i < m_numOfVerts; i++)
            {
                Normals.push_back(m_pVerts[i].vNormal);
            }

            for (int i = 0; i < m_numOfVerts; i++)
            {
                glm::vec3 N = glm::vec3(0, 0, 0);
                std::vector<int> normalIndex;
                bool first = true;
                for (int j = 0; j < m_numOfVerts; ++j)
                {
                    if (m_pVerts[i].vPosition == m_pVerts[j].vPosition)
                    {
                        if (first)
                        {
                            normalIndex.push_back(i);
                            first = false;
                        }
                        else
                        {
                            bool valid = true;
                            for (unsigned int k = 0; k < normalIndex.size(); ++k)
                            {
                                if (glm::dot(Normals.at(j), Normals.at(normalIndex.at(k))) <= 0.0)
                                {
                                    valid = false;
                                    break;
                                }
                                else
                                {
                                }
                            }
                            if (valid)
                                normalIndex.push_back(j);
                        }
                    }
                }

                for (unsigned int k = 0; k < normalIndex.size(); k++)
                    N = glm::normalize(N + Normals.at(normalIndex.at(k)));

                m_pVerts[i].vNormal = N;
            }

            for (i = m_numOfFaces - 1; i >= 0; i--)
            {
                tBSPFace *pFace = &m_pFaces[i];
                if (pFace->type != FACE_POLYGON && pFace->type != FACE_MESHVERT)
                    continue;
                std::vector<glm::vec3> faceVertices;
                std::vector<glm::vec3> faceNormals;
                std::vector<glm::vec2> faceTexCoords;
                std::vector<glm::vec2> faceLMTexCoords;
                std::vector<unsigned int> faceIndices;

                for (int j = 0; j < pFace->numOfVerts; j++)
                {
                    faceVertices.push_back(m_pVerts[pFace->startVertIndex + j].vPosition);
                }

                for (int j = 0; j < pFace->numOfVerts; j++)
                {
                    faceTexCoords.push_back(m_pVerts[pFace->startVertIndex + j].vTextureCoord);
                }

                for (int j = 0; j < pFace->numOfVerts; j++)
                {
                    faceLMTexCoords.push_back(m_pVerts[pFace->startVertIndex + j].vLightmapCoord);
                }

                for (int j = 0; j < pFace->numOfVerts; j++)
                {
                    faceNormals.push_back(m_pVerts[pFace->startVertIndex + j].vNormal);
                }

                for (int k = 0; k < pFace->numOfIndices; k++)
                {
                    faceIndices.push_back(m_pIndices[pFace->startIndex + k]);
                }

                unsigned int texture = -1;

                string str(pTextures[pFace->textureID].strName);
                std::size_t found = str.find("skies");
                if (found != std::string::npos)
                {
                    pFace->type = FACE_UNUSED;
                    continue;
                }

                for (unsigned int k = 0; k < Textures.size(); k++)
                {
                    if (Textures.at(k).path == pTextures[pFace->textureID].strName)
                    {
                        texture = k;
                        break;
                    }
                }

                BSPTexture tex;
                BSPTexture normal;
                BSPTexture specular;
                BSPTexture metallic;

                tex.path = std::string(pTextures[pFace->textureID].strName) + ".png";
                ResourceManager::Get().addTextureToQueue(tex.path);
                tex.type = 1;
                Textures.push_back(tex);

                normal.path = string(pTextures[pFace->textureID].strName) + "_n" + ".png";
                ResourceManager::Get().addTextureToQueue(normal.path);
                normal.type = 1;
                normalTextures.push_back(normal);

                specular.path = string(pTextures[pFace->textureID].strName) + "_s" + ".png";
                ResourceManager::Get().addTextureToQueue(specular.path);
                specular.type = 1;
                specularTextures.push_back(specular);

                metallic.path = string(pTextures[pFace->textureID].strName) + "_m" + ".png";
                ResourceManager::Get().addTextureToQueue(metallic.path);
                metallic.type = 1;
                metallicTextures.push_back(metallic);

                faceTexture[i] = tex;
                faceTexture_normal[i] = normal;
                faceTexture_specular[i] = specular;
                faceTexture_metallic[i] = metallic;

                BSPFace Face;

                Face.BuildFace(faceVertices, faceNormals, faceTexCoords, faceLMTexCoords, faceIndices, i, pTextures[pFace->textureID].strName, pLightmaps[pFace->lightmapID]);

                this->Faces[i] = Face;
            }
        }
        catch (...)
        {
            throw;
        }

        return true;
    }

    int CQuake3BSP::FindLeaf(glm::vec3 vPos)
    {
        int i = 0;
        float distance = 0.0f;

        while (i >= 0)
        {
            const tBSPNode &node = m_pNodes[i];
            const tBSPPlane &plane = m_pPlanes[node.plane];

            distance = plane.vNormal.x * vPos.x +
                       plane.vNormal.y * vPos.y +
                       plane.vNormal.z * vPos.z - plane.d * 0.1f;

            if (distance >= 0)
            {
                i = node.front;
            }
            else
            {
                i = node.back;
            }
        }
        return ~i; /// Binary operation
    }

    inline int CQuake3BSP::IsClusterVisible(int current, int test)
    {
        if (!m_clusters.pBitsets || current < 0)
            return 1;

        char visSet = m_clusters.pBitsets[(current * m_clusters.bytesPerCluster) + (test / 8)];
        int result = visSet & (1 << ((test)&7));

        return (result);
    }

    void CQuake3BSP::RenderFace(int faceIndex, GLuint shader, bool simpleRender)
    {
        this->Faces[faceIndex].RenderFace(shader,
                                          ResourceManager::Get().useTexture(faceTexture[faceIndex].path),
                                          ResourceManager::Get().useTexture(faceTexture_normal[faceIndex].path),
                                          ResourceManager::Get().useTexture(faceTexture_specular[faceIndex].path),
                                          ResourceManager::Get().useTexture(faceTexture_metallic[faceIndex].path),
                                          simpleRender);
    }

    int g_VisibleFaces;

    void CQuake3BSP::RenderLevel(glm::vec3 vPos, GLuint shader, bool Shadow)
    {

        m_FacesDrawn.ClearAll();

        int leafIndex = FindLeaf(vPos);
        int cluster = m_pLeafs[leafIndex].cluster;
        g_VisibleFaces = 0;

        for (int i = m_numOfLeafs; i--;)
        {
            tBSPLeaf *pLeaf = &(m_pLeafs[i]);

            if (!IsClusterVisible(cluster, pLeaf->cluster))
                continue;

            if (Shadow)
            {
                if (!Frustum.BoxInFrustum((float)pLeaf->min.x, (float)pLeaf->min.y, (float)pLeaf->min.z,
                                          (float)pLeaf->max.x, (float)pLeaf->max.y, (float)pLeaf->max.z))
                    continue;
            }

            faceCount = pLeaf->numOfLeafFaces;

            while (faceCount--)
            {
                int faceIndex = m_pLeafFaces[pLeaf->leafface + faceCount];
                if (m_pFaces[faceIndex].type != FACE_POLYGON && m_pFaces[faceIndex].type != FACE_MESHVERT)
                    continue;

                if (!m_FacesDrawn.On(faceIndex))
                {
                    g_VisibleFaces++;
                    m_FacesDrawn.Set(faceIndex);
                    RenderFace(faceIndex, shader, !Shadow);
                }
            }
        }
    }

    void CQuake3BSP::Destroy()
    {
        if (m_pVerts)
        {
            delete[] m_pVerts;
            m_pVerts = NULL;
        }
        if (m_pFaces)
        {
            delete[] m_pFaces;
            m_pFaces = NULL;
        }
        if (m_pLeafs)
        {
            delete[] m_pLeafs;
            m_pLeafs = NULL;
        }
        if (m_pIndices)
        {
            delete[] m_pIndices;
            m_pIndices = NULL;
        }
        if (m_pNodes)
        {
            delete[] m_pNodes;
            m_pNodes = NULL;
        }
        if (m_pPlanes)
        {
            delete[] m_pPlanes;
            m_pPlanes = NULL;
        }
        if (m_pLeafFaces)
        {
            delete[] m_pLeafFaces;
            m_pLeafFaces = NULL;
        }
        glDeleteTextures(m_numOfTextures, m_textures);
    }
} // namespace Epsilon