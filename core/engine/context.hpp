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
        Context()
        {
        }

        void Init(const std::string &name, renderer_type rtype)
        {
            self.m_pRendererType = rtype;
#if !defined(ANDROID) && !defined(__ANDROID__)
            bool res = al::initDevice(&alData);

            if (!res)
                std::cout << "Couldn't start audio device" << std::endl;
#endif

            switch (self.m_pRendererType)
            {
            case renderer_type::vulkan:
                self.m_pResourceManager = std::make_shared<engine::VulkanResourceManager>();

                self.m_pRenderer = std::make_shared<engine::VulkanRenderer>();
                std::static_pointer_cast<engine::VulkanRenderer>(self.m_pRenderer)->setResourceManagerRef(std::static_pointer_cast<engine::VulkanResourceManager>(self.m_pResourceManager).get());
                break;
            default:

                throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
                break;
            }

            std::cout << "Initiated API context" << std::endl;
        }

        const std::string &ApplicationName() { return self.m_pApplicationName; }

        framework::Window &Window() { return self.m_pWindow; }

        auto ResourceManager() { return self.m_pResourceManager; }

        auto Renderer() { return self.m_pRenderer; }

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
