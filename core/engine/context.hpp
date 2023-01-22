#pragma once

#include "core/framework/window.hpp"
#include "core/framework/exception.hpp"
#include "core/engine/renderer/resource_manager.hpp"
#include "core/engine/renderer/renderer.hpp"
#include "core/engine/renderer/vulkan/resource_manager.hpp"
#include "../../apis/al/al.hpp"
#include "apis/al/al.hpp"

#ifdef WIN32
#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#endif

namespace engine {
    class Context {
    public:
        Context() {
        }

        void Init(const std::string &name, renderer_type rtype) {
            m_pRendererType = rtype;

            bool res = al::initDevice(&alData);

            if(!res) std::cout << "Couldn't start audio device" << std::endl;

            switch (m_pRendererType) {
                case renderer_type::vulkan:
                    m_pResourceManager = std::make_shared<engine::VulkanResourceManager>();

                    m_pRenderer = std::make_shared<engine::VulkanRenderer>();
                    std::static_pointer_cast<engine::VulkanRenderer>(m_pRenderer)->setResourceManagerRef(
                            std::static_pointer_cast<engine::VulkanResourceManager>(m_pResourceManager).get());
                    break;
                default:


                    throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
                    break;
            }


            std::cout << "Initiated API context" << std::endl;
        }

        const std::string &ApplicationName() { return m_pApplicationName; }

        framework::Window &Window() { return m_pWindow; }

        auto ResourceManager() { return m_pResourceManager; }

        auto Renderer() { return m_pRenderer; }

    protected:
        std::string m_pApplicationName = "Default";
        renderer_type m_pRendererType;
        OpenALData alData;

        framework::Window m_pWindow = {};
        std::shared_ptr<engine::ResourceManager> m_pResourceManager = nullptr;
        std::shared_ptr<engine::Renderer> m_pRenderer = nullptr;

    };
}
