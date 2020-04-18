#ifndef SCENE_H
#define SCENE_H


#include <ResourceManager.h>
#include <Shader.h>
#include <Entity/Entity.h>
#include <BSP.h>
#include <ShadowMapping.h>
#include <Sun.h>
#include <Skybox.h>
#include <PostProcess.h>
#include <Light.h>
#include <EpsilonMemory.h>
#include <camera.h>
#include <Frustum.h>
#include <memory>
#include <map>
#include <iostream>
#include <string>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>
class Scene
{
public:
    Scene(string sceneName);
    virtual ~Scene();

    bool setupScene();

    bool ReloadScene();

    void Update(float deltatime, GLFWwindow* window);

    void Render();

private:

    bool LoadGeometry();

    bool LoadEntities();

    bool LoadSounds();

    void CalculateVisibility();

    void GeometryPrePass();

    void DeferredRenderingPass();

    void ForwardRenderingPass();

    void ImageProcessing();

private:

    std::shared_ptr<ResourceManager> m_ResourceManager;

    std::map<int, std::shared_ptr<EntityTemplate>> map_Entities;
    std::map<int, std::shared_ptr<Light>> map_Lights;
    std::map<int, std::shared_ptr<ShadowMap>> map_ShadowMaps;

    std::unique_ptr<CQuake3BSP> m_BSPMap;
    std::unique_ptr<Sun> m_Sun;
    std::unique_ptr<Skybox> m_Skybox;
    std::unique_ptr<PostProcess> m_DeferredRenderer;
    std::unique_ptr<PostProcess> m_PostProcess;
    std::unique_ptr<ProgramData> m_ProgramData;
    std::unique_ptr<CFrustum> m_Frustum;

    std::string m_SceneName;
};

#endif // SCENE_H
