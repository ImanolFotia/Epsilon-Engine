#include <Model.h>
#include <ResourceManager.h>
#include <Log.h>

Model::Model(const char* path, std::shared_ptr<ResourceManager> rm, glm::vec3 pos, glm::vec3 sc, glm::quat rot)
{
    this->path = path;
    resm = rm;
    Position = pos;
    Scale = sc;
    Rotation = rot;
    PrevModel = glm::mat4();
    ModelMatrix = glm::mat4();

    this->loadModel(path, 0);
    //std::cout << "Resource manager in epsilon address: " << resm.get() << std::endl;
    //cout << "Cantidad de texturas: " << textures_loaded.size() << endl;
}


bool Model::loadModel(string emlPath, int a)
{
    //std::cout << "Open File: " << emlPath << std::endl;
    ifstream inFILE(emlPath, std::ios::binary);
    if(!inFILE.is_open())
    {
        std::cout << "Fail to open EML file" << std::endl;
        Global::Log::WriteToLog("Model: " + emlPath + " could not be loaded.");
        return false;
    }

    int numvertices = 0;
    int numMeshes = 0;
    int numIndices = 0;

    t_eml_header* header = new t_eml_header();
    t_Lumps* lumps = new t_Lumps[maxLumps];

    inFILE.seekg(0, inFILE.beg);
    inFILE.read((char*)&header[0], sizeof(t_eml_header));

    if((int)header->format != EMLHEADER)
    {
        std::cout << "This file is not a valid SEML file. " << std::endl;
        Global::Log::WriteToLog("Not a valid EML file: " + emlPath);
        return false;
    }

    if((float)header->version != emlVersion)
    {
        std::cout << "This file version doesn't match the one required."<< std::endl <<
                  "This file: " << (float)header->version << " Required: " << emlVersion << std::endl;
        Global::Log::WriteToLog("This file version doesn't match the one required.");
        Global::Log::WriteToLog("This file: " + Helpers::floatTostring((float)header->version) + " Required: " + Helpers::floatTostring(emlVersion));
        return false;
    }

    //Read the lump of this file
    inFILE.seekg(sizeof(t_eml_header), inFILE.beg);
    for(int i = 0; i < maxLumps; ++i)
    {
        inFILE.read((char*)&lumps[i], sizeof(t_Lumps));
    }

    //Get the array of vertices in the file
    numvertices = lumps[lVertices].size / sizeof(t_Vertex);
    t_Vertex* l_vertices = new t_Vertex[numvertices];
    inFILE.seekg(lumps[lVertices].offset, inFILE.beg);
    for(int i = 0; i < numvertices; ++i)
    {
        inFILE.read((char*)&l_vertices[i], sizeof(t_Vertex));
    }

    //std::cout << "Num of vertices: " << numvertices << std::endl;

    //Get the array of indices in the file
    numIndices = lumps[lIndices].size / sizeof(t_Index);
    t_Index* l_indices = new t_Index[numIndices];
    inFILE.seekg(lumps[lIndices].offset, inFILE.beg);
    for(int i = 0; i < numIndices; ++i)
    {
        inFILE.read((char*)&l_indices[i], sizeof(t_Index));
    }

    //std::cout << "Num of indices: " << numIndices << std::endl;
    //Get the array of meshes in the file
    numMeshes = (int)header->numOfMeshes;
    t_Mesh* l_meshes = new t_Mesh[numMeshes];
    inFILE.seekg(lumps[lMeshes].offset, inFILE.beg);
    for(int i = 0; i < numMeshes; ++i)
    {
        inFILE.read((char*)&l_meshes[i], sizeof(t_Mesh));
    }
    //std::cout << "Num of meshes: " << numMeshes << std::endl;



    MinMaxPoints.MAX_X = 0;
    MinMaxPoints.MAX_Y = 0;
    MinMaxPoints.MAX_Z = 0;

    MinMaxPoints.MIN_X = 0;
    MinMaxPoints.MIN_Y = 0;
    MinMaxPoints.MIN_Z = 0;

    for(int i = 0; i < numMeshes; ++i)
    {
        std::vector<t_Vertex> tmpVertVector;
        std::vector<unsigned int> tmpIndicesVector;
        std::vector<Texture> tmpTexturesVector;


        for(int j = 0; j < l_meshes[i].mNumVertices; ++j)
        {
            tmpVertVector.push_back(l_vertices[l_meshes[i].mFirstVertex + j]);

            if(l_vertices[l_meshes[i].mFirstVertex + j].position.x > MinMaxPoints.MAX_X)
                MinMaxPoints.MAX_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
            if(l_vertices[l_meshes[i].mFirstVertex + j].position.y > MinMaxPoints.MAX_Y)
                MinMaxPoints.MAX_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
            if(l_vertices[l_meshes[i].mFirstVertex + j].position.z > MinMaxPoints.MAX_Z)
                MinMaxPoints.MAX_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;


            if(l_vertices[l_meshes[i].mFirstVertex + j].position.x < MinMaxPoints.MIN_X)
                MinMaxPoints.MIN_X = l_vertices[l_meshes[i].mFirstVertex + j].position.x;
            if(l_vertices[l_meshes[i].mFirstVertex + j].position.y < MinMaxPoints.MIN_Y)
                MinMaxPoints.MIN_Y = l_vertices[l_meshes[i].mFirstVertex + j].position.y;
            if(l_vertices[l_meshes[i].mFirstVertex + j].position.z < MinMaxPoints.MIN_Z)
                MinMaxPoints.MIN_Z = l_vertices[l_meshes[i].mFirstVertex + j].position.z;
        }

        for(int j = 0; j < l_meshes[i].mNumIndices; ++j)
        {
            tmpIndicesVector.push_back(l_indices[l_meshes[i].mFirstIndex + j].index);
        }
        if(a == 0)
        {
            for(int j = 0; j < 4; ++j)
            {
                Texture tex;
                tex.id = 0;
                tex.path = std::string(l_meshes[i].mMaterial[j]);
                tex.type = i;
                resm->addTextureToQueue(tex.path);
                tmpTexturesVector.push_back(tex);
            }
        }
        if(!a)
        {
            meshes.push_back(Mesh(tmpVertVector, tmpIndicesVector, tmpTexturesVector, resm->NearestCubeMap(Position)));
        }
        else
            meshes.push_back(Mesh(tmpVertVector, tmpIndicesVector, tmpTexturesVector));
    }

    float dx, dy, dz;
    dx = (MinMaxPoints.MAX_X - MinMaxPoints.MIN_X);
    dy = (MinMaxPoints.MAX_Y - MinMaxPoints.MIN_Y);
    dz = (MinMaxPoints.MAX_Z - MinMaxPoints.MIN_Z);

    glm::vec3 centerOfMass = glm::vec3(MinMaxPoints.MIN_X + (dx*0.5),
                                       MinMaxPoints.MIN_Y + (dy*0.5),
                                       MinMaxPoints.MIN_Z + (dz*0.5));

    glm::vec3 origin = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 delta = origin - centerOfMass;

    for(int i = 0; i < numMeshes; ++i)
    {
        for(int j = 0; j < meshes.at(i).vertices.size(); j++){
            meshes.at(i).vertices[j].position.x += delta.x;
            meshes.at(i).vertices[j].position.y += delta.y;
            meshes.at(i).vertices[j].position.z += delta.z;
        }

        meshes.at(i).setupMesh();
    }

    MinMaxPoints.MIN_X += delta.x;
    MinMaxPoints.MIN_Y += delta.y;
    MinMaxPoints.MIN_Z += delta.z;

    MinMaxPoints.MAX_X += delta.x;
    MinMaxPoints.MAX_Y += delta.y;
    MinMaxPoints.MAX_Z += delta.z;
/*
        MinMaxPoints.MAX_X = ((MinMaxPoints.MAX_X * Position.x)* Scale.x);
        MinMaxPoints.MAX_Y = ((MinMaxPoints.MAX_Y * Position.y)* Scale.y);
        MinMaxPoints.MAX_Z = ((MinMaxPoints.MAX_Z * Position.z)* Scale.z);
        MinMaxPoints.MIN_X = ((MinMaxPoints.MIN_X * Position.x)* Scale.x);
        MinMaxPoints.MIN_Y = ((MinMaxPoints.MIN_Y * Position.y)* Scale.y);
        MinMaxPoints.MIN_Z = ((MinMaxPoints.MIN_Z * Position.z)* Scale.z);
*/
        //glm::vec3 M = glm::vec3(MinMaxPoints.MAX_X, MinMaxPoints.MAX_Y, MinMaxPoints.MAX_Z) * Rotation;
        //glm::vec3 m = glm::vec3(MinMaxPoints.MIN_X, MinMaxPoints.MIN_Y, MinMaxPoints.MIN_Z) * Rotation;

        //MinMaxPoints.MAX_X = M.x; MinMaxPoints.MAX_Y = M.y; MinMaxPoints.MAX_Z = M.z;
        //MinMaxPoints.MIN_X = m.x; MinMaxPoints.MIN_Y = m.y; MinMaxPoints.MIN_Z = m.z;


    delete header;
    delete[] lumps;
    delete[] l_vertices;
    delete[] l_indices;
    delete[] l_meshes;

    inFILE.close();
    Global::Log::WriteToLog("Model: " + std::string(emlPath) + " Loaded.");

    return true;

}

void Model::Draw(Shader* shader, glm::vec3 pos = glm::vec3(0,0,0))
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw(shader, this->resm, pos);
    //shader->Free();
}

void Model::Draw(GLuint shader, glm::vec3 pos = glm::vec3(0,0,0))
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw(shader, this->resm, pos);
}

void Model::SetUniforms(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation, std::shared_ptr<Camera> cam)
{

    this->PrevModel = this->ModelMatrix;
    glm::mat4 ScaleMatrix = glm::scale(glm::mat4(), scale);
    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), position);
    glm::mat4 RotationMatrix;
    RotationMatrix = glm::mat4(1) * glm::toMat4(glm::normalize(rotation));
    this->ModelMatrix = TranslationMatrix * ScaleMatrix * RotationMatrix;


    ScaleMatrix = glm::scale(glm::mat4(), PrevScale);
    TranslationMatrix = glm::translate(glm::mat4(), PrevPos);
    RotationMatrix = glm::mat4(1) * glm::toMat4(glm::normalize(PrevRot));
    this->PrevModel = TranslationMatrix * ScaleMatrix * RotationMatrix;

    glm::mat4 MVP = cam->getProjectionMatrix() * cam->getViewMatrix() * this->ModelMatrix;
    glUniformMatrix4fv(shader->MVP_Location, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "PrevModel"), 1, GL_FALSE, &this->PrevModel[0][0]);
    glUniformMatrix4fv(shader->WorldTransform_Location, 1, GL_FALSE, &this->ModelMatrix[0][0]);
    glUniformMatrix4fv(shader->View_Location, 1, GL_FALSE, &cam->getViewMatrix()[0][0]);
    glUniformMatrix4fv(shader->PrevViewPos_Location, 1, GL_FALSE, &cam->getPrevViewMatrix()[0][0]);
    glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniform3f(shader->viewPos_Location,  cam->getPosition().x, cam->getPosition().y, cam->getPosition().z);

    glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"),  glfwGetTime());
    glUniform2i(glGetUniformLocation(shader->getProgramID(), "Resolution"), 1920, 1080);
    PrevPos = position;
    PrevScale = scale;
    PrevRot = rotation;

}


void Model::SetUniforms(Shader* shader, glm::vec3 position, glm::vec3 scale, glm::quat rotation,
                                         glm::vec3 pposition, glm::vec3 pscale, glm::quat protation,
                                         std::shared_ptr<Camera> cam)
{

    glm::mat4 ScaleMatrix = glm::scale(glm::mat4(), scale);
    glm::mat4 TranslationMatrix = glm::translate(glm::mat4(), position);
    glm::mat4 RotationMatrix;
    RotationMatrix = glm::mat4(1) * glm::toMat4(glm::normalize(rotation));
    this->ModelMatrix = TranslationMatrix * ScaleMatrix * RotationMatrix;

    ScaleMatrix = glm::scale(glm::mat4(), pscale);
    TranslationMatrix = glm::translate(glm::mat4(), pposition);
    RotationMatrix = glm::mat4(1) * glm::toMat4(glm::normalize(protation));
    this->PrevModel = TranslationMatrix * ScaleMatrix * RotationMatrix;
    glm::mat4 MVP = cam->getProjectionMatrix() * cam->getViewMatrix() * this->ModelMatrix;
    glUniformMatrix4fv(shader->MVP_Location, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shader->getProgramID(), "PrevModel"), 1, GL_FALSE, &this->PrevModel[0][0]);
    glUniformMatrix4fv(shader->WorldTransform_Location, 1, GL_FALSE, &this->ModelMatrix[0][0]);
    glUniformMatrix4fv(shader->View_Location, 1, GL_FALSE, &cam->getViewMatrix()[0][0]);
    glUniformMatrix4fv(shader->PrevViewPos_Location, 1, GL_FALSE, &cam->getPrevViewMatrix()[0][0]);
    glUniformMatrix4fv(shader->Projection_Location, 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);
    glUniform3f(shader->viewPos_Location,  cam->getPosition().x, cam->getPosition().y, cam->getPosition().z);

    glUniform1f(glGetUniformLocation(shader->getProgramID(), "time"),  glfwGetTime());
    glUniform2i(glGetUniformLocation(shader->getProgramID(), "Resolution"), 1920, 1080);

}
