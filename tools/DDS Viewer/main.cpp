#define GLEW_STATIC
#define GLFW_STATIC
#include <iostream>
#include <vector>
#include <Core.h>
#include <GLFW/glfw3.h>
#include "DDS.h"
#include "Shader.h"

using namespace std;


vector<GLuint> indices;
vector<GLfloat> quad;


int main(int argc, char* argv[])
{
    if(!glfwInit())
    {
        std::cout << "GLFW could not be initialized" << std::endl;
    }
    GLFWwindow * window = glfwCreateWindow(1280, 720, "DDS Loader", nullptr, nullptr);

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    glewInit();


    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(3);
    indices.push_back(1);
    indices.push_back(2);
    indices.push_back(3);

    quad.push_back(1.0);
    quad.push_back(1.0);
    quad.push_back(0.0);
    quad.push_back(0.0);
    quad.push_back(0.0);

    quad.push_back(1.0);
    quad.push_back(-1.0);
    quad.push_back(0.0);
    quad.push_back(0.0);
    quad.push_back(1.0);

    quad.push_back(-1.0);
    quad.push_back(-1.0);
    quad.push_back(0.0);
    quad.push_back(1.0);
    quad.push_back(1.0);

    quad.push_back(-1.0);
    quad.push_back(1.0);
    quad.push_back(0.0);
    quad.push_back(1.0);
    quad.push_back(0.0);


    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * quad.size(), &quad[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    Shader shader("vertex.glsl", "fragment.glsl");

    double bef, af;
    bef = glfwGetTime();

    string path;

    if(argc < 2)
        return 0;
    else
        path = std::string(argv[1]);

    DDS ddsfile(path);


    af = glfwGetTime();

    double total = af - bef;

    cout << "Time to load the image: " << total << " Seconds" <<  endl;

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);

    unsigned int blockSize = (ddsfile.InternalFormat() == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
    unsigned int offset = 0;
    int width = ddsfile.header()->dwWidth, height = ddsfile.header()->dwHeight;
    /* load the mipmaps */
    for (unsigned int level = 0; level < (unsigned)ddsfile.header()->dwMipMapCount && (width || height); ++level)
    {
        unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;

        glCompressedTexImage2D(GL_TEXTURE_2D, level, ddsfile.InternalFormat(), width, height,
                               0, size, ddsfile.Data().get() + offset);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        offset += size;
        width  /= 2;
        height /= 2;

        // Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
        if(width < 1) width = 1;
        if(height < 1) height = 1;

    }
    glBindTexture(GL_TEXTURE_2D, 0);

    glViewport(0,0,1280, 720);
    glClearColor(0.0, 0.5, 0.4, 1.0);
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(1,0,1,1);

        shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shader.ProgramID, "sampler"), 0);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    return 0;
}
