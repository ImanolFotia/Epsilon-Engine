#pragma once

#include "DrawCommandsTypes.hpp"
#include "../DrawCommand.hpp"

#include "../DrawCommandType.hpp"

#include <glad/glad.h>

#include <memory>

namespace Epsilon::API::OpenGL
{
    template <CommandType Type>
    class DrawCommand : public Epsilon::API::DrawCommand
    {
    public:
        DrawCommand(GLuint vc, GLuint ic, GLuint fi, GLuint bv, GLuint bi) : mType(Type)
        {
            switch (Type)
            {
            case CommandType::Indirect:
                DrawCommandData = DrawCommandDataIndirect_t(vc, ic, fi, bv);
            case CommandType::MultiIndirect:
                DrawCommandData = MultiDrawCommandDataIndirect_t(vc, ic, fi, bv, bi);
            default:  
                DrawCommandData = {vc, ic, fi, bv};
            }
        }

        [[nodiscard]] const std::any &data() override
        {
            return DrawCommandData;
        }

    private:
        Type mType;

        DrawCommandData_t DrawCommandData;
    };
}