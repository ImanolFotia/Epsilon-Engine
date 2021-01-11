#pragma once

#include <Driver/API/OpenGL/ShaderStorageBuffer.h>
#include <Driver/API/ShaderStorage.hpp>

#include <pch.hpp>
#include <Engine.hpp>
#include <memory>

namespace Epsilon
{
    namespace Renderer
    {
        class ShaderStorage
        {
        public:
            ShaderStorage(size_t size, unsigned int binding_point)
            {
                using API::CONTEXT_TYPE;
                auto type = Engine::Get().ContextType();
                switch (type)
                {
                case CONTEXT_TYPE::OGL:
                    mShaderStorageObject = std::make_shared<API::OpenGL::ShaderStorageBuffer>(size, binding_point);
                    break;

                default:
                    IO::PrintLine("Context type: ", type, " is not yet implemented...");
                    break;
                }
            }

            void Init(const void *data)
            {
                mShaderStorageObject->Init(data);
            }
            void Update(size_t size, unsigned int offset, const void *data)
            {
                mShaderStorageObject->Update(size, offset, data);
            }

            void Get(void *data)
            {
                mShaderStorageObject->Get(data);
            }

            void GetSubData(size_t size, unsigned int offset, void *data)
            {
                mShaderStorageObject->GetSubData(size, offset, data);
            }

            void Bind()
            {
                mShaderStorageObject->Bind();
            }
            void Unbind()
            {
                mShaderStorageObject->Unbind();
            }

            std::shared_ptr<API::ShaderStorage> mShaderStorageObject;
        };
    } // namespace Renderer
} // namespace Epsilon