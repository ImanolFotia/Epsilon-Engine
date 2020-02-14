#include <include/Mesh.h>


/// Render the mesh
void Mesh::Draw(GLuint shader)
{

    //std::cout << "Texture address in Mesh: " << &this->textures[0] << std::endl;
    //std::cout << this->textures[0]->getTextureId() <<std::endl;
    //std::cout << "Llega Mesh" << std::endl;
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(shader, "texture_diffuse"), 0);
    glBindTexture(GL_TEXTURE_2D, this->textures[0]->getTextureId());

    glActiveTexture(GL_TEXTURE1);
    glUniform1i(glGetUniformLocation(shader, "texture_specular"), 1);
    glBindTexture(GL_TEXTURE_2D, this->textures[1]->getTextureId());

    glActiveTexture(GL_TEXTURE2);
    glUniform1i(glGetUniformLocation(shader, "texture_normal"), 2);
    glBindTexture(GL_TEXTURE_2D, this->textures[2]->getTextureId());

    glActiveTexture(GL_TEXTURE3);
    glUniform1i(glGetUniformLocation(shader, "texture_height"), 3);
    glBindTexture(GL_TEXTURE_2D, this->textures[3]->getTextureId());

    /// Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 1);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, 2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, 3);

}
