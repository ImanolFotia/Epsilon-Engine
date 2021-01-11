#include "Model.h"
#include <ResourceManager.h>
#include <Log.h>
#include <Engine.hpp>

namespace Epsilon
{
    Model::Model(const char *path, glm::vec3 pos, glm::vec3 sc, glm::quat rot)
    {
        this->path = path;
        Position = pos;
        Scale = sc;
        Rotation = rot;
        PrevModel = glm::mat4(1.0f);
        ModelMatrix = glm::mat4(1.0f);
        uniformsSet = false;

        this->loadModel(path, 0);
    }

    bool Model::loadModel(std::string emlPath, int a)
    {
        using namespace Renderer;
        std::ifstream inFILE(emlPath, std::ios::binary);
        if (!inFILE.is_open())
        {
            std::cout << "Fail to open EML file" << std::endl;
            Log::WriteToLog("Model: " + emlPath + " could not be loaded.");
            return false;
        }

        int numvertices = 0;
        int numMeshes = 0;
        int numIndices = 0;

        t_eml_header *header = new t_eml_header();
        t_Lumps *lumps = new t_Lumps[maxLumps];

        inFILE.seekg(0, inFILE.beg);
        inFILE.read((char *)&header[0], sizeof(t_eml_header));

        if ((int)header->format != EMLHEADER)
        {
            std::cout << "This file is not a valid SEML file. " << std::endl;
            Log::WriteToLog("Not a valid EML file: " + emlPath);
            return false;
        }

        if ((float)header->version != emlVersion)
        {
            std::cout << "This file version doesn't match the one required." << std::endl
                      << "This file: " << (float)header->version << " Required: " << emlVersion << std::endl;
            Log::WriteToLog("This file version doesn't match the one required.");
            Log::WriteToLog("This file: " + Helpers::floatTostring((float)header->version) + " Required: " + Helpers::floatTostring(emlVersion));
            return false;
        }

        //Read the lump of this file
        inFILE.seekg(sizeof(t_eml_header), inFILE.beg);
        for (int i = 0; i < maxLumps; ++i)
        {
            inFILE.read((char *)&lumps[i], sizeof(t_Lumps));
        }

        //Get the array of vertices in the file
        numvertices = lumps[lVertices].size / sizeof(t_Vertex);
        t_Vertex *l_vertices = new t_Vertex[numvertices];
        inFILE.seekg(lumps[lVertices].offset, inFILE.beg);
        for (int i = 0; i < numvertices; ++i)
        {
            inFILE.read((char *)&l_vertices[i], sizeof(t_Vertex));
        }

        //Get the array of indices in the file
        numIndices = lumps[lIndices].size / sizeof(t_Index);
        t_Index *l_indices = new t_Index[numIndices];
        inFILE.seekg(lumps[lIndices].offset, inFILE.beg);
        for (int i = 0; i < numIndices; ++i)
        {
            inFILE.read((char *)&l_indices[i], sizeof(t_Index));
        }

        //Get the array of meshes in the file
        numMeshes = (int)header->numOfMeshes;
        t_Mesh *l_meshes = new t_Mesh[numMeshes];
        inFILE.seekg(lumps[lMeshes].offset, inFILE.beg);
        for (int i = 0; i < numMeshes; ++i)
        {
            inFILE.read((char *)&l_meshes[i], sizeof(t_Mesh));
        }
        //std::cout << "Num of meshes: " << numMeshes << std::endl;

        MinMaxPoints.MAX_X = 0;
        MinMaxPoints.MAX_Y = 0;
        MinMaxPoints.MAX_Z = 0;

        MinMaxPoints.MIN_X = 0;
        MinMaxPoints.MIN_Y = 0;
        MinMaxPoints.MIN_Z = 0;

        //Pre Process the model and create it's bounding boxes
        try
        {
            mMeshesBoundingBoxes.reserve(numMeshes);
            mMeshesPositionsRelative.reserve(numMeshes);
            

            for (int i = 0; i < numMeshes; ++i)
            {
                std::vector<t_Vertex> tmpVertVector;
                std::vector<unsigned int> tmpIndicesVector;
                std::vector<Texture> tmpTexturesVector;
                MIN_MAX_POINTS tmp_MinMax;

                for (int j = 0; j < l_meshes[i].mNumVertices; ++j)
                {
                    tmpVertVector.push_back(l_vertices[l_meshes[i].mFirstVertex + j]);

                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.x > MinMaxPoints.MAX_X)
                        MinMaxPoints.MAX_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.y > MinMaxPoints.MAX_Y)
                        MinMaxPoints.MAX_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.z > MinMaxPoints.MAX_Z)
                        MinMaxPoints.MAX_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;

                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.x < MinMaxPoints.MIN_X)
                        MinMaxPoints.MIN_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.y < MinMaxPoints.MIN_Y)
                        MinMaxPoints.MIN_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.z < MinMaxPoints.MIN_Z)
                        MinMaxPoints.MIN_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;

                    //min max for this mesh
                    
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.x > tmp_MinMax.MAX_X)
                        tmp_MinMax.MAX_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.y > tmp_MinMax.MAX_Y)
                        tmp_MinMax.MAX_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.z > tmp_MinMax.MAX_Z)
                        tmp_MinMax.MAX_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;

                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.x < tmp_MinMax.MIN_X)
                        tmp_MinMax.MIN_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.y < tmp_MinMax.MIN_Y)
                        tmp_MinMax.MIN_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
                    if (l_vertices[l_meshes[i].mFirstVertex + j].position.z < tmp_MinMax.MIN_Z)
                        tmp_MinMax.MIN_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;

                }

                mMeshesBoundingBoxes.push_back(tmp_MinMax);
                float dx, dy, dz;
                dx = (mMeshesBoundingBoxes.back().MAX_X - mMeshesBoundingBoxes.back().MIN_X);
                dy = (mMeshesBoundingBoxes.back().MAX_Y - mMeshesBoundingBoxes.back().MIN_Y);
                dz = (mMeshesBoundingBoxes.back().MAX_Z - mMeshesBoundingBoxes.back().MIN_Z);

                glm::vec3 centerOfMass = glm::vec3(mMeshesBoundingBoxes.back().MIN_X + (dx * 0.5),
                                                   mMeshesBoundingBoxes.back().MIN_Y + (dy * 0.5),
                                                   mMeshesBoundingBoxes.back().MIN_Z + (dz * 0.5));

                mMeshesPositionsRelative.push_back(centerOfMass);
                mMeshesPositions.push_back(centerOfMass);

                for (int j = 0; j < l_meshes[i].mNumIndices; ++j)
                {
                    tmpIndicesVector.push_back(l_indices[l_meshes[i].mFirstIndex + j].index);
                }
                if (a == 0)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        Texture tex;
                        tex.id = 0;
                        tex.path = std::string(l_meshes[i].mMaterial[j]);
                        tex.type = i;
                        ResourceManager::Get().addTextureToQueue(tex.path);
                        tmpTexturesVector.push_back(tex);
                    }
                }
                if (!a)
                {
                    meshes.push_back(Mesh(tmpVertVector, tmpIndicesVector, tmpTexturesVector, ResourceManager::Get().NearestCubeMap(Position)));
                }
                else
                    meshes.push_back(Mesh(tmpVertVector, tmpIndicesVector, tmpTexturesVector));
            }

            float dx, dy, dz;
            dx = (MinMaxPoints.MAX_X - MinMaxPoints.MIN_X);
            dy = (MinMaxPoints.MAX_Y - MinMaxPoints.MIN_Y);
            dz = (MinMaxPoints.MAX_Z - MinMaxPoints.MIN_Z);

            glm::vec3 centerOfMass = glm::vec3(MinMaxPoints.MIN_X + (dx * 0.5),
                                               MinMaxPoints.MIN_Y + (dy * 0.5),
                                               MinMaxPoints.MIN_Z + (dz * 0.5));

            glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

            glm::vec3 delta = origin - centerOfMass;

            MinMaxPoints.MIN_X += delta.x;
            MinMaxPoints.MIN_Y += delta.y;
            MinMaxPoints.MIN_Z += delta.z;

            MinMaxPoints.MAX_X += delta.x;
            MinMaxPoints.MAX_Y += delta.y;
            MinMaxPoints.MAX_Z += delta.z;

            for (int i = 0; i < numMeshes; ++i)
            {
                for (int j = 0; j < meshes.at(i).vertices.size(); j++)
                {
                    meshes.at(i).vertices[j].position.x += delta.x;
                    meshes.at(i).vertices[j].position.y += delta.y;
                    meshes.at(i).vertices[j].position.z += delta.z;
                }
                /*mMeshesBoundingBoxes.at(i).MIN_X += delta.x;
                mMeshesBoundingBoxes.at(i).MIN_Y += delta.y;
                mMeshesBoundingBoxes.at(i).MIN_Z += delta.z;

                mMeshesBoundingBoxes.at(i).MAX_X += delta.x;
                mMeshesBoundingBoxes.at(i).MAX_Y += delta.y;
                mMeshesBoundingBoxes.at(i).MAX_Z += delta.z;*/
                meshes.at(i).setupMesh();
            }
        }
        catch (std::exception &e)
        {
            throw std::runtime_error(std::string("Exception while pre processing the model ::: ") + std::string(e.what()));
        }

        delete header;
        delete[] lumps;
        delete[] l_vertices;
        delete[] l_indices;
        delete[] l_meshes;

        inFILE.close();
        Log::WriteToLog("Model: " + std::string(emlPath) + " Loaded.");

        return true;
    }

    void Model::Draw(std::shared_ptr<Shader> shader)
    {

        for (GLuint i = 0; i < this->meshes.size(); i++)
        {
            this->meshes[i].Draw(shader, mMeshesPositions.at(i));
        }
    }

    void Model::SetUniforms(std::shared_ptr<Shader> shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation,
                            glm::vec3 pposition, glm::vec3 pscale, glm::quat protation,
                            std::shared_ptr<Camera> cam)
    {
        glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0), scale);
        glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0), position);
        glm::mat4 RotationMatrix = glm::mat4(1.0f) * glm::toMat4(glm::normalize(rotation));
        this->ModelMatrix = TranslationMatrix * ScaleMatrix * RotationMatrix;
        shader->PushUniform("model", this->ModelMatrix);

        ScaleMatrix = glm::scale(glm::mat4(1.0), pscale);
        TranslationMatrix = glm::translate(glm::mat4(1.0), pposition);
        RotationMatrix = glm::mat4(1) * glm::toMat4(glm::normalize(protation));
        this->PrevModel = TranslationMatrix * ScaleMatrix * RotationMatrix;
        glm::mat4 MVP = cam->getProjectionMatrix() * cam->getViewMatrix() * this->ModelMatrix;
        shader->PushUniform("MVP", MVP);
        shader->PushUniform("PrevModel", this->PrevModel);
        //glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "PrevModel"), 1, GL_FALSE, &this->PrevModel[0][0]);
        //glUniformMatrix4fv(shader->WorldTransform_Location, 1, GL_FALSE, &this->ModelMatrix[0][0]);
        glUniformMatrix4fv(shader->View_Location, 1, GL_FALSE, &cam->getViewMatrix()[0][0]);
        glm::mat3 invModelMatrix = glm::transpose(glm::inverse(glm::mat3(this->ModelMatrix)));
        glm::mat3 invNormalMatrix = glm::transpose(glm::inverse(glm::mat3(cam->getViewMatrix() * this->ModelMatrix)));
        shader->PushUniform("invModelMatrix", invModelMatrix);
        shader->PushUniform("invNormalMatrix", invNormalMatrix);
        glUniformMatrix4fv(shader->PrevViewPos_Location, 1, GL_FALSE, &cam->getPrevViewMatrix()[0][0]);
        glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
        glUniform3f(shader->viewPos_Location, cam->getPosition().x, cam->getPosition().y, cam->getPosition().z);

        //glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"), glfwGetTime());
        shader->PushUniform("time", (float)glfwGetTime());
        //glUniform2i(glGetUniformLocation(shader->getProgramID(), "Resolution"), 1920, 1080);
        shader->PushUniform("Resolution", glm::ivec2(Engine::Get().Width(), Engine::Get().Height()));
        int index = 0;
        if(!uniformsSet) IO::PrintLine("Position for model ", this->path, position.x, position.y, position.z);
        for(auto &i : mMeshesPositionsRelative) {
            mMeshesPositions.at(index) = (i * scale) + position;
            //IO::PrintLine("mesh position:", mMeshesPositions.at(index).x, mMeshesPositions.at(index).y, mMeshesPositions.at(index).z);
            index++;
        }
        
        uniformsSet = true;
    }
} // namespace Epsilon