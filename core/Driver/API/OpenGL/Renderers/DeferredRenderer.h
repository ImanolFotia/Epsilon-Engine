#pragma once

#include <pch.hpp>
#include <OpenGL/Renderers/renderer.h>
#include <Light.h>

namespace Epsilon {
class DeferredRenderer : public Renderer
{
public:
    DeferredRenderer() = default;
    virtual ~DeferredRenderer(){}

    virtual void Start(){}

    virtual void End(){}

    virtual void Flush() {}

private:

    std::vector<Light> m_Ligths;

};
}

