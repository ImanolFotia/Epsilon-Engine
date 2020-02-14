#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <include/Types.h>
#include <GL/glad.h>

class BoundingBox
{
public:
    BoundingBox(){}
    ~BoundingBox(){}

public:
    void Render()
    {
            if(first) {
                float lines[] = {0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
                                 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
                                 0.0, 0.0, 1.0, 1.0, 0.0, 1.0,
                                 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
                                 0.0, 1.0, 0.0, 1.0, 1.0, 0.0,
                                 0.0, 1.0, 0.0, 0.0, 1.0, 1.0,
                                 0.0, 1.0, 1.0, 1.0, 1.0, 1.0,
                                 1.0, 1.0, 0.0, 1.0, 1.0, 1.0,
                                 0.0, 0.0, 0.0, 0.0, 1.0, 0.0,
                                 1.0, 0.0, 0.0, 1.0, 1.0, 0.0,
                                 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,
                                 1.0, 0.0, 1.0, 1.0, 1.0, 1.0
                                };

                glGenVertexArrays(1, &cVAO);
                glGenBuffers(1, &cVBO);
                glBindVertexArray(cVAO);
                glBindBuffer(GL_ARRAY_BUFFER, cVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 72, &lines, GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (GLvoid*)0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
                first = false;
            }

            glBindVertexArray(cVAO);
            glDrawArrays(GL_LINES, 0, 24);
            glBindVertexArray(0);
        }

private:

    bool first = true;
    GLuint cVAO, cVBO;

};

#endif // BOUNDINGBOX_H
