#ifndef SKYBOX_H
#define SKYBOX_H

#include <include/Model.h>
#include <include/Shader.h>

class Skybox
{
public:
    Skybox(){
        mModel = (std::shared_ptr<Model>) new Model("sphere.eml");
        std::cout << "Llega" << std::endl;
        mShader = (std::shared_ptr<Shader>) new Shader("assets/shaders/skybox.vglsl", "assets/shaders/skybox.fglsl");

        for(auto & m : mModel->meshes)
        {
            for(auto & t : m.textures)
            {
                t->getTextureId();
                t->Destroy();
            }
            //m.setupMesh();
        }
    }

    ~Skybox(){
        mModel->Destroy();
    }

    void Render(glm::mat4 camView, glm::mat4 camProjection, float time){
            glDepthFunc(GL_LEQUAL);
            mShader->Use();

            glm::mat4 view = glm::mat4(glm::mat3(camView));
            glm::mat4 projection = camProjection;
            glm::mat4 model = glm::mat4();

            glm::mat4 ScaleMatrix = glm::scale(model, glm::vec3(1,1,1));
            glm::mat4 TranslationMatrix = glm::translate(model, glm::vec3(0,0,0));
            model = ScaleMatrix * TranslationMatrix;
            glUniform1f(glGetUniformLocation(mShader->getProgramID(),"time"), time);
            glUniformMatrix4fv(glGetUniformLocation(mShader->getProgramID(), "model"), 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(mShader->getProgramID(), "view"), 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(glGetUniformLocation(mShader->getProgramID(), "projection"), 1, GL_FALSE, &projection[0][0]);
            mModel->DrawNoTexture();
            glUseProgram(0);
            glDepthFunc(GL_LESS);
    }

private:
    std::shared_ptr<Model> mModel;
    std::shared_ptr<Shader> mShader;
};

#endif // SKYBOX_H
