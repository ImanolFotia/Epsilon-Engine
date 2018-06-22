#include <Patch.h>

Patch::Patch(glm::vec3 position, float s, int w, int h, std::string texturePath) {
    this->mScale = s;
    this->mWidth = w;
    this->mHeight = h;
    this->mPosition = position;

    this->mTexture = (std::shared_ptr<eTexture>) new eTexture(texturePath.c_str());
    this->mNormalTexture = (std::shared_ptr<eTexture>) new eTexture("sponza/sponza_curtain_ddn.tga");

    this->generateGeometry();
    this->setupVertexBuffers();
}

void Patch::Destroy() {
    glDeleteBuffers(1, &this->VBO);
    glDeleteBuffers(1, &this->NBO);
    glDeleteBuffers(1, &this->UVBO);
    glDeleteBuffers(1, &this->EBO);
    glDeleteVertexArrays(1, &this->VAO);

    this->mTexture->Destroy();
}

void Patch::Render(Shader* inShader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {

    glDisable(GL_CULL_FACE);
    inShader->Use();
    glm::mat4 model = glm::mat4();//glm::translate(glm::mat4(), this->mPosition);
    glm::mat4 MVP = projectionMatrix * viewMatrix * model;
    glBindVertexArray(this->VAO);
    inShader->PushUniform("MVP", MVP);
    inShader->PushUniform("Model", model);
    inShader->PushUniform("view", viewMatrix);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->mTexture->getTextureID());
    inShader->PushUniform("texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->mNormalTexture->getTextureID());
    inShader->PushUniform("texture1", 1);

    glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);

    inShader->Free();
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
}

void Patch::RenderShadows()
{
    glDisable(GL_CULL_FACE);
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);
    glEnable(GL_CULL_FACE);
    glBindVertexArray(0);
}

void Patch::Render(Shader inShader, glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {

    inShader.Use();
    glm::mat4 model = glm::mat4();//glm::translate(glm::mat4(), this->mPosition);
    glm::mat4 MVP = projectionMatrix * viewMatrix * model;
    glBindVertexArray(this->VAO);
    inShader.PushUniform("MVP", MVP);
    inShader.PushUniform("Model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->mTexture->getTextureID());
    inShader.PushUniform("texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, this->mNormalTexture->getTextureID());
    inShader.PushUniform("texture1", 1);

    glDrawElements(GL_TRIANGLES, this->mIndices.size(), GL_UNSIGNED_INT, 0);

    inShader.Free();
    glBindVertexArray(0);
}

void Patch::updateVertexBuffers(std::vector<Physics::PhysicObject::t_ClothVertex> inVertices) {

    std::vector<glm::vec3> tmpVerts;
    std::vector<glm::vec3> tmpNormals;

    for(auto i = 0; i < inVertices.size(); i++) {
        glm::vec3 pos = glm::vec3(inVertices[i].position.x(), inVertices[i].position.y(), inVertices[i].position.z());
        glm::vec3 nor = glm::vec3(inVertices[i].normal.x(), inVertices[i].normal.y(), inVertices[i].normal.z());
        tmpVerts.push_back(pos);
        tmpNormals.push_back(nor);
    }

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) *  inVertices.size(), &tmpVerts[0]);
    glBindBuffer(GL_ARRAY_BUFFER, this->NBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) *  inVertices.size(), &tmpNormals[0]);

    glBindVertexArray(0);
}

void Patch::generateGeometry() {
    int numVertices = this->mWidth * this->mHeight;

    float x = 0;
    float y = 0;

    for(int i = 0; i< numVertices; ++i) {
            x = i % mWidth;
            y = glm::floor(i / mWidth);

            float s = x / (this->mWidth-1);
            float t = y / (this->mHeight-1);

            this->mVertices.push_back(glm::vec3(x, y, 0));
            this->mTexCoords.push_back(glm::vec2(s, t));
            this->mNormals.push_back(glm::vec3(0,0,1));
    }

    int counter = 0;
    for(int i = 0 ; i < (this->mWidth * this->mHeight) - mWidth ; i++) {
        if(counter == this->mWidth-1) {
            counter = 0;
            continue;
        }

        this->mIndices.push_back(i+1);
        this->mIndices.push_back(i+this->mWidth);
        this->mIndices.push_back(i+this->mWidth - this->mWidth);
        this->mIndices.push_back(i + 1);
        this->mIndices.push_back(i+this->mWidth+1);
        this->mIndices.push_back(i+this->mWidth);

        counter++;
    }

    std::cout << "Created new patch" << std::endl;
    std::cout << "\tVertices: " << this->mVertices.size() << std::endl;
    std::cout << "\tIndices: " << this->mIndices.size() << std::endl;
}

void Patch::setupVertexBuffers() {
    /// Create buffers/arrays
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->NBO);
    glGenBuffers(1, &this->UVBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    /// Load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->mVertices.size() * sizeof(glm::vec3), &this->mVertices[0], GL_STREAM_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, this->UVBO);
    glBufferData(GL_ARRAY_BUFFER, this->mTexCoords.size() * sizeof(glm::vec2), &this->mTexCoords[0], GL_STREAM_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (GLvoid*)0);

    glBindBuffer(GL_ARRAY_BUFFER, this->NBO);
    glBufferData(GL_ARRAY_BUFFER, this->mNormals.size() * sizeof(glm::vec3), &this->mNormals[0], GL_STREAM_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->mIndices.size() * sizeof(unsigned int), &this->mIndices[0], GL_STREAM_DRAW);

    glBindVertexArray(0);
}
