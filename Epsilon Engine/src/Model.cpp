#include <Model.h>
#include <ResourceManager.h>

Model::Model(const char* path, std::shared_ptr<ResourceManager> rm, glm::vec3 pos, glm::vec3 sc, glm::quat rot)
{
    this->path = path;
    resm = rm;
    Position = pos;
    Scale = sc;
    Rotation = rot;

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
        return false;
    }

    if((float)header->version != emlVersion)
    {
        std::cout << "This file version doesn't match the one required."<< std::endl <<
                  "This file: " << (float)header->version << " Required: " << emlVersion << std::endl;
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



    MinMaxPoints.MAX_X = l_vertices[l_meshes[0].mFirstVertex].position.x;
    MinMaxPoints.MAX_Y = l_vertices[l_meshes[0].mFirstVertex].position.y;
    MinMaxPoints.MAX_Z = l_vertices[l_meshes[0].mFirstVertex].position.z;

    MinMaxPoints.MIN_X = l_vertices[l_meshes[0].mFirstVertex].position.x;
    MinMaxPoints.MIN_Y = l_vertices[l_meshes[0].mFirstVertex].position.y;
    MinMaxPoints.MIN_Z = l_vertices[l_meshes[0].mFirstVertex].position.z;

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
        if(!a){
            meshes.push_back(Mesh(tmpVertVector, tmpIndicesVector, tmpTexturesVector, resm->NearestCubeMap(Position)));
        }
        else
            meshes.push_back(Mesh(tmpVertVector, tmpIndicesVector, tmpTexturesVector));
    }


    delete header;
    delete[] lumps;
    delete[] l_vertices;
    delete[] l_indices;
    delete[] l_meshes;

    inFILE.close();
    return true;

}

void Model::Draw(Shader* shader)
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw(shader, this->resm);
    shader->Free();
}

void Model::Draw(GLuint shader)
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw(shader, this->resm);
}
