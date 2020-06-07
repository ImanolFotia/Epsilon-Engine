#pragma once

#include <memory>

namespace Epsilon
{
    namespace Platform
    {
        class ContextBase
        {
        };

        using ContextBase_ptr = std::shared_ptr<ContextBase>;
    } // namespace Platform
} // namespace Epsilon