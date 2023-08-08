#include "imgui_setup.hpp"

#include "../resource_manager.hpp"


void ImGuiRenderer::setResourceManager(engine::VulkanResourceManager* c) {
	m_pResourceManagerRef = c;
}

uint32_t ImGuiRenderer::addTexture(const std::string& renderpass, int index)
{

	auto pass = m_pResourceManagerRef->renderPassPool.get(std::hash<std::string>{}(renderpass));
	auto texture = pass->renderPassChain.Textures.at(index);
	VkDescriptorSet DS = ImGui_ImplVulkan_AddTexture(texture.sampler, texture.imageView, texture.imageLayout);
	m_pImages[m_pImageCount] = std::make_shared<ImageInfo>();
	m_pImages[m_pImageCount]->id = DS;
	m_pImages[m_pImageCount]->index = index;
	m_pImages[m_pImageCount]->render_pass = renderpass;
	m_pImages[m_pImageCount]->size = { texture.info.width, texture.info.height };
	m_pImageCount++;
	return m_pImageCount-1;
}


uint32_t ImGuiRenderer::addTexture(engine::Ref<engine::Texture> texture_ref) {
	auto texture = m_pResourceManagerRef->texPool.get(texture_ref);
	texture->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkDescriptorSet DS = ImGui_ImplVulkan_AddTexture(texture->sampler, texture->imageView, texture->imageLayout);
	m_pImages[m_pImageCount] = std::make_shared<ImageInfo>();
	m_pImages[m_pImageCount]->id = DS;
	m_pImages[m_pImageCount]->index = texture->index;
	m_pImages[m_pImageCount]->size = { texture->info.width, texture->info.height };
	m_pImages[m_pImageCount]->render_pass = "GlobalRenderPass";
	m_pImageCount++;
	return m_pImageCount - 1;
}

void ImGuiRenderer::recreateDescriptorSets() {
	for (auto& [index, image] : m_pImages) {
		if (image->render_pass == "GlobalRenderPass") continue;
		ImGui_ImplVulkan_RemoveTexture(image->id);

		auto pass = m_pResourceManagerRef->renderPassPool.get(std::hash<std::string>{}(image->render_pass));
		auto texture = pass->renderPassChain.Textures.at(image->index);
		VkDescriptorSet DS = ImGui_ImplVulkan_AddTexture(texture.sampler, texture.imageView, texture.imageLayout);
		m_pImages[index]->id = DS;
		m_pImages[index]->render_pass = image->render_pass;
	}
}