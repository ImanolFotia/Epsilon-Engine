#pragma once

#include <GL/glad.h>
#include <glm/glm.hpp>

#include <include/Shader.h>

#include <include/gizmo_geometry.hpp>

class Gizmo
{
public:
    Gizmo() = default;

    virtual void Setup() = 0;

    virtual void Render(Shader* shader) = 0;

    virtual void Destroy() = 0;

protected:
    GLuint VAO0 = 0, EBO0 = 0, VBO0 = 0;
    GLuint VAO1 = 0, EBO1 = 0, VBO1 = 0;
    GLuint VAO2 = 0, EBO2 = 0, VBO2 = 0;

private:
};

class GizmoTranslate : public Gizmo
{

public:
    GizmoTranslate() = default;

    void Setup()
    {

        for(auto &i: GizmoGeometry::gizmo_translate_0_index) {
            i -= 1;
        }
        for(auto &i: GizmoGeometry::gizmo_translate_1_index) {
            i -= 98;
        }
        for(auto &i: GizmoGeometry::gizmo_translate_2_index) {
            i -= 195;
        }

        glGenVertexArrays(1, &VAO0);
        glGenBuffers(1, &VBO0);
        glGenBuffers(1, &EBO0);

        glBindVertexArray(VAO0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO0);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * GizmoGeometry::gizmo_translate_0_vert.size(), &GizmoGeometry::gizmo_translate_0_vert[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (GLvoid *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO0);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * GizmoGeometry::gizmo_translate_0_index.size(), &GizmoGeometry::gizmo_translate_0_index[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        glGenVertexArrays(1, &VAO1);
        glGenBuffers(1, &VBO1);
        glGenBuffers(1, &EBO1);

        glBindVertexArray(VAO1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * GizmoGeometry::gizmo_translate_1_vert.size(), &GizmoGeometry::gizmo_translate_1_vert[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (GLvoid *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * GizmoGeometry::gizmo_translate_1_index.size(), &GizmoGeometry::gizmo_translate_1_index[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);


        glGenVertexArrays(1, &VAO2);
        glGenBuffers(1, &VBO2);
        glGenBuffers(1, &EBO2);

        glBindVertexArray(VAO2);

        glBindBuffer(GL_ARRAY_BUFFER, VBO2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * GizmoGeometry::gizmo_translate_2_vert.size(), &GizmoGeometry::gizmo_translate_2_vert[0], GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (GLvoid *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * GizmoGeometry::gizmo_translate_2_index.size(), &GizmoGeometry::gizmo_translate_2_index[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void Render(Shader* shader)
    {
        glBindVertexArray(VAO0);
        glUniform3f(glGetUniformLocation(shader->getProgramID(), "color"), 110.0f/255.0f, 230.0f/255.0f, 0);
        glDrawElements(GL_TRIANGLES, GizmoGeometry::gizmo_translate_0_index.size(), GL_UNSIGNED_INT, (void*)0 );
        glBindVertexArray(0);

        glBindVertexArray(VAO1);
        glUniform3f(glGetUniformLocation(shader->getProgramID(), "color"), 230.0f/255.0f, 50.0f/255.0f, 0);
        glDrawElements(GL_TRIANGLES, GizmoGeometry::gizmo_translate_1_index.size(), GL_UNSIGNED_INT, (void*)0 );
        glBindVertexArray(0);

        glBindVertexArray(VAO2);
        glUniform3f(glGetUniformLocation(shader->getProgramID(), "color"), 0, 138.0f/255.0f, 230.0f/255.0f);
        glDrawElements(GL_TRIANGLES, GizmoGeometry::gizmo_translate_2_index.size(), GL_UNSIGNED_INT, (void*)0 );
        glBindVertexArray(0);
    }

    void Destroy()
    {
        glDeleteVertexArrays(1, &VAO0);
        glDeleteBuffers(1, &VBO0);
        glDeleteBuffers(1, &EBO0);

        glDeleteVertexArrays(1, &VAO1);
        glDeleteBuffers(1, &VBO1);
        glDeleteBuffers(1, &EBO1);

        glDeleteVertexArrays(1, &VAO2);
        glDeleteBuffers(1, &VBO2);
        glDeleteBuffers(1, &EBO2);
    }

private:
};

class GizmoRotate : public Gizmo
{

public:
    GizmoRotate() = default;

    void Setup()
    {
    }

    void Render()
    {
    }

    void Destroy()
    {
        glDeleteVertexArrays(1, &VAO0);
        glDeleteBuffers(1, &VBO0);
        glDeleteBuffers(1, &EBO0);
    }

private:
};

class GizmoScale : public Gizmo
{

public:
    GizmoScale() = default;

    void Setup()
    {
    }

    void Render()
    {
    }

    void Destroy()
    {
    }

private:
};
