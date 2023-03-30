#pragma once

#include "../framework/window.hpp"
#include "../framework/exception.hpp"
#include "renderer/resource_manager.hpp"
#include "renderer/renderer.hpp"
#include "renderer/vulkan/resource_manager.hpp"
#include "../../apis/al/al.hpp"
#include "apis/al/al.hpp"
#include "core/framework/singleton.hpp"

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

namespace engine
{
    class Context : public singleton<Context>
    {
    public:
        Context() = default;

        void Init(const std::string &name, renderer_type rtype);

        ~Context();

        const std::string &ApplicationName();

        framework::Window &Window();

        std::shared_ptr<engine::ResourceManager> ResourceManager();

        std::shared_ptr<engine::Renderer> Renderer();

    protected:
        std::string m_pApplicationName = "Default";
        renderer_type m_pRendererType;
#if defined(ANDROID) || defined(__ANDROID__)
        struct OpenALData
        {
        };
#endif
        OpenALData alData;

        framework::Window m_pWindow = {};
        std::shared_ptr<engine::ResourceManager> m_pResourceManager = nullptr;
        std::shared_ptr<engine::Renderer> m_pRenderer = nullptr;
    };
}
