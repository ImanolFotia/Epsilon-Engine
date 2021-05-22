#pragma once

#include <Driver/API/OpenGL/DrawCommand.hpp>

#include <IO/IO.hpp>

#include <Driver/API/Context.hpp>
#include <Engine.hpp>

namespace Epsilon::Renderer
{
    template<typename Type>
    class DrawCommand
    {
        using DrawCommand_ptr = std::shared_ptr<API::DrawCommand>;

        DrawCommand(uint32_t vc, uint32_t ic, uint32_t fi, uint32_t bv, uint32_t bi = 0)
        {
            using API::CONTEXT_TYPE;
            auto type = Engine::Get().ContextType();
            switch (type)
            {
            case CONTEXT_TYPE::OGL:
                mCommand = std::make_shared<API::OpenGL::DrawCommand<Type>>(vc, ic, fi, bv, bi);
                break;

            default:
                IO::PrintLine("Context type: ", type, " is not yet implemented...");
                break;
            }
        }

        [[nodiscard]] const auto & data() {
            return std::any_cast<Type>mCommand->data();
        }

        DrawCommand_ptr mCommand;
    };
}