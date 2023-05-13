
#include "context.hpp"

template <>
engine::Context singleton<engine::Context>::self{};
namespace engine
{

	void Context::Init(const std::string& name, renderer_type rtype)
	{
		self.m_pRendererType = rtype;
#if !defined(ANDROID) && !defined(__ANDROID__)
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

		switch (m_pAudioType) {
		case audio_type::openal:
			self.m_pAudioManager = std::make_shared<engine::audio::ALAudioManager>();
			self.m_pAudioManager->Init();
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

	const std::string& Context::ApplicationName() { return self.m_pApplicationName; }

	framework::Window& Context::Window() { return self.m_pWindow; }

	std::shared_ptr<engine::ResourceManager> Context::ResourceManager() { return self.m_pResourceManager; }

	std::shared_ptr<engine::Renderer> Context::Renderer() { return self.m_pRenderer; }

	std::shared_ptr<engine::audio::AudioManager> Context::AudioManager() { return self.m_pAudioManager; }
}