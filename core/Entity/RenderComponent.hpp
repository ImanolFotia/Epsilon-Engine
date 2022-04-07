#pragma once

#include <Entity/Component.h>

#include <Renderer/Model.h>
#include <Renderer/Primitives/Sphere.hpp>
namespace Epsilon
{
    namespace Component
    {
        /// Rendering Components
        class RenderComponent : public Component
        {
        public:
            RenderComponent(std::string path, glm::vec3 pos, std::string shader)
                : modelPath(path) /*, mPosition(pos),*/, shaderType(shader)
            {
                mType = RENDERCOMPONENT;
                hasModel = true;
                /*mPrevPosition = pos;*/
                ResourceManager::Get().addModelToQueue(modelPath /*, mPosition, glm::vec3(1), glm::quat(0, 0, 0, 0)*/);
            }

            RenderComponent(std::shared_ptr<Renderer::Drawable> drawable, std::string shader) : shaderType(shader)
            {
                mDrawable = drawable;
                hasModel = false;
            }

            COMPONENT_TYPE getType() { return mType; }
            ~RenderComponent()
            {
                std::cout << "RenderComponent Destructor" << std::endl;
            }

            void setRenderId(int id)
            {
                mId = id;
            }
            const std::string &getModelPath()
            {
                return modelPath;
            }

            void Destroy()
            {
            }

        public:
            void Update()
            {
            }

            void Render()
            {
                if (isVisible)
                {
                    // glUniform1i(glGetUniformLocation(mResourceManager->getShader(shaderType).get(), "isTransparent"), this->isTransparent);
                    if (isDoubleFaced())
                        glDisable(GL_CULL_FACE);
                    else
                        glEnable(GL_CULL_FACE);

                    auto tShader = ResourceManager::Get().useShader(shaderType);
                    

                    if (mPass == 0)
                    {
                        tShader->PushUniform("isTransparent", this->isTransparent);
                        tShader->PushUniform("EntityId", this->mId);
                    }
                    if (hasModel)
                    {
                        auto tModel = ResourceManager::Get().getModel(modelPath /*ResourceManager::Get().useShader(shaderType), mPosition*/);
                        tModel->Draw(tShader);
                    }
                    else
                    {

                        using Tex2D_ptr = std::shared_ptr<Renderer::Texture2D>;
                        auto SphereMaterial = std::static_pointer_cast<Renderer::Sphere>(mDrawable)->getMaterial();

                        auto albedo = SphereMaterial->get<Tex2D_ptr>(Renderer::MaterialBase::MaterialParameter::Albedo);
                        auto roughness = SphereMaterial->get<Tex2D_ptr>(Renderer::MaterialBase::MaterialParameter::Roughness);
                        auto metallic = SphereMaterial->get<Tex2D_ptr>(Renderer::MaterialBase::MaterialParameter::Metallic);
                        auto normal = SphereMaterial->get<Tex2D_ptr>(Renderer::MaterialBase::MaterialParameter::Normal);

                        if (albedo != nullptr && !SphereMaterial->usingAlbedoColor())
                        {
                            albedo->Bind(0);
                            tShader->PushUniform("texture_diffuse", 0);
                            tShader->PushUniform("using_color_diffuse", 0);
                        }
                        else
                        {
                            tShader->PushUniform("texture_diffuse", 0);
                            tShader->PushUniform("using_color_diffuse", 1);
                            tShader->PushUniform("color_diffuse", glm::vec4(SphereMaterial->get<glm::vec4>(Renderer::MaterialBase::MaterialParameter::Albedo)));
                        }
                        if (roughness != nullptr && !SphereMaterial->usingRoughnessColor())
                        {
                            roughness->Bind(1);
                            tShader->PushUniform("texture_specular", 1);
                            tShader->PushUniform("using_color_specular", 0);
                        }
                        else
                        {
                            tShader->PushUniform("texture_specular", 1);
                            tShader->PushUniform("using_color_specular", 1);
                            tShader->PushUniform("color_specular", glm::vec4(SphereMaterial->get<glm::vec4>(Renderer::MaterialBase::MaterialParameter::Roughness)));
                        }
                        if (normal != nullptr && !SphereMaterial->usingNormalColor())
                        {
                            normal->Bind(2);
                            tShader->PushUniform("texture_normal", 2);
                            tShader->PushUniform("using_color_normal", 0);
                        }
                        else
                        {
                            tShader->PushUniform("texture_normal", 2);
                            tShader->PushUniform("using_color_normal", 1);
                            tShader->PushUniform("color_normal", glm::vec4(SphereMaterial->get<glm::vec4>(Renderer::MaterialBase::MaterialParameter::Normal)));
                        }
                        if (metallic != nullptr && !SphereMaterial->usingMetallicColor())
                        {
                            metallic->Bind(3);
                            tShader->PushUniform("texture_height", 3);
                            tShader->PushUniform("using_color_height", 0);
                            tShader->PushUniform("color_height", glm::vec4(SphereMaterial->get<glm::vec4>(Renderer::MaterialBase::MaterialParameter::Metallic)));
                        }
                        else
                        {
                            tShader->PushUniform("texture_height", 3);
                            tShader->PushUniform("using_color_height", 1);
                            tShader->PushUniform("color_height", glm::vec4(SphereMaterial->get<glm::vec4>(Renderer::MaterialBase::MaterialParameter::Metallic)));
                        }
                        /*
                                                auto &Instance = ResourceManager::Get();

                                                unsigned CubemapId = 0;

                                                if (Instance.cubemapsLoaded)
                                                {
                                                    CubemapId = Instance.NearestCubeMap(mPosition);
                                                }
                                                std::shared_ptr<Epsilon::Renderer::TextureCube> cubemap;
                                                if (CubemapId >= 0)
                                                    cubemap = Instance.getCubemap(CubemapId);

                                                if (cubemap)
                                                {
                                                    cubemap->Bind(4);
                                                    tShader->PushUniform("skybox", 4);
                                                }*/

                        mDrawable->Render();

                        glActiveTexture(GL_TEXTURE0);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glActiveTexture(GL_TEXTURE2);
                        glBindTexture(GL_TEXTURE_2D, 0);
                        glActiveTexture(GL_TEXTURE3);
                        glBindTexture(GL_TEXTURE_2D, 0);

                        glEnable(GL_CULL_FACE);
                    }
                }
            }

            void RenderShadows()
            {
                if (hasModel)
                {
                    auto tModel = ResourceManager::Get().getModel(modelPath /*ResourceManager::Get().useShader(shaderType), mPosition*/);
                    auto tShader = ResourceManager::Get().useShader(shaderType);
                    tModel->Draw(tShader);
                }
                else
                {
                    mDrawable->Render();
                }
            }

            void setShader(std::string sh)
            {
                shaderType = sh;
            }

            bool isDoubleFaced()
            {
                return mIsDoubleFaced;
            }

            void isDoubleFaced(bool x)
            {
                mIsDoubleFaced = x;
            }

            void CastsShadows(bool x)
            {
                mCastsShadows = x;
            }

            bool CastsShadows()
            {
                return mCastsShadows;
            }

            void setTransparency(bool x) { isTransparent = x; }

            void setVisibility(bool x)
            {
                isVisible = x;
            }

            void setPass(int p)
            {
                mPass = p;
            }

            bool hasModel = false;
            COMPONENT_TYPE mType = RENDERCOMPONENT;
            std::string modelPath;
            std::string shaderType;
            MODEL_TYPE ModelType;
            std::shared_ptr<Renderer::Drawable> mDrawable;
            bool isTransparent = false;
            bool mIsDoubleFaced;
            bool mCastsShadows;
            bool isVisible = false;
            int mId = 0;
            int mPass = 0;
            /* glm::vec3 mPosition;
            glm::vec3 mPrevPosition;*/
        };

        using RenderComponent_ptr = std::shared_ptr<RenderComponent>;
    } // namespace Component
} // namespace Epsilon