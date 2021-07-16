///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <GL/glad.h>

#include "include/BSP.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <memory>
#include "include/Frustum.h"
#include <stb_image.h>
#include <include/helpers.h>
#include <include/texture.hpp>
#include <exception>
using namespace std;

CQuake3BSP::CQuake3BSP()
{
    m_numOfVerts    = 0;
    m_numOfFaces    = 0;
    m_numOfIndices  = 0;
    m_numOfTextures = 0;

    m_pVerts = NULL;
    m_pFaces = NULL;
    m_pIndices = NULL;
}


bool CQuake3BSP::LoadBSP(const char *strFileName)
{
    try
    {
        std::cout << strFileName << std::endl;

        FILE *fp = NULL;
        int i = 0;

        tBSPHeader header = {0};
        tBSPLump lumps[kMaxLumps] = {0};

        fp = fopen(strFileName, "rb");

        if(fp == NULL)
        {
            std::cout << "File not opened" << std::endl;
            return 0;
        }
        fread(&header, 1, sizeof(tBSPHeader), fp);
        fread(&lumps, kMaxLumps, sizeof(tBSPLump), fp);

        m_numOfVerts = lumps[kVertices].length / sizeof(tBSPVertex);
        m_pVerts     = new tBSPVertex [m_numOfVerts];

        m_numOfFaces = lumps[kFaces].length / sizeof(tBSPFace);
        m_pFaces     = new tBSPFace [m_numOfFaces];

        m_numOfIndices = lumps[kIndices].length / sizeof(int);
        m_pIndices     = new int [m_numOfIndices];

        m_numOfTextures = lumps[kTextures].length / sizeof(tBSPTexture);
        tBSPTexture *pTextures = new tBSPTexture [m_numOfTextures];

        m_numOfLightmaps = lumps[kLightmaps].length / sizeof(tBSPLightmap);
        tBSPLightmap *pLightmaps = new tBSPLightmap [m_numOfLightmaps ];

        ///cout << "Number of lightmaps : " << lumps[kLightmaps].length / sizeof(tBSPLightmap) << endl;

        fseek(fp, lumps[kVertices].offset, SEEK_SET);

        for(i = 0; i < m_numOfVerts; i++)
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

        for(i = 0; i < m_numOfLightmaps ; i++)
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
        m_pNodes     = new tBSPNode [m_numOfNodes];

        fseek(fp, lumps[kNodes].offset, SEEK_SET);
        fread(m_pNodes, m_numOfNodes, sizeof(tBSPNode), fp);

        m_numOfLeafs = lumps[kLeafs].length / sizeof(tBSPLeaf);
        m_pLeafs     = new tBSPLeaf [m_numOfLeafs];

        fseek(fp, lumps[kLeafs].offset, SEEK_SET);
        fread(m_pLeafs, m_numOfLeafs, sizeof(tBSPLeaf), fp);

        for(i = 0; i < m_numOfLeafs; i++)
        {
            int temp = m_pLeafs[i].min.y;
            m_pLeafs[i].min.y = m_pLeafs[i].min.z;
            m_pLeafs[i].min.z = -temp;

            temp = m_pLeafs[i].max.y;
            m_pLeafs[i].max.y = m_pLeafs[i].max.z;
            m_pLeafs[i].max.z = -temp;
        }

        m_numOfLeafFaces = lumps[kLeafFaces].length / sizeof(int);
        m_pLeafFaces     = new int [m_numOfLeafFaces];

        fseek(fp, lumps[kLeafFaces].offset, SEEK_SET);
        fread(m_pLeafFaces, m_numOfLeafFaces, sizeof(int), fp);

        m_numOfPlanes = lumps[kPlanes].length / sizeof(tBSPPlane);
        m_pPlanes     = new tBSPPlane [m_numOfPlanes];

        fseek(fp, lumps[kPlanes].offset, SEEK_SET);
        fread(m_pPlanes, m_numOfPlanes, sizeof(tBSPPlane), fp);

        for(i = 0; i < m_numOfPlanes; i++)
        {
            float temp = m_pPlanes[i].vNormal.y;
            m_pPlanes[i].vNormal.y = m_pPlanes[i].vNormal.z;
            m_pPlanes[i].vNormal.z = -temp;
        }

        fseek(fp, lumps[kVisData].offset, SEEK_SET);

        if(lumps[kVisData].length)
        {
            fread(&(m_clusters.numOfClusters),	 1, sizeof(int), fp);
            fread(&(m_clusters.bytesPerCluster), 1, sizeof(int), fp);

            int size = m_clusters.numOfClusters * m_clusters.bytesPerCluster;
            m_clusters.pBitsets = new char [size];

            fread(m_clusters.pBitsets, 1, sizeof(char) * size, fp);
        }
        else
            m_clusters.pBitsets = NULL;

        fseek(fp, lumps[kEntities].offset, SEEK_SET);

        //char ents[ lumps[kEntities].length ];

        //fread(ents, lumps[kEntities].length, sizeof(char), fp);

        //cout << ents << endl;

        fclose(fp);

        m_FacesDrawn.Resize(m_numOfFaces);

        /// Used to Smooth the normals of the whole BSP map
        vector<glm::vec3> Normals;
        for(int i = 0 ; i < m_numOfVerts ; i++)
        {
            Normals.push_back(m_pVerts[i].vNormal);
        }

        for (int i = 0 ; i < m_numOfVerts ; i++)
        {
            glm::vec3 N = glm::vec3(0,0,0);
            std::vector<int> normalIndex;
            bool first = true;
            for (int j = 0; j < m_numOfVerts; ++j)
            {
                if (m_pVerts[i].vPosition == m_pVerts[j].vPosition)
                {
                    if(first)
                    {
                        //N = N + m_pVerts[j].vNormal;
                        normalIndex.push_back(i);
                        first = false;
                    }
                    else
                    {
                        bool valid = true;
                        for(int k = 0 ; k < normalIndex.size() ; ++k)
                        {
                            if( glm::dot(Normals.at(j), Normals.at(normalIndex.at(k)) ) <= 0.0)
                            {
                                valid = false;
                                //cout << glm::dot(Normals.at(j), Normals.at(normalIndex.at(k))) << endl;
                                break;

                            }
                            else
                            {
                            }
                        }
                        if(valid)
                            normalIndex.push_back(j);

                    }
                }
            }

            for(int k = 0 ; k < normalIndex.size() ; k++)
                N = glm::normalize(N +  Normals.at(normalIndex.at(k)));

            m_pVerts[i].vNormal = N;
        }


        for(i = m_numOfFaces ; i >= 0 ; i--)
        {
            tBSPFace *pFace = &m_pFaces[i];
            if(pFace->type != FACE_POLYGON && pFace->type != FACE_MESHVERT) continue;
            std::vector<glm::vec3> faceVertices;
            std::vector<glm::vec3> faceNormals;
            std::vector<glm::vec2> faceTexCoords;
            std::vector<glm::vec2> faceLMTexCoords;
            std::vector<unsigned int> faceIndices;

            for(int j = 0 ; j < pFace->numOfVerts ; j++)
            {
                faceVertices.push_back(m_pVerts[pFace->startVertIndex + j].vPosition);

            }

            for(int j = 0 ; j < pFace->numOfVerts; j++)
            {
                faceTexCoords.push_back(m_pVerts[pFace->startVertIndex + j].vTextureCoord);
            }

            for(int j = 0 ; j < pFace->numOfVerts; j++)
            {
                faceLMTexCoords.push_back(m_pVerts[pFace->startVertIndex + j].vLightmapCoord);
            }

            for(int j = 0 ; j < pFace->numOfVerts; j++)
            {
                faceNormals.push_back(m_pVerts[pFace->startVertIndex + j].vNormal);
            }

            for(int k = 0 ; k < pFace->numOfIndices ; k++)
            {
                faceIndices.push_back (m_pIndices[pFace->startIndex + k]);
            }

            int texture = -1;

            string str( pTextures[pFace->textureID].strName);
            std::size_t found=str.find("skies");
            if (found!=std::string::npos)
            {
                pFace->type = FACE_UNUSED;
                continue;
            }

            for(int k = 0 ; k < Textures.size() ; k++)
            {
                if(Textures.at(k)->getRelativePath() == pTextures[pFace->textureID].strName)
                {
                    texture = k;
                    break;
                }
            }
            OpenGL::Texture* tex;
            OpenGL::Texture* normal;
            OpenGL::Texture* specular;
            OpenGL::Texture* metallic;

            if(texture != -1)
            {

                //std::cout << "Found duplicated texture" << std::endl;
                OpenGL::Texture* tex;
                OpenGL::Texture* normal;
                OpenGL::Texture* specular;
                OpenGL::Texture* metallic;
                tex = std::move(Textures.at(texture));
                Textures.push_back(tex);

                normal =std::move( normalTextures.at(texture));
                normalTextures.push_back(normal);

                specular = std::move(specularTextures.at(texture));
                specularTextures.push_back(specular);

                metallic = std::move(metallicTextures.at(texture));
                metallicTextures.push_back(metallic);
            }
            else
            {
               // std::cout << "Found new texture" << std::endl;

                tex = new OpenGL::Texture();
                tex->Load(std::string(pTextures[pFace->textureID].strName), "C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/", ".png");
                Textures.push_back(tex);

                normal = new OpenGL::Texture();
                normal->Load(string(pTextures[pFace->textureID].strName), "C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/", ".png", "_n");
                normalTextures.push_back(normal);

                specular = new OpenGL::Texture();
                specular->Load(string(pTextures[pFace->textureID].strName), "C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/", ".png", "_s");
                specularTextures.push_back(specular);

                metallic = new OpenGL::Texture();
                metallic->Load(string(pTextures[pFace->textureID].strName), "C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/", ".png", "_m");
                metallicTextures.push_back(metallic);
            }

            faceTexture[i] = tex;
            faceTexture_normal[i] = normal;
            faceTexture_specular[i] = specular;
            faceTexture_metallic[i] = metallic;

            BSPFace Face;

            Face.BuildFace(faceVertices, faceNormals, faceTexCoords, faceLMTexCoords, faceIndices, i, pTextures[pFace->textureID].strName, pLightmaps[pFace->lightmapID]);

            this->Faces[i] = Face;

        }
    }
            catch(...)
            {
                throw;
            }
        mIsLoaded = true;
        return true;
    }


    int CQuake3BSP::FindLeaf(glm::vec3 vPos)
    {
        int i = 0;
        float distance = 0.0f;

        while(i >= 0)
        {
            const tBSPNode&  node = m_pNodes[i];
            const tBSPPlane& plane = m_pPlanes[node.plane];

            distance =	plane.vNormal.x * vPos.x +
                        plane.vNormal.y * vPos.y +
                        plane.vNormal.z * vPos.z - plane.d/10;

            if(distance >= 0)
            {
                i = node.front;
            }
            else
            {
                i = node.back;
            }
        }
        return ~i;  /// Binary operation
    }


    inline int CQuake3BSP::IsClusterVisible(int current, int test)
    {
        if(!m_clusters.pBitsets || current < 0) return 1;

        char visSet = m_clusters.pBitsets[(current*m_clusters.bytesPerCluster) + (test / 8)];

        int result = visSet & (1 << ((test) & 7));

        return ( result );
    }


    void CQuake3BSP::RenderFace(int faceIndex, GLuint shader, GLuint texID)
    {/*
        if(m_pFaces[faceIndex].type == 3)
            glUniform1i(glGetUniformLocation(shader, "lightmap"), 0);
        else
            glUniform1i(glGetUniformLocation(shader, "lightmap"), 1);*/
        this->Faces[faceIndex].RenderFace(shader, faceTexture[faceIndex], faceTexture_normal[faceIndex], faceTexture_specular[faceIndex], faceTexture_metallic[faceIndex]);


    }

    //int g_VisibleFaces;

    void CQuake3BSP::RenderLevel(glm::vec3 vPos, GLuint shader, bool Shadow)
    {

        m_FacesDrawn.ClearAll();

        int leafIndex = FindLeaf(vPos);

        int cluster = m_pLeafs[leafIndex].cluster;

        int i = m_numOfLeafs;
        //static int g_VisibleFaces = 0;

        while(i--)
        {

            tBSPLeaf *pLeaf = &(m_pLeafs[i]);

            if(!IsClusterVisible(cluster, pLeaf->cluster))
                continue;

                if(!Frustum.BoxInFrustum((float)pLeaf->min.x, (float)pLeaf->min.y, (float)pLeaf->min.z,
                                         (float)pLeaf->max.x, (float)pLeaf->max.y, (float)pLeaf->max.z))
                    continue;

            faceCount = pLeaf->numOfLeafFaces;

            while(faceCount--)
            {
                int faceIndex = m_pLeafFaces[pLeaf->leafface + faceCount];
                if(m_pFaces[faceIndex].type != FACE_POLYGON && m_pFaces[faceIndex].type != FACE_MESHVERT) continue;

                if(!m_FacesDrawn.On(faceIndex))
                {
                    //std::cout << g_VisibleFaces++ << std::endl;
                    m_FacesDrawn.Set(faceIndex);
                    RenderFace(faceIndex, shader, faceTexture[faceIndex]->getTextureId());
                }
            }

        }

        ///glfwSetWindowTitle(window, ("Faces Drawn: " + to_string(g_VisibleFaces)).c_str());
    }


    void CQuake3BSP::Destroy()
    {
        try{
            if(m_pVerts)
            {
                delete [] m_pVerts;
                m_pVerts = nullptr;
            }

            if(m_pFaces)
            {
                delete [] m_pFaces;
                m_pFaces = nullptr;
            }

            if(m_pLeafs)
            {
                delete [] m_pLeafs;
                m_pLeafs = nullptr;
            }

            if(m_pIndices)
            {
                delete [] m_pIndices;
                m_pIndices = nullptr;
            }

            if(m_pNodes)
            {
                delete [] m_pNodes;
                m_pNodes = nullptr;
            }

            if(m_pPlanes)
            {
                delete [] m_pPlanes;
                m_pPlanes = nullptr;
            }

            if(m_pLeafFaces)
            {
                delete [] m_pLeafFaces;
                m_pLeafFaces = nullptr;
            }

            for(auto & a : this->Faces){
                a.second.Destroy();
            }

            for(auto & a : this->Textures){
                a->Destroy();
                delete a;
                a = nullptr;
            }


            for(auto & a : this->faceTexture){
                a.second->Destroy();
                delete a.second;
                a.second = nullptr;
            }


            for(auto & a : this->faceTexture_normal){
                a.second->Destroy();
                delete a.second;
                a.second = nullptr;
            }

            for(auto & a : this->faceTexture_specular){
                a.second->Destroy();
                delete a.second;
                a.second = nullptr;
            }

            for(auto & a : this->faceTexture_metallic){
                a.second->Destroy();
                delete a.second;
                a.second = nullptr;
            }


            for(auto & a : this->normalTextures){
                a->Destroy();
                delete a;
                a = nullptr;
            }

            for(auto & a : this->specularTextures){
                a->Destroy();
                delete a;
                a = nullptr;
            }

            for(auto & a : this->metallicTextures){
                a->Destroy();
                delete a;
                a = nullptr;
            }

            glDeleteTextures(m_numOfTextures, m_textures);
        }
        catch(std::exception e)
        {
            std::cout << "Exception received from: " << __FILE__ << " Function: " << __FUNCTION__ << "\n" << "What(): " << e.what() << std::endl;
        }
    }
