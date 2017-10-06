#include <Mesh.h>
#include <ResourceManager.h>


/// Render the mesh
void Mesh::Draw(Shader*& shader, std::shared_ptr<ResourceManager> resm, glm::vec3 pos)
{


    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_diffuse"), 0);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[0].path));


    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_specular"), 1);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[1].path));


    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_normal"), 2);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[2].path));


    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "texture_height"), 3);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[3].path));

    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(shader->getProgramID(), "skybox"), 4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, resm->useCubeMap(resm->mCubemapIndex.at(resm->NearestCubeMap(pos))));

    /// Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

/// Render the mesh
void Mesh::Draw(GLuint shader, std::shared_ptr<ResourceManager> resm, glm::vec3 pos)
{

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader, "texture_diffuse"), 0);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[0].path));


    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader, "texture_specular"), 1);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[1].path));


    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader, "texture_normal"), 2);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[2].path));


    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(shader, "texture_height"), 3);
    glBindTexture(GL_TEXTURE_2D, resm->useTexture(textures[3].path));


    glActiveTexture(GL_TEXTURE4);
    glUniform1i(glGetUniformLocation(shader, "skybox"), 4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, resm->useCubeMap(resm->mCubemapIndex.at(resm->NearestCubeMap(pos))));

    /// Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}
