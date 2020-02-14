
#include <include/BSPFace.h>

bool BSPFace::BuildFace(std::vector<glm::vec3> Vertices, std::vector<glm::vec3> Normals, std::vector<glm::vec2> TexCoords, std::vector<glm::vec2> LMTexCoords, std::vector<unsigned int> Indices, int ID, string imagePath, tBSPLightmap LightMap)
{
    /// cout << "Face #" << ID << endl;
    this->Vertices = Vertices;

    this->TexCoords = TexCoords;

    this->Indices = Indices;
    this->Normals = Normals;
    this->faceID = ID;
    //std::cout << this->ObjectID << std::endl;
    this->imagePath = imagePath;
    this->imagePath = this->imagePath;
    //std::cout << imagePath << std::endl;
    //this->LightMap = LightMap;
    this->LMTexCoords = LMTexCoords;
    this->CalcTangentSpace();
    this->prepareVAO();


    //this->LoadLightMapTexture();

    return true;
}
