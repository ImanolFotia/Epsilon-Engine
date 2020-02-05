#include <include/terrain.h>
#include <iostream>
#include <thread>
#include <include/helpers.h>
Terrain::Terrain(int size, double roughness, double height,glm::vec3 pos)
{
    m_Size = size;
    m_Roughness = roughness;
    m_Height = height;
    this->m_Position = pos;

   // th = std::thread([&](){
        this->genVertexData();
        generated = true;
        if(generated){
            //th.join();
            this->genVertexArrayObject();
            generated = false;
        }
   // });
        texture = new OpenGL::Texture();
        texture->Load("assets/textures/checkers.png");
    std::cout << "Done" << std::endl;
}

void Terrain::genVertexData()
{
    TVertex vert;

    std::cout << "get vertex data" << std::endl;

        float texU = float(m_Size);
        float texV = float(m_Size);
        int counter = 0;

        std::vector< std::vector<float> > Grid;
        std::vector<float> row;
try{
        for(int i = 0 ; i < this->m_Size ; i++)
        {
            for(int j = 0 ; j < this->m_Size ; j++)
            {
                vert.Position.x = (float)i + m_Position.x;
                vert.Position.y = (float)glm::pow(fbm(glm::vec2(i, j)*(float)m_Roughness), 4.0) * (float)m_Height + m_Position.y;

                //vert.Position.y = (float)glm::pow(fbm(glm::vec2(i, j) * (float)m_Roughness), (float)m_Height) + m_Position.y;
                vert.Position.z = (float)j + m_Position.z;
                row.push_back(vert.Position.y);
                float fScaleC = float(j)/float(this->m_Size-1);
                float fScaleR = float(i)/float(this->m_Size-1);
                vert.TexCoords.s = this->m_Size*fScaleC/this->m_Size;
                vert.TexCoords.t = this->m_Size*fScaleR/this->m_Size;
                m_Vertices.push_back(vert);
                counter++;
            }
            Grid.push_back(row);
            row.clear();
        }
        }
        catch(std::exception e)
        {
            std::cout << "Runtime error detected while calculating terrain positions." << std::endl << "Exception: " << e.what() << std::endl;
        }

        //cout << Grid.size() << endl;
        //cout << Grid.at(0).size() << endl;

        int mod = 0;
        try{
        counter = 0;
        for(int i = 0 ; i < this->m_Size ; i++)
        {
            for(int j = 0 ; j < this->m_Size ; j++)
            {
                if(i > 0 && j > 0 && i < Grid.size()-1 && j < Grid.size()-1)
                {
                    float HL = Grid.at(i-1).at(j);
                    float HR = Grid.at(i+1).at(j);
                    float HD = Grid.at(i).at(j-1);
                    float HU = Grid.at(i).at(j+1);

                    glm::vec3 norm = glm::vec3(HL - HR, 2.0f, HD - HU);

                    m_Normals.push_back(norm);
                }
                else
                {
                    m_Normals.push_back(glm::vec3(0,1,0));
                }
            //vert.Color = m_Normals;

            }
        }

        for(auto i = 0; i < m_Vertices.size(); ++i)
        {
            m_Vertices.at(i).Normal = m_Normals.at(i);
        }

        }
        catch(std::exception e)
        {
            std::cout << "Runtime error detected while calculating terrain normals." << std::endl << "Exception: " << e.what() << std::endl;
        }

        counter= 0;

        try{
        for(int i = 0 ; i < (this->m_Size * this->m_Size)-this->m_Size ; i++)
        {
            if(counter == this->m_Size-1)
            {
                counter = 0;
                mod++;
                continue;
            }
            Face face;

            face.vertex0 = m_Vertices[i+1].Position;
            m_Indices.push_back(i+1);
            face.vertex1 = m_Vertices[i+this->m_Size].Position;
            m_Indices.push_back(i+this->m_Size);
            face.vertex2 = m_Vertices[i+this->m_Size - this->m_Size].Position;
            m_Indices.push_back(i+this->m_Size - this->m_Size);

            m_Faces.push_back(face);

            face.vertex0 = m_Vertices[i + 1].Position;
            m_Indices.push_back(i + 1);
            face.vertex1 = m_Vertices[i+this->m_Size+1].Position;
            m_Indices.push_back(i+this->m_Size+1);
            face.vertex2 = m_Vertices[i+this->m_Size].Position;
            m_Indices.push_back(i+this->m_Size);

            m_Faces.push_back(face);

            counter++;

        }

        }
        catch(std::exception e)
        {
            std::cout << "Runtime error detected while calculating terrain indices." << std::endl << "Exception: " << e.what() << std::endl;
        }


}

void Terrain::genVertexArrayObject()
{
    glGenVertexArrays(1, &this->m_VAO);
        glGenBuffers(1, &this->m_VBO);
        glGenBuffers(1, &this->m_EBO);

        glBindVertexArray(this->m_VAO);

        /** Vertex And UV buffer Attachments*/

        glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO);
        glBufferData(GL_ARRAY_BUFFER, this->m_Vertices.size() * sizeof(TVertex), &this->m_Vertices[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid*)offsetof(TVertex, TexCoords));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (GLvoid*)offsetof(TVertex, Normal));

        /** Element buffer Attachments*/

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_Indices.size() * sizeof(GLuint), &this->m_Indices[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
}

void Terrain::Render(GLuint shader)
{
    if(generated){
        th.join();
        this->genVertexArrayObject();
        generated = false;
    }

    glBindVertexArray(this->m_VAO);
    //glUseProgram(shader);

    glActiveTexture(GL_TEXTURE0);
    if(texture->getTextureId() != 0)
        glBindTexture(GL_TEXTURE_2D, texture->getTextureId());
    else
        glBindTexture(GL_TEXTURE_2D, 0);
    glUniform1i(glGetUniformLocation(shader, "texture_diffuse"), 0);

    glUniform1i(glGetUniformLocation(shader, "repeat"), this->textureRepeat);

    glDrawElements(GL_TRIANGLES, this->m_Indices.size(), GL_UNSIGNED_INT, 0);

    //glUseProgram(0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void Terrain::Destroy()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    m_Vertices.clear();
    std::vector<TVertex>().swap(m_Vertices);
    m_Faces.clear();
    std::vector<Face>().swap(m_Faces);
    m_Indices.clear();
    std::vector<unsigned int>().swap(m_Indices);
    m_Normals.clear();
    std::vector<glm::vec3>().swap(m_Normals);
    m_VAO = 0;
    m_VBO = 0;
}
