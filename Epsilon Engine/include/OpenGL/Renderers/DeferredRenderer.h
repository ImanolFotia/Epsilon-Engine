#pragma once

#include <OpenGL/Renderers/renderer.h>
#include <Light.h>

#include <vector>

class DeferredRenderer : public Renderer()
{
public:
    DeferredRenderer() = default;
    virtual ~DeferredRenderer(){}

    virtual void Start(){}

    virtual void End(){}

private:

    std::vector<Light> m_Ligths;

};

