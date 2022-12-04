#pragma once

#include "../framework/window.hpp"
#include "../framework/exception.hpp"
#include "renderer/resource_manager.hpp"
#include "renderer/renderer.hpp"
#include "renderer/vulkan/resource_manager.hpp"

namespace engine
{
    class Context
    {
    public:
        Context() {
        }

        void Init(const std::string& name, renderer_type rtype) {
            m_pApplicationName = name; 
            m_pRendererType = rtype;

            switch (m_pRendererType)
            {
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

        const std::string& ApplicationName() { return m_pApplicationName;}
        framework::Window& Window() { return m_pWindow; }
        auto ResourceManager() { return m_pResourceManager; }
        auto Renderer() { return m_pRenderer; }

    protected:
        std::string m_pApplicationName = "Default";
        renderer_type m_pRendererType;

        framework::Window m_pWindow = {};
        std::shared_ptr<engine::ResourceManager> m_pResourceManager = nullptr;
        std::shared_ptr<engine::Renderer> m_pRenderer = nullptr;

    };
}
