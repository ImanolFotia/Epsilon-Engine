#pragma once

namespace Epsilon
{
    namespace OpenGLHelpers
    {
        class Quad
        {
        public:
            Quad(float width, float height) : m_Width(width / 2.0), m_Height(height / 2.0)
            {
                setupVAO();
            }
            ~Quad() {}

            void setupVAO(void)
            {
                GLfloat quadVertices[20] =
                    {
                        // Positions         //Texture Coords
                        -m_Width, m_Height, 0.0f, 0.0f, 1.0f,
                        -m_Width, -m_Height, 0.0f, 0.0f, 0.0f,
                        m_Width, m_Height, 0.0f, 1.0f, 1.0f,
                        m_Width, -m_Height, 0.0f, 1.0f, 0.0f};
                // Setup plane VAO
                glGenVertexArrays(1, &quadVAO);
                glGenBuffers(1, &quadVBO);
                glBindVertexArray(quadVAO);
                glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 20, &quadVertices[0], GL_STATIC_DRAW);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)0);
                glEnableVertexAttribArray(1);
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
            }

            void Render()
            {
                glBindVertexArray(quadVAO);
                glCache::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                glBindVertexArray(0);
            }

        private:
            GLuint quadVAO = 0, quadVBO = 0;
            float m_Width, m_Height;
        };

    } // namespace OpenGLHelpers
} // namespace Epsilon
