#pragma once
namespace Epsilon::API
{
    enum class CommandType
    {
        Single,
        Instanced,
        Indirect,
        MultiIndirect,
        Size
    };

    struct DrawCommandDataBase_t {
    };
}