#include <Scene.h>

Scene::Scene(string sceneName)
{
    m_SceneName = sceneName;

    m_ProgramData = (std::unique_ptr<ProgramData>(new ProgramData()));
}

Scene::~Scene()
{
    std::cout << "Scene Destroyed" << std::endl;
}

bool Scene::setupScene()
{
    m_PostProcess = std::move((unique_ptr<PostProcess>)(new PostProcess()));

    map_ShadowMaps[0] =  std::move((unique_ptr<ShadowMap>)(new ShadowMap(m_ProgramData->SHADOWMAP_SIZE, m_ProgramData->SHADOWMAP_SIZE, -20.0f, 80.0f)));

    LoadGeometry();

    LoadEntities();

    return true;
}

void Scene::CalculateVisibility()
{/*
    for(std::map<int, std::shared_ptr<EntityTemplate>>::iterator itr = this->map_Entities.begin(); itr != this->map_Entities.end(); itr++)
    {
        if(!map_Entities[itr->first]->mHasModel)
            continue;

        glm::mat4 Model = glm::mat4(1.0);
        glm::mat4 ScaleMatrix = glm::scale(glm::mat4(1.0), m_ResourceManager->getModelScale(map_Entities[itr->first]->modelPath));
        glm::mat4 TranslationMatrix = glm::translate(glm::mat4(1.0), m_ResourceManager->getModelPosition(map_Entities[itr->first]->modelPath));
        glm::mat4 RotationMatrix = glm::toMat4(glm::normalize(m_ResourceManager->getModelRotation(map_Entities[itr->first]->modelPath)));

        Model = TranslationMatrix * ScaleMatrix * RotationMatrix;

        m_Frustum->CalculateFrustum(glm::mat4(m_ResourceManager->m_Camera->getProjectionMatrix() * m_ResourceManager->m_Camera->getViewMatrix()), Model);

        if(m_Frustum->BoxInFrustum(m_ResourceManager->getModelBoundingBox(map_Entities[itr->first]->modelPath)))
        {
            m_ResourceManager->setModelVisibility(map_Entities[itr->first]->modelPath, true);
        }
    }*/
}

bool Scene::LoadEntities()
{
   // m_ResourceManager->m_Camera = ((shared_ptr<Camera>)(new Camera(glm::vec3(20.0, 11.0, 2.0), glm::vec3(0.0f,0.0f,0.0f))));

    return true;
}

bool Scene::LoadGeometry()
{
    return true;
}

void Scene::Update(float deltatime, GLFWwindow* window)
{
    //m_ResourceManager->m_Camera->Update(window);
   // m_ResourceManager->m_Camera->UpdateMatrices();

    CalculateVisibility();

    Render();
}

void Scene::Render()
{
    for(std::map<int, std::shared_ptr<EntityTemplate>>::iterator itr = map_Entities.begin(); itr != map_Entities.end(); itr++)
    {
        itr->second->Update();
    }

    glCullFace(GL_FRONT);
    glm::mat4 BSPmodel = glm::mat4(1.0);
    glm::mat4 sModel = glm::scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1));
    BSPmodel = sModel;
    //m_BSPMap->Frustum.CalculateFrustum(glm::mat4(m_ResourceManager->m_Camera->getProjectionMatrix() * m_ResourceManager->m_Camera->getViewMatrix()), BSPmodel);
    //m_ResourceManager->useShader("Main");
    //m_BSPMap->RenderLevel(m_ResourceManager->m_Camera->getPosition(), m_ResourceManager->getShaderID("Main"), true);
}
