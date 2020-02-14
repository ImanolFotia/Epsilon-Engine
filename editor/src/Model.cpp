#include <include/Model.h>
#include <include/Types.h>
#include <image.hpp>
#include <include/texture.hpp>

Model::Model(const char* path, glm::vec3 pos, glm::vec3 sc, glm::quat rot)
{
    this->path = path;
    Position = pos;
    Scale = sc;
    Rotation = rot;

    this->loadModel(path, 1);
    //cout << "Cantidad de texturas: " << textures_loaded.size() << endl;
}


bool Model::loadModel(string emlPath, int a)
{
    std::cout << "Open File: " << emlPath << std::endl;
    ifstream inFILE(emlPath, std::ios::binary);
    if(!inFILE.is_open())
    {
        inFILE.open(std::string("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/models/") + emlPath, std::ios::binary);
        if(!inFILE.is_open())
        {
            std::cout << "Fail to open EML file" << std::endl;
            return false;
        }
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

    std::cout << "Num of vertices: " << numvertices << std::endl;

    //Get the array of indices in the file
    numIndices = lumps[lIndices].size / sizeof(t_Index);
    t_Index* l_indices = new t_Index[numIndices];
    inFILE.seekg(lumps[lIndices].offset, inFILE.beg);
    for(int i = 0; i < numIndices; ++i)
    {
        inFILE.read((char*)&l_indices[i], sizeof(t_Index));
    }

    std::cout << "Num of indices: " << numIndices << std::endl;
    //Get the array of meshes in the file
    numMeshes = (int)header->numOfMeshes;
    t_Mesh* l_meshes = new t_Mesh[numMeshes];
    inFILE.seekg(lumps[lMeshes].offset, inFILE.beg);
    for(int i = 0; i < numMeshes; ++i)
    {
        inFILE.read((char*)&l_meshes[i], sizeof(t_Mesh));
    }
    std::cout << "Num of meshes: " << numMeshes << std::endl;

    for(int i = 0; i < numMeshes; ++i)
    {

        std::cout << "Mesh: " << i << std::endl;

        std::vector<t_Vertex> tmpVertVector;
        std::vector<unsigned int> tmpIndicesVector;
        std::vector<std::shared_ptr<OpenGL::Texture> > tmpTexturesVector;

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


        bool skip = false;
        std::shared_ptr<OpenGL::Texture> tex[4];
        try{
            if(i > 0)
                for(int k = 0 ; k < meshes.size() ; k++)
                {
                    for(int z = 0; z < meshes.at(k).textures.size(); z++){
                        if(l_meshes[i].mMaterial[z] == meshes.at(k).textures.at(z)->getRelativePath())
                        {
                            std::shared_ptr<OpenGL::Texture> tex;
                            tex = meshes.at(k).textures.at(z);
                            tmpTexturesVector.push_back(tex);
                            skip = true;
                            break;
                        }
                    }
                }
        }
        catch(std::exception e)
        {
            std::cout << e.what() << std::endl;
        }

        if(!skip)
        for(int k = 0; k < 4; k++){
            std::string path = std::string(std::string("C:/Users/Imanol/Documents/Code/C++/epsilon-engine/Epsilon Engine/bin/Release/materials/") + l_meshes[i].mMaterial[k]);
            tex[k] = nullptr;
            tex[k] = (std::shared_ptr<OpenGL::Texture>) new OpenGL::Texture();
            //std::cout << "Texture address in Model: " << &tex[k] << std::endl;
            tex[k]->Load(path);
            std::cout << "Llega" << std::endl;
            tmpTexturesVector.push_back(tex[k]);
        }


        meshes.push_back(Mesh(tmpVertVector, tmpIndicesVector, tmpTexturesVector));
        std::cout << "added Mesh" << std::endl;
    }

    m_Centroid.x =  ((this->MinMaxPoints.MAX_X - this->MinMaxPoints.MIN_X));
    m_Centroid.y =  ((this->MinMaxPoints.MAX_Y - this->MinMaxPoints.MIN_Y));
    m_Centroid.z =  ((this->MinMaxPoints.MAX_Z - this->MinMaxPoints.MIN_Z));

    m_Centroid.x = this->MinMaxPoints.MIN_X + m_Centroid.x * 0.5f;
    m_Centroid.y = this->MinMaxPoints.MIN_Y + m_Centroid.y * 0.5f;
    m_Centroid.z = this->MinMaxPoints.MIN_Z + m_Centroid.z * 0.5f;


    glm::vec3 offsetVector = glm::vec3(0.0f) - m_Centroid;
    std::cout << "Length to move " << path << " by: " << glm::length(offsetVector) << std::endl;

    std::cout << "m_Centroid vector: " << m_Centroid.x << " " << m_Centroid.y << " " << m_Centroid.z << std::endl;
    std::cout << "Offset vector: " << offsetVector.x << " " << offsetVector.y << " " << offsetVector.z << std::endl;

    this->MinMaxPoints.MAX_X += offsetVector.x;
    this->MinMaxPoints.MAX_Y += offsetVector.y;
    this->MinMaxPoints.MAX_Z += offsetVector.z;
    this->MinMaxPoints.MIN_X += offsetVector.x;
    this->MinMaxPoints.MIN_Y += offsetVector.y;
    this->MinMaxPoints.MIN_Z += offsetVector.z;

    for(auto &m: meshes) {
        for(auto &v: m.vertices) {
            v.position.x += offsetVector.x;
            v.position.y += offsetVector.y;
            v.position.z += offsetVector.z;
        }
        m.setupMesh();
    }

    delete header;
    delete[] lumps;
    delete[] l_vertices;
    delete[] l_indices;
    delete[] l_meshes;

    inFILE.close();
    return true;

}


void Model::Draw(GLuint shader)
{
    for(GLuint i = 0; i < this->meshes.size(); i++)
        this->meshes[i].Draw(shader);
}
