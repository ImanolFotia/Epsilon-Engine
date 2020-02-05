#pragma once

#include <GL/glad.h>

#include <vector>
#include <glm/glm.hpp>

class defaultGrid
{
public:
    defaultGrid(){
        if(firstTime)
        {
            struct Line {
                glm::vec3 a;
                glm::vec3 b;
            };

            std::vector<Line> lines;
            glm::vec3 inita = glm::vec3(-50 * 3.2f, 0, -50 * 3.2f);
            glm::vec3 initb = glm::vec3(-50 * 3.2f, 0, -50 * 3.2f);
            for(int i = 0; i <= 200; i++) {
                Line tmpLine;
                tmpLine.a = inita;
                tmpLine.b = glm::vec3(inita.x, inita.y, 50 * 3.2f);
                lines.push_back(tmpLine);

                tmpLine.a = initb;
                tmpLine.b = glm::vec3(50 * 3.2f, initb.y, initb.z);
                lines.push_back(tmpLine);
                initb.z += 3.2f;
                inita.x += 3.2f;
            }
            numLines = lines.size();
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);

            glBufferData(GL_ARRAY_BUFFER, sizeof(Line) * lines.size(), &lines[0], GL_STATIC_DRAW);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (GLvoid*)0);
            glEnableVertexAttribArray(0);
            glBindVertexArray(0);

        }
    }

    ~defaultGrid(){}

    void Render(){
        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, numLines);
        glBindVertexArray(0);
    }

private:
    GLuint VAO, VBO;
    bool firstTime = true;
    int numLines = 0;
protected:

};
