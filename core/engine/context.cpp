
#include "context.hpp"

namespace engine
{

	void Context::Init(const std::string &name, renderer_type rtype)
	{
		m_pRendererType = rtype;
#if !defined(ANDROID) && !defined(__ANDROID__)
#endif

		switch (m_pRendererType)
		{
		case renderer_type::vulkan:
			m_pResourceManager = std::make_shared<engine::VulkanResourceManager>();

			m_pRenderer = std::make_shared<engine::VulkanRenderer>();
			std::static_pointer_cast<engine::VulkanRenderer>(m_pRenderer)->setResourceManagerRef(std::static_pointer_cast<engine::VulkanResourceManager>(m_pResourceManager).get());
			break;
		default:

			throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
			break;
		}

		switch (m_pAudioType)
		{
		case audio_type::openal:
			m_pAudioManager = std::make_shared<engine::audio::ALAudioManager>();
			m_pAudioManager->Init();
			break;
		default:

			throw framework::NotImplemented(__FILE__, __PRETTY_FUNCTION__);
			break;
		}

		std::cout << "Initiated API context" << std::endl;
	}

	Context::~Context()
	{
	}

	const std::string &Context::ApplicationName() { return m_pApplicationName; }

	framework::Window &Context::Window() { return m_pWindow; }

	std::shared_ptr<engine::ResourceManager> Context::ResourceManager() { return m_pResourceManager; }

	std::shared_ptr<engine::Renderer> Context::Renderer() { return m_pRenderer; }

	std::shared_ptr<engine::audio::AudioManager> Context::AudioManager() { return m_pAudioManager; }
}