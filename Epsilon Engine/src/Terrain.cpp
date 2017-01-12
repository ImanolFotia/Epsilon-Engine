///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================

#include <iostream>
#include <fstream>
#include <Terrain.h>
#include <SOIL.h>
#include <Physics.h>
#include <ResourceManager.h>
#include <exception>

#include <Includes.h>

Terrain::Terrain(const char* heightMap,const char* diffuseTexture, float scale, int gridSize/**Must be Power of two*/, std::shared_ptr<ResourceManager> rm)
{
    this->GL_n_texture = 0;

    this->rM = rm;

    this->gridSize = gridSize;

    this->heightMap = heightMap;

    this->diffuseTexture = diffuseTexture;

    this->GetHeightData();

    this->LoadTexture(diffuseTexture);

    this->GenerateVertexBuffers();

}

Terrain::Terrain(const char* heightMap,const char* diffuseTexture, const char* normalTexture, const char* specularTexture, float sc, int gridSize, glm::vec3 Position ,std::shared_ptr<ResourceManager> rm /**Must be Power of two*/)
{
    this->gridSize = gridSize;

    this->heightMap = heightMap;

    this->scale = sc;

    this->rM = rm;

    this->m_Position = Position;

    this->diffuseTexture = diffuseTexture;

    this->specularTexture = specularTexture;

    this->GetHeightData();

    this->LoadTexture(diffuseTexture, normalTexture, specularTexture);

    this->GenerateVertexBuffers();

}

bool Terrain::GetHeightData()
{
    int w, h;

    unsigned char* pixels = SOIL_load_image(this->heightMap, &this->width, &this->height, 0, SOIL_LOAD_L);

    if(this->width != this->height)
    {
        cout << "TERRAIN ERROR: THE TEXTURE'S SIZE PROVIDED IS NOT POWER OF TWO::ABORTING" << endl;
        return 1;
    }

    this->cantPixels = this->width * this->height;

    this->GenerateGrid(pixels);

    SOIL_free_image_data(pixels);

}

bool Terrain::GenerateGrid(unsigned char* pixels)
{
    TVertex vert;


    float texU = float(this->height);
    float texV = float(this->height);
    int counter = 0;

    vector< vector<float> > Grid;
    vector<float> row;

    for(int i = 0 ; i < this->width ; i++)
    {
        for(int j = 0 ; j < this->height ; j++)
        {
            vert.Position.x = ((float)i + m_Position.x) * scale;
            vert.Position.y = ((float)pixels[counter]/4) + m_Position.y;
            vert.Position.z = ((float)j + m_Position.z) * scale;
            row.push_back(vert.Position.y);
            float fScaleC = float(j)/float(this->height-1);
            float fScaleR = float(i)/float(this->width-1);
            vert.TexCoords.s = this->width*fScaleC/this->width*30;
            vert.TexCoords.t = this->height*fScaleR/this->height*30;
            vertices.push_back(vert);
            counter++;

        }
        Grid.push_back(row);
        row.clear();
    }

    //cout << Grid.size() << endl;
    //cout << Grid.at(0).size() << endl;

    int mod = 0;
    counter = 0;
    for(int i = 0 ; i < this->width ; i++)
    {
        for(int j = 0 ; j < this->width ; j++)
        {
            if(i > 0 && j > 0 && i < Grid.size()-1 && j < Grid.size()-1)
            {
                float HL = Grid.at(i-1).at(j);
                float HR = Grid.at(i+1).at(j);
                float HD = Grid.at(i).at(j-1);
                float HU = Grid.at(i).at(j+1);

                glm::vec3 norm = glm::vec3(HL - HR, 2.0f, HD - HU);

                Normals.push_back(norm);
            }
            else
            {
                Normals.push_back(glm::vec3(0,1,0));
            }


        }
    }

    counter= 0;

    for(int i = 0 ; i < this->cantPixels-this->width ; i++)
    {
        if(counter == this->width-1)
        {
            counter = 0;
            mod++;
            continue;
        }
        Face face;

        face.vertex0 = vertices[i+1].Position;
        indices.push_back(i+1);
        face.vertex1 = vertices[i+this->width].Position;
        indices.push_back(i+this->width);
        face.vertex2 = vertices[i+this->width - this->width].Position;
        indices.push_back(i+this->width - this->width);

        vface.push_back(face);

        face.vertex0 = vertices[i + 1].Position;
        indices.push_back(i + 1);
        face.vertex1 = vertices[i+this->width+1].Position;
        indices.push_back(i+this->width+1);
        face.vertex2 = vertices[i+this->width].Position;
        indices.push_back(i+this->width);

        vface.push_back(face);

        counter++;

    }

    std::vector<glm::vec3> tmp_vertices;

    std::cout << indices.size() << std::endl;

    for(int i = 0; i < vertices.size(); ++i)
        tmp_vertices.push_back(vertices[i].Position);

    std::cout << "llega" << std::endl;

    try {
        std::shared_ptr<Physics::TriangleMeshPhysicObject> TerrainPhysicsMesh = (std::shared_ptr<Physics::TriangleMeshPhysicObject>) new Physics::TriangleMeshPhysicObject();

        this->rigidBody = nullptr;

        this->rigidBody = TerrainPhysicsMesh->addObject(tmp_vertices, this->indices, 1.0f);

        this->collinfo = (std::shared_ptr<Physics::CollisionInfo>) new Physics::CollisionInfo();

        this->collinfo->setName("Terrain");

        TerrainPhysicsMesh->Body->setUserPointer(collinfo.get());

        this->rM->m_PhysicsWorld->world->addRigidBody(rigidBody.get());

        this->CollisionObject = TerrainPhysicsMesh;
    }
    catch(exception e)
    {
        std::cout << e.what() << std::endl;
    }

    std::cout << "llega 2 " << std::endl;

    /*
        indices.pop_back();
        indices.pop_back();
        indices.pop_back();

        indices.pop_back();
        indices.pop_back();
        indices.pop_back();*/
    calculateTangentSpace();
}

bool Terrain::calculateTangentSpace()
{
    glm::vec3 tangent1, bitangent1;
    for(int i = 0 ; i < vface.size() ; i++)
    {
        glm::vec3 edge1 = vertices.at(indices.at((i+1))).Position + vertices.at(indices.at((i))).Position;
        glm::vec3 edge2 = vertices.at(indices.at((i+2))).Position + vertices.at(indices.at((i))).Position;

        glm::vec2 deltaUV1 = vertices.at(indices.at((i+2))).TexCoords + vertices.at(indices.at((i))).TexCoords;
        glm::vec2 deltaUV2 = vertices.at(indices.at((i+1))).TexCoords + vertices.at(indices.at((i))).TexCoords;

        float f = (deltaUV2.x * deltaUV1.x + deltaUV2.y * deltaUV1.y) == 0.0f ? 1.0f : -1.0f;
        // when t1, t2, t3 in same position in UV space, just use default UV direction.
        if ( 0 == deltaUV2.x && 0 ==deltaUV2.y && 0 == deltaUV1.x && 0 == deltaUV1.y )
        {
            deltaUV1.x = 0.0;
            deltaUV1.y = 1.0;
            deltaUV2.y = 1.0;
            deltaUV2.x = 0.0;
        }

        tangent1.x = f * (deltaUV2.y * edge1.x + deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y + deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z + deltaUV1.y * edge2.z);
        if(tangent1.x == 0 && tangent1.y == 0 && tangent1.z == 0)
            tangent1 = glm::vec3(0,0,1);
        tangent1 = glm::normalize(tangent1);

        Tangents.push_back(tangent1);
        Tangents.push_back(tangent1);
        Tangents.push_back(tangent1);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
        if(tangent1.x == 0 && tangent1.y == 0 && tangent1.z == 0)
            bitangent1 = glm::vec3(0,0,1);
        bitangent1 = glm::normalize(bitangent1);

        Bitangents.push_back(bitangent1);
        Bitangents.push_back(bitangent1);
        Bitangents.push_back(bitangent1);
    }
    return true;
}

bool Terrain::LoadTexture(const char* diff)
{
    int texwidth, texheight, texchannels;

    eTexture dTex(diff);

    GL_d_texture = dTex.getTextureID();

    texwidth = dTex.getWidth();

    texheight = dTex.getWidth();

}

bool Terrain::LoadTexture(const char* diff, const char* normal, const char* specular)
{
    int texwidth, texheight, texchannels;

    eTexture dTex(diff);

    eTexture nTex(normal);

    eTexture sTex(specular);

    eTexture Decal("decal.png", GL_CLAMP_TO_BORDER);

    GL_d_texture = dTex.getTextureID();

    GL_n_texture = nTex.getTextureID();

    GL_s_texture = sTex.getTextureID();

    GL_decal_texture = Decal.getTextureID();

    texwidth = dTex.getWidth();

    texheight = dTex.getWidth();

}

bool Terrain::GenerateVertexBuffers()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->NVB);
    glGenBuffers(1, &this->TVB);
    glGenBuffers(1, &this->BVB);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    /** Vertex And UV buffer Attachments*/

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(TVertex), &this->vertices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid*)offsetof(TVertex, TexCoords));

    /** Normal buffer Attachments*/

    glBindBuffer(GL_ARRAY_BUFFER, this->NVB);
    glBufferData(GL_ARRAY_BUFFER, this->Normals.size() * sizeof(glm::vec3), &this->Normals[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Normals), (GLvoid*)0);

    /** Normal buffer Attachments*/

    glBindBuffer(GL_ARRAY_BUFFER, this->TVB);
    glBufferData(GL_ARRAY_BUFFER, this->Tangents.size() * sizeof(glm::vec3), &this->Tangents[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Tangents), (GLvoid*)0);

    /** Normal buffer Attachments*/

    glBindBuffer(GL_ARRAY_BUFFER, this->BVB);
    glBufferData(GL_ARRAY_BUFFER, this->Bitangents.size() * sizeof(glm::vec3), &this->Bitangents[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Bitangents), (GLvoid*)0);

    /** Element buffer Attachments*/

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Terrain::RenderTerrain(Shader* shader)
{

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_diffuse"), 0);
    glBindTexture(GL_TEXTURE_2D, GL_d_texture);

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_specular"), 1);
    glBindTexture(GL_TEXTURE_2D, GL_s_texture);

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_normal"), 2);
    glBindTexture(GL_TEXTURE_2D, GL_n_texture);
/*
    glActiveTexture(GL_TEXTURE5);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_decal"), 5);
    glBindTexture(GL_TEXTURE_2D, GL_decal_texture);
*/
    // Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, 0);

}

