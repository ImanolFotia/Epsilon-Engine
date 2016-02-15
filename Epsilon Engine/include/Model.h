///========= Copyright Survtech, All rights reserved. ============//
///
/// Purpose:
///
///=============================================================================
#pragma once
/// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;
/// GL Includes
#include <GL/glew.h> /// Contains all the necessery OpenGL includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <Texture.h>
#include <Mesh.h>


class Model
{
public:
    /**  Functions   */
    /// Constructor, expects a filepath to a 3D model.
    Model(GLchar* path)
    {
        this->loadModel(path);
        //cout << "Cantidad de texturas: " << textures_loaded.size() << endl;
    }
    string directory;
    /// Draws the model, and thus all its meshes
    void Draw(Shader* shader)
    {
        for(GLuint i = 0; i < this->meshes.size(); i++)
            this->meshes[i].Draw(shader);
        shader->Free();
    }

    vector<Mesh> meshes;

    vector<Texture> textures_loaded;

    /// Structure to store the models bounding box for visibility and collision computation
    typedef struct BOUNDING_BOX
    {
        float FRONT_TOP_LEFT;
        float FRONT_TOP_RIGHT;
        float FRONT_BOTTOM_LEFT;
        float FRONT_BOTTOM_RIGHT;

        float BACK_TOP_LEFT;
        float BACK_TOP_RIGHT;
        float BACK_BOTTOM_LEFT;
        float BACK_BOTTOM_RIGHT;
    };

    typedef struct MIN_MAX_POINTS
    {
        float MAX_X;
        float MAX_Y;
        float MAX_Z;

        float MIN_X;
        float MIN_Y;
        float MIN_Z;
    };

    MIN_MAX_POINTS MinMaxPoints;

    BOUNDING_BOX BoundingBox;


private:
    /**  Model Data  */

    /// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    bool CubeMapLoaded = false;

    /**  Functions   */
    /// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string path)
    {
        /// Read file via ASSIMP
        const aiScene* scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs | aiProcess_GenUVCoords);
        /// Check for errors
        if(!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            /// cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        this->directory = path.substr(0, path.find_last_of('/'));

        /// Process ASSIMP's root node recursively
        this->processNode(scene->mRootNode, scene);

        aiReleaseImport(scene);
    }

    /// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
    {
        /// Process each mesh located at the current node
        for(GLuint i = 0; i < node->mNumMeshes; i++)
        {
            /// The node object only contains indices to index the actual objects in the scene.
            /// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh, scene));
        }
        /// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
        for(GLuint i = 0; i < node->mNumChildren; i++)
        {
            this->processNode(node->mChildren[i], scene);
        }

    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        /// Data to fill
        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;

        MinMaxPoints.MAX_X = 0;
        MinMaxPoints.MAX_Y = 0;
        MinMaxPoints.MAX_Z = 0;

        MinMaxPoints.MIN_X = 0;
        MinMaxPoints.MIN_Y = 0;
        MinMaxPoints.MIN_Z = 0;

        /// Walk through each of the mesh's vertices
        for(GLuint i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            glm::vec3 vector; /// We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
            /// Positions
            vector.x = mesh->mVertices[i].x;
            vector.y = mesh->mVertices[i].y;
            vector.z = mesh->mVertices[i].z;

            if(vector.x > MinMaxPoints.MAX_X)
            MinMaxPoints.MAX_X = vector.x;
            if(vector.y > MinMaxPoints.MAX_Y)
            MinMaxPoints.MAX_Y = vector.y;
            if(vector.z > MinMaxPoints.MAX_Z)
            MinMaxPoints.MAX_Z = vector.z;


            if(vector.x < MinMaxPoints.MIN_X)
            MinMaxPoints.MIN_X = vector.x;
            if(vector.y < MinMaxPoints.MIN_Y)
            MinMaxPoints.MIN_Y = vector.y;
            if(vector.z < MinMaxPoints.MIN_Z)
            MinMaxPoints.MIN_Z = vector.z;

            vertex.Position = vector;
            /// Normals
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = glm::normalize(vector);
            /// Tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = glm::normalize(vector);
            /// Bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = glm::normalize(vector);


            /// Texture Coordinates
            if(mesh->mTextureCoords[0]) /// Does the mesh contain texture coordinates?
            {
                glm::vec2 vec;
                /// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                /// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                vec.x = mesh->mTextureCoords[0][i].x;
                vec.y = mesh->mTextureCoords[0][i].y;
                vertex.TexCoords = vec;
            }
            else
                vertex.TexCoords = glm::vec2(0.0f, 0.0f);
            vertices.push_back(vertex);
        }
        /// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
        for(GLuint i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            /// Retrieve all indices of the face and store them in the indices vector
            for(GLuint j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }
        /// Process materials
        if(mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            /// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
            /// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.
            /// Same applies to other texture as the following list summarizes:
            /// Diffuse: texture_diffuseN
            /// Specular: texture_specularN
            /// Normal: texture_normalN

            /// 1. Diffuse maps
            vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            /// 2. Specular maps
            vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            /// 2. Normal maps
            vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            /// 2. Heigth maps
            vector<Texture> heigthMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heigthMaps.begin(), heigthMaps.end());
            /**
            Texture Tex;
            Tex.id = loadCubemap();
            Tex.type = "CubeMap";
            Tex.path = "materials/skyboxes/Miramar/right.tga";
            textures.push_back(Tex);
            */
        }

        //cout << "MaxPoints: " << MinMaxPoints.MAX_X << " " << MinMaxPoints.MAX_Y << " " << MinMaxPoints.MAX_Z << endl;
        //cout << "MinPoints: " << MinMaxPoints.MIN_X << " " << MinMaxPoints.MIN_Y << " " << MinMaxPoints.MIN_Z << endl;

        /// Return a mesh object created from the extracted mesh data
        return Mesh(vertices, indices, textures);
    }

    /// Checks all material textures of a given type and loads the textures if they're not loaded yet.
    /// The required info is returned as a Texture struct.
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
    {
        vector<Texture> TMPtextures;
        for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            string path = "materials/" + std::string(str.C_Str());
            /// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            GLboolean skip = false;
            for(GLuint j = 0; j < textures_loaded.size(); j++)
            {
                if(std::string(textures_loaded[j].path.C_Str()) == path)
                {
                    TMPtextures.push_back(textures_loaded[j]);
                    skip = true; /// A texature with the same filepath has already been loaded, continue to next one. (optimization)
                    break;
                }
            }
            if(!skip)
            {
                /// If texture hasn't been loaded already, load it
                Texture texture;
                eTexture texID(str.C_Str());
                texture.id = texID.texture;
                texture.type = typeName;
                texture.path = path;
                TMPtextures.push_back(texture);
                this->textures_loaded.push_back(texture);  /// Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            }
        }
/**
        if(typeName == "CubeMap")
        {
            Texture texture;
            texture.id = loadCubemap();
            texture.type = "CubeMap";
            texture.path = "materials/skyboxes/Miramar/right.tga";
            TMPtextures.push_back(texture);
            this->textures_loaded.push_back(texture);  /// Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
            CubeMapLoaded = true;
            return TMPtextures;
        }
*/
        return TMPtextures;
    }

    GLuint loadCubemap()
    {
        vector<std::string> faces;
        faces.push_back("materials/skyboxes/Miramar_Blurred/right.tga");
        faces.push_back("materials/skyboxes/Miramar_Blurred/left.tga");
        faces.push_back("materials/skyboxes/Miramar_Blurred/top.tga");
        faces.push_back("materials/skyboxes/Miramar_Blurred/bottom.tga");
        faces.push_back("materials/skyboxes/Miramar_Blurred/back.tga");
        faces.push_back("materials/skyboxes/Miramar_Blurred/front.tga");

    GLuint CubeMapID;
    glGenTextures(1, &CubeMapID);
    int width,height, channels;
    unsigned char* image;
    glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapID);

    for(GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i].c_str(), &width, &height, &channels, SOIL_LOAD_AUTO);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        return CubeMapID;
    }

    void createBoundingBox()
    {

    }
};



