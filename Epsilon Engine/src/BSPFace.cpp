#include <Physics.h>
#include <ResourceManager.h>
#include <BSPFace.h>

bool BSPFace::BuildFace(std::vector<glm::vec3> Vertices, std::vector<glm::vec3> Normals, std::vector<glm::vec2> TexCoords, std::vector<glm::vec2> LMTexCoords, std::vector<unsigned int> Indices, int ID, string imagePath, tBSPLightmap LightMap, std::shared_ptr<ResourceManager> Resm)
{
    /// cout << "Face #" << ID << endl;
    this->Vertices = Vertices;

    this->TexCoords = TexCoords;

    this->Indices = Indices;
    this->Normals = Normals;
    this->faceID = ID;
    this->ObjectID = "BSPFace_" + Helpers::intTostring(ID);
    //std::cout << this->ObjectID << std::endl;
    this->imagePath = imagePath;
    this->imagePath = this->imagePath;
    //std::cout << imagePath << std::endl;
    this->LightMap = LightMap;
    this->LMTexCoords = LMTexCoords;

    this->CalcTangentSpace();
    for(unsigned int i = 0; i < Vertices.size(); i++)
    {
        t_Vertex vert;
        vert.position = Vertices[i];
        vert.texcoord = TexCoords[i];
        vert.normal = glm::normalize(-Normals[i]);
        vert.tangent = -Tangents[i];
        vert.bitangent = -Bitangents[i];
        mVertices.push_back(vert);

        mPosition += Vertices[i];
    }

    mPosition /= Vertices.size();

    bool res = this->prepareVAO();
    this->resm = Resm;

    std::shared_ptr<Physics::TriangleMeshPhysicObject> ph = (std::shared_ptr<Physics::TriangleMeshPhysicObject>) new Physics::TriangleMeshPhysicObject();

    rigidBody = nullptr;

    rigidBody = ph->addObject(this->Vertices, this->Indices, 0.1);
    //mPosition = glm::vec3(rigidBody->getCenterOfMassPosition().x(), rigidBody->getCenterOfMassPosition().y(), rigidBody->getCenterOfMassPosition().z());

    collinfo->setName(this->ObjectID);

    ph->Body->setUserPointer(collinfo.get());

    resm->m_PhysicsWorld->world->addRigidBody(rigidBody.get());

    this->CollisionObject = ph;

    //this->LoadLightMapTexture();

    return true;
}

void BSPFace::RenderFace(GLuint shader, GLuint TextureID,GLuint normalID, GLuint specularID, GLuint metallicID, bool simpleRender)
{

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureID);
    glUniform1i(glGetUniformLocation(shader, "texture_diffuse"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularID);
    glUniform1i(glGetUniformLocation(shader, "texture_specular"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, normalID);
    glUniform1i(glGetUniformLocation(shader, "texture_normal"), 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, metallicID);
    glUniform1i(glGetUniformLocation(shader, "texture_height"), 3);

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(shader, "skybox"), 4);
    if(simpleRender)
        glBindTexture(GL_TEXTURE_CUBE_MAP, resm->useCubeMap(2));
    else
        glBindTexture(GL_TEXTURE_CUBE_MAP, resm->useCubeMap(resm->mCubemapIndex.at(resm->NearestCubeMap(mPosition))));

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->Indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}
