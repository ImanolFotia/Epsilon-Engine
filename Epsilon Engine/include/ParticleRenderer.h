#pragma once

#include <Particle.h>
#include <Shader.h>

class ParticleRenderer
{
public:
    ParticleRenderer(unsigned int ID, unsigned int numParticles, std::vector<glm::mat4> initPos) :
        m_Positions(initPos), System_ID(ID), m_NumParticles(numParticles) {
        GenerateParticleGeometry();

    }
    ~ParticleRenderer() {}

    void Render() {
        glDisable(GL_CULL_FACE);
        glBindVertexArray(this->VAO);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_NumParticles);
        glBindVertexArray(0);
        glEnable(GL_CULL_FACE);
    }

    void UpdateParticleRenderer(std::vector<glm::mat4> newPositions) {
        glBindBuffer(GL_ARRAY_BUFFER, PositionBuffer);
		glBufferData(GL_ARRAY_BUFFER, newPositions.size() * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, newPositions.size() * sizeof(glm::mat4), &newPositions[0]);
    }

private:

    void GenerateParticleGeometry() {
        VAO = EBO = VBO = 0;
        PositionBuffer = 0;
        glGenVertexArrays(1, &VAO);

        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(ParticleVertices), ParticleVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ParticlesIndices), ParticlesIndices, GL_STATIC_DRAW);


        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
/*
        glm::mat4* modelMatrices = new glm::mat4[m_Positions.size()];
        glm::mat4 model;
        for(int i = 0; i < m_Positions.size(); ++i)
        {
            model = glm::mat4(1);
            model = glm::translate(glm::mat4(1), m_Positions[i]);
            model = glm::scale(model, glm::vec3(7.5f));
            modelMatrices[i] = model;
        }
*/
        glGenBuffers(1, &PositionBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, PositionBuffer);
        glBufferData(GL_ARRAY_BUFFER, m_Positions.size() * sizeof(glm::mat4), &m_Positions[0], GL_STREAM_DRAW);
        // Set attribute pointers for matrix (4 times vec4)
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(sizeof(glm::vec4)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(2 * sizeof(glm::vec4)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (GLvoid*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

private:
    std::vector<glm::mat4> m_Positions;
    unsigned int m_NumParticles;
    unsigned int System_ID;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint PositionBuffer;

    GLfloat ParticleVertices[32] =
    {
        0.25f,  0.25f,  0.0f, 1.0f, 1.0f, 0.0, 1.0, 0.0,
        0.25f, -0.25f,  0.0f, 1.0f, 0.0f, 0.0, 1.0, 0.0,
        -0.25f, -0.25f,  0.0f, 0.0f, 0.0f, 0.0, 1.0, 0.0,
        -0.25f,  0.25f,  0.0f, 0.0f, 1.0f, 0.0, 1.0, 0.0,
    };

    GLuint ParticlesIndices[6] =
    {
        0, 1, 3,
        1, 2, 3,
    };

};
