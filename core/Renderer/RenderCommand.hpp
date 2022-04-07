#pragma once

#include "pch.hpp"

#include <Driver/API/OpenGL/DrawCommandsTypes.hpp>

namespace Epsilon::Renderer
{
    enum RenderCommandType
    {
        OPAQUE = 0,
        TRANSPARENT,
        PARTICLE,
        HAIR,
        NOT_SET
    };

    struct RenderCommand
    {   
        using data_type = Epsilon::API::DrawCommandDataBase_t;
        using gl_data_type = Epsilon::API::OpenGL::MultiDrawCommandDataIndirect_t;
        using pointer_data_type = std::shared_ptr<data_type>;

        Type m_Type = NOT_SET;
        int m_ObjectId = -1;
        int m_MaterialId = -1;
        int m_TransformId = -1;

        RenderCommand() = default;

        RenderCommand(Type t, unsigned int ObjectId, unsigned int MaterialId, unsigned int TransformId)
         : m_Type(t), m_ObjectId(ObjectId), m_MaterialId(m_MaterialId), m_TransformId(TransformId)
        {

            using API::CONTEXT_TYPE;
            auto type = Engine::Get().ContextType(); // CONTEXT_TYPE::OGL;//mContext->getType();
            switch (type)
            {
            case CONTEXT_TYPE::OGL:
                m_CommandData = std::make_shared<gl_data_type>();
                break;

            default:
                IO::PrintLine("Context type: ", type, " is not yet implemented...");
                break;
            }
        }

        pointer_data_type m_CommandData;
    };

    using RenderCommand_ptr = std::shared_ptr<RenderCommand>;
} // namespace Epsilon