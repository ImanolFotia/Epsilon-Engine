#pragma once
// Dear ImGui: standalone example application for Glfw + Vulkan
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important note to the reader who wish to integrate imgui_impl_vulkan.cpp/.h in their own engine/app.
// - Common ImGui_ImplVulkan_XXX functions and structures are used to interface with imgui_impl_vulkan.cpp/.h.
//   You will use those if you want to use this rendering backend in your engine/app.
// - Helper ImGui_ImplVulkanH_XXX functions and structures are only used by this example (main.cpp) and by
//   the backend itself (imgui_impl_vulkan.cpp), but should PROBABLY NOT be used by your own engine/app code.
// Read comments in imgui_impl_vulkan.h.

#include <imgui.h>
#include <ImGuizmo/ImGuizmo.h>
#include <ImPlot/implot.h>
#include <IconsFontAwesome5.h>
#include <modules/imgui/imgui_impl_glfw.h>
#include <modules/imgui/imgui_impl_vulkan.h>
#include <stdio.h>	// printf, fprintf
#include <stdlib.h> // abort
#include <functional>
#include <unordered_map>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>
#include "../resource_manager.hpp"

#include <apis/vk/vk_data.hpp>
#include "../../types.hpp"

class ImGuiRenderer
{

	ImGui_ImplVulkanH_Window m_pMainWindowData;

	VkAllocationCallbacks *m_pAllocator = NULL;
	VkInstance m_pInstance = VK_NULL_HANDLE;
	VkPhysicalDevice m_pPhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_pDevice = VK_NULL_HANDLE;
	uint32_t m_pQueueFamily = (uint32_t)-1;
	VkQueue m_pQueue = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT m_pDebugReport = VK_NULL_HANDLE;
	VkPipelineCache m_pPipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool m_pDescriptorPool = VK_NULL_HANDLE;
	VkFramebuffer m_pFramebuffer = VK_NULL_HANDLE;
	VkCommandBuffer m_pCommandBuffer = VK_NULL_HANDLE;
	VkRenderPass m_pRenderPass = VK_NULL_HANDLE;

	uint32_t m_pImageCount = 0;

	vk::VulkanData *m_pVkDataPtr = nullptr;
	uint32_t m_pCurrentIndex{};

	std::function<void()> m_pUserFunction;

	bool m_pEnabled = true;
	engine::VulkanResourceManager *m_pResourceManagerRef;

	void setStyle()
	{

		ImGuiStyle *style = &ImGui::GetStyle();

		ImVec4 BackgroundDark = ImVec4(48 / 255.0, 56 / 255.0, 65 / 255.0, 0.7f);
		ImVec4 BackgroundLight = ImVec4(58 / 255.0, 71 / 255.0, 80 / 255.0, 1.00f);
		ImVec4 ActiveColor = ImVec4(0, 173 / 255.0, 181 / 255.0, 1.00f);
		ImVec4 TextColor = ImVec4(238.0 / 255.0, 238 / 255.0, 238 / 255.0, 1.00f);
		ImVec4 TextColorSelected = ImVec4(150.0 / 255.0, 150 / 255.0, 150 / 255.0, 1.00f);
		ImVec4 DisabledTextColor = ImVec4(200.0 / 255.0, 200 / 255.0, 200 / 255.0, 1.00f);

		ImVec4 SecondaryActiveColor = ImVec4(0, 130 / 255.0, 150 / 255.0, 1.00f);

		// style->WindowPadding = ImVec2(15, 15);
		// style->WindowRounding = 0.0f;
		// style->FramePadding = ImVec2(5, 5);
		// style->FrameRounding = 0.0f;
		// style->ItemSpacing = ImVec2(12, 8);
		// style->ItemInnerSpacing = ImVec2(8, 6);
		// style->IndentSpacing = 25.0f;
		// style->ScrollbarSize = 15.0f;
		style->GrabMinSize = 5.0f;

		style->ScrollbarRounding = 5.0f;
		style->GrabRounding = 5.0f;
		style->FrameRounding = 5.0f;
		style->WindowRounding = 5.0f;
		style->ChildRounding = 5.0f;
		style->PopupRounding = 5.0f;
		style->TabRounding = 5.0f;

		style->Colors[ImGuiCol_Text] = TextColor;
		style->Colors[ImGuiCol_TextDisabled] = DisabledTextColor;
		style->Colors[ImGuiCol_WindowBg] = BackgroundDark;
		// style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
		style->Colors[ImGuiCol_PopupBg] = BackgroundLight;
		style->Colors[ImGuiCol_Border] = BackgroundDark;
		style->Colors[ImGuiCol_BorderShadow] = BackgroundLight;
		style->Colors[ImGuiCol_FrameBg] = BackgroundLight;
		style->Colors[ImGuiCol_FrameBgHovered] = BackgroundLight;
		style->Colors[ImGuiCol_FrameBgActive] = BackgroundLight;
		style->Colors[ImGuiCol_TitleBg] = BackgroundDark;
		style->Colors[ImGuiCol_TitleBgCollapsed] = BackgroundLight;
		style->Colors[ImGuiCol_TitleBgActive] = BackgroundLight;
		style->Colors[ImGuiCol_MenuBarBg] = BackgroundDark;
		style->Colors[ImGuiCol_ScrollbarBg] = BackgroundDark;
		style->Colors[ImGuiCol_ScrollbarGrab] = BackgroundLight;
		style->Colors[ImGuiCol_ScrollbarGrabHovered] = SecondaryActiveColor;
		style->Colors[ImGuiCol_ScrollbarGrabActive] = ActiveColor;
		// style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
		style->Colors[ImGuiCol_CheckMark] = ActiveColor;
		style->Colors[ImGuiCol_SliderGrab] = SecondaryActiveColor;
		style->Colors[ImGuiCol_SliderGrabActive] = ActiveColor;
		style->Colors[ImGuiCol_Button] = BackgroundDark;
		style->Colors[ImGuiCol_ButtonHovered] = BackgroundLight;
		style->Colors[ImGuiCol_ButtonActive] = ActiveColor;
		style->Colors[ImGuiCol_Header] = BackgroundDark;
		style->Colors[ImGuiCol_HeaderHovered] = SecondaryActiveColor;
		style->Colors[ImGuiCol_HeaderActive] = ActiveColor;
		// style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		// style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
		// style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
		style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
		// style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
		// style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
		// style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
		style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
		style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
		style->Colors[ImGuiCol_TextSelectedBg] = TextColorSelected;
		style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

		style->Colors[ImGuiCol_Tab] = BackgroundDark;
		style->Colors[ImGuiCol_TabHovered] = SecondaryActiveColor;
		style->Colors[ImGuiCol_TabActive] = ActiveColor;
		style->Colors[ImGuiCol_TabUnfocused] = SecondaryActiveColor;
		style->Colors[ImGuiCol_TabUnfocusedActive] = BackgroundLight;

		style->WindowBorderSize = 1.0;
		style->FrameBorderSize = 1.0;
		style->PopupBorderSize = 1.0;
		/*
		{

			ImGuiStyle* style = &ImGui::GetStyle();
			style->Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
			style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			style->Colors[ImGuiCol_WindowBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			style->Colors[ImGuiCol_ChildBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			style->Colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
			style->Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.25f);
			style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style->Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
			style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.68f, 0.69f, 1.00f);
			style->Colors[ImGuiCol_TitleBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
			style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
			style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
			style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
			//style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
			style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
			//style->Colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
			style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.73f, 0.78f, 1.00f);
			style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style->Colors[ImGuiCol_Button] = ImVec4(0.02f, 0.60f, 0.60f, 0.00f);
			//style->Colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
			style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
			style->Colors[ImGuiCol_Header] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
			style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style->Colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
			style->Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
			style->Colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
			//style->Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
			style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
			style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
			style->Colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
			style->Colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
			style->Colors[ImGuiCol_TabActive] = ImVec4(0.28f, 0.73f, 0.78f, 1.00f);
			style->Colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
			style->Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			style->Colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
			style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
			style->Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
			//style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
			style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
			//style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.81f, 0.32f, 1.00f);
			style->Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
			style->Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
			style->Colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
			style->Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			style->Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.28f, 0.73f, 0.78f, 1.00f);
			style->Colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
			style->Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
			style->Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
			style->Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
			style->Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

			style->Colors[ImGuiCol_PlotLines] = ImVec4(225.0 / 255.0, 121.0 / 255.0, 12.0 / 255.0, 1.0);

			style->WindowPadding = ImVec2(5, 5);
			style->WindowRounding = 0.0;
			style->WindowTitleAlign = ImVec2(0.5, 0.5);
			style->WindowBorderSize = 0.0;
		}*/
		style->WindowTitleAlign = ImVec2(0.5, 0.5);

		ImGuiIO &io = ImGui::GetIO();

		// io.Fonts->AddFontFromFileTTF("./resources/Roboto-Regular.ttf", 12);
		// io.Fonts->AddFontFromFileTTF("./resources/Roboto-Regular.ttf", 10);
		// io.Fonts->AddFontFromFileTTF("./resources/Roboto-Regular.ttf", 15);
		ImFontConfig config;
		config.OversampleH = 2;

		static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;

		// #ifdef _WIN32

		icons_config.GlyphOffset.y = 0.8;
		icons_config.GlyphOffset.x = 0.8;
		NormalFont = io.Fonts->AddFontFromFileTTF("./assets/fonts/Roboto-Regular.ttf", 16.0, &config);
		io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-regular-400.ttf", 16.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-brands-400.ttf", 16.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-solid-900.ttf", 16.0f, &icons_config, icons_ranges);
		icons_config.GlyphOffset.y = 0.8;
		icons_config.GlyphOffset.x = 0.8;

		BigFont = io.Fonts->AddFontFromFileTTF("./assets/fonts/Roboto-Regular-Big.ttf", 19, &config);
		io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-regular-400 - Big.ttf", 19.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-brands-400 - Big.ttf", 19.0f, &icons_config, icons_ranges);
		io.Fonts->AddFontFromFileTTF("./assets/fonts/fa-solid-900 - Big.ttf", 19.0f, &icons_config, icons_ranges);

		// #endif
		// #ifdef __linux__
		//		NormalFont = io.Fonts->AddFontFromFileTTF("./assets/fonts/FiraMono-Regular.ttf", 15, &config);
		//		BigFont = io.Fonts->AddFontFromFileTTF("./assets/fonts/FiraMono-Regular.ttf", 19, &config);
		// #endif

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	bool m_pShowDebugPerformance = true;

public:
	ImFont *NormalFont = nullptr;
	ImFont *BigFont = nullptr;

	struct ImageInfo
	{
		VkDescriptorSet id;
		std::string render_pass{};
		uint32_t index;
		glm::ivec2 size;
		std::string name;
	};

	void ShowDebugPerformance(bool val)
	{
		m_pShowDebugPerformance = val;
	}

	void Enable()
	{
		m_pEnabled = true;
	}

	void Disable()
	{
		m_pEnabled = false;
	}

	void Toggle()
	{
		m_pEnabled = !m_pEnabled;
	}
	void setUserFunction(std::function<void()> &&func)
	{
		m_pUserFunction = func;
	}
	void newFrame(uint32_t currentIndex, VkCommandBuffer &currentCommandBuffer)
	{
		m_pCurrentIndex = currentIndex;
		m_pCommandBuffer = currentCommandBuffer;
		ImGuiBegin();
		ImGui::NewFrame();
	}

	void CreateDescriptorPool()
	{
		// Create default descriptor pool
		vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);
		std::array<VkDescriptorPoolSize, 11> poolSizes =
			{{{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
			  {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
			  {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
			  {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
			  {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
			  {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
			  {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}}};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.maxSets = poolSizes.size() * 1000;
		if (vkCreateDescriptorPool(m_pVkDataPtr->logicalDevice, &poolInfo, nullptr, &m_pDescriptorPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void Destroy()
	{
		vkDestroyDescriptorPool(m_pVkDataPtr->logicalDevice, m_pDescriptorPool, nullptr);
		ImGui_ImplVulkan_Shutdown();
	}
	void
	Init(vk::VulkanData &vk_data,
		 framework::Window::windowType *window,
		 VkDescriptorPool &descriptorPool,
		 VkRenderPass &renderPass,
		 VkCommandPool &commandPool,
		 VkCommandBuffer commandBuffer)
	{
		m_pVkDataPtr = &vk_data;

		CreateDescriptorPool();
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO &io = ImGui::GetIO();

		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		m_pMainWindowData.Width = vk_data.swapChainWidth;
		m_pMainWindowData.Height = vk_data.swapChainHeight;

		m_pMainWindowData.Swapchain = vk_data.swapChain;
		m_pMainWindowData.Surface = vk_data.surface;
		m_pMainWindowData.ClearEnable = true;
		m_pDevice = vk_data.logicalDevice;
		m_pMainWindowData.ClearValue.color = {0.0, 0.0, 0.0, 0.0f};

		m_pRenderPass = vk_data.defaultRenderPass.renderPass;

		m_pMainWindowData.RenderPass = vk_data.defaultRenderPass.renderPass;
		// m_pMainWindowData.Pipeline = vk_data.defaultRenderPass.renderPipelines.front().graphicsPipeline;
		m_pFramebuffer = vk_data.defaultRenderPass.renderPassChain.Framebuffers.front();
		// m_pQueue = vk_data.graphicsQueue;
		m_pCommandBuffer = commandBuffer; // The window pipeline may uses a different VkRenderPass than the one passed in ImGui_ImplVulkan_InitInfo

		// uint32_t FrameIndex;     // Current frame being rendered to (0 <= FrameIndex < FrameInFlightCount)
		// uint32_t ImageCount;     // Number of simultaneous in-flight frames (returned by vkGetSwapchainImagesKHR, usually derived from min_image_count)
		// uint32_t SemaphoreIndex; // Current set of swapchain wait semaphores we're using (needs to be distinct from per frame data)
		// ImGui_ImplVulkanH_Frame *Frames;
		// ImGui_ImplVulkanH_FrameSemaphores *FrameSemaphores;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
		// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		setStyle();
		// ImGui::StyleColorsLight();
		ImGuiStyle *style = &ImGui::GetStyle();
		style->ScrollbarRounding = 5.0f;
		style->GrabRounding = 5.0f;
		style->FrameRounding = 5.0f;
		style->WindowRounding = 5.0f;
		style->ChildRounding = 5.0f;
		style->PopupRounding = 5.0f;
		style->TabRounding = 5.0f;

		ImGui_ImplVulkanH_Window *wd = &m_pMainWindowData;
		// Setup Platform/Renderer backends
#if USE_GLFW
		ImGui_ImplGlfw_InitForVulkan(window, true);
#endif
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = vk_data.instance;
		init_info.PhysicalDevice = vk_data.physicalDevice;
		init_info.Device = vk_data.logicalDevice;
		init_info.QueueFamily = 0;
		init_info.Queue = vk_data.graphicsQueue[0];
		init_info.PipelineCache = VK_NULL_HANDLE;
		init_info.DescriptorPool = m_pDescriptorPool;
		init_info.Subpass = 0;
		init_info.MinImageCount = vk::MAX_FRAMES_IN_FLIGHT;
		init_info.ImageCount = vk::MAX_FRAMES_IN_FLIGHT;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		init_info.Allocator = VK_NULL_HANDLE;
		ImGui_ImplVulkan_Init(&init_info, m_pVkDataPtr->defaultRenderPass.renderPass);

		{
			// Use any command queue
			VkCommandPool command_pool = commandPool;
			VkCommandBuffer command_buffer = commandBuffer;

			vkResetCommandPool(vk_data.logicalDevice, command_pool, 0);
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(command_buffer, &begin_info);

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			vkEndCommandBuffer(command_buffer);
			vkQueueSubmit(vk_data.graphicsQueue[0], 1, &end_info, VK_NULL_HANDLE);

			vkDeviceWaitIdle(vk_data.logicalDevice);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
		}

		// ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}
	void ImGuiBegin()
	{
		ImGui_ImplVulkan_NewFrame();

#if USE_GLFW
		ImGui_ImplGlfw_NewFrame();
#endif
	}
	void Render(ImDrawData *draw_data)
	{
		/*
		Create render pass info
		Record command buffer
		*/
		// vk::recordCommandBuffer(m_pCommandBuffer, 0);
		/*
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_pVkDataPtr->defaultRenderPass.renderPass;

		renderPassInfo.framebuffer = m_pVkDataPtr->defaultRenderPass.renderPassChain.Framebuffers[m_pCurrentIndex];

		renderPassInfo.clearValueCount = m_pVkDataPtr->defaultRenderPass.clearValues.size();
		renderPassInfo.pClearValues = m_pVkDataPtr->defaultRenderPass.clearValues.data();

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent;
		m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent = m_pVkDataPtr->defaultRenderPass.renderPassChain.Extent;*/

		// vkCmdBeginRenderPass(m_pCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		/**/
		ImGui_ImplVulkan_RenderDrawData(draw_data, m_pCommandBuffer, VK_NULL_HANDLE);
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow *backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		// vk::endRenderPass(m_pCommandBuffer, *m_pVkDataPtr);
		//   vk::endRecording(m_pCommandBuffer);
		/*
		end render pass
		stop cmd recording
		*/
	}
	void ImGuiEnd()
	{

		ImGui_ImplVulkanH_Window *wd = &m_pMainWindowData;

		// Rendering

		ImGui::Render();
		//}

		ImDrawData *draw_data = ImGui::GetDrawData();
		const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		if (!is_minimized)
		{
			wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
			wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
			wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
			wd->ClearValue.color.float32[3] = clear_color.w;
			Render(draw_data);
			// FramePresent(wd);
		}
	}

	void DrawUI(glm::vec3 &data, engine::ResourcesMemory_t resources)
	{
		if (m_pEnabled)
		{
			// ImGuiBegin();

			ImGui_ImplVulkanH_Window *wd = &m_pMainWindowData;
			// Our state
			static bool show_demo_window = true;
			static bool show_another_window = false;

			MainWindow();

			// ImGui::SetNextWindowSize(ImVec2(wd->Width, wd->Height));

			///////// USER INPUT BEGINS HERE
			if (/*m_pShowDebugPerformance*/ true)
			{
				static bool show_graph = false;
				ImGui::SetNextWindowPos(ImVec2(10, 120));
				ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
				ImGui::Text("Timings:");
				ImGui::BulletText("Framerate %.1f FPS", ImGui::GetIO().Framerate);
				ImGui::BulletText("frametime %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
				ImGui::BulletText("Draw calls: %i", resources.numDrawCalls);
				ImGui::BulletText("Num Vertices: %i", resources.numVertices);
				ImGui::Checkbox("Show graph", &show_graph);
				static float frametime_values[100] = {};
				static int frametime_values_offset = 0;
				if (show_graph) {
					frametime_values_offset = (frametime_values_offset + 1) % IM_ARRAYSIZE(frametime_values);
					frametime_values[frametime_values_offset] = 1000.0 / ImGui::GetIO().Framerate;
					SparkLine("Frametime", "Frametime", frametime_values, 100, 0.5, 18.0, frametime_values_offset, ImVec4(1.0, 1.0, 1.0, 1.0), ImVec2(200, 100));
				}
				ImGui::Separator();
				const char *gpu_name = resources.GPUName.c_str();
				ImGui::Text("%s", gpu_name);
				ImGui::Text("GPU Memory:");
				int i = 0;
				for (auto &heap : resources.heaps)
				{
					//ImGui::Text("Heap %i", i);
					char header_name[128];
					sprintf(header_name, "Heap %i", i);
					if (ImGui::CollapsingHeader(header_name)) {
						ImGui::BulletText("Total %i MB", heap.total_memory / 1024 / 1024);
						ImGui::BulletText("Used %i MB", heap.used_memory / 1024 / 1024);
						ImGui::BulletText("Free %i MB", heap.free_memory / 1024 / 1024);
					}
					i++;
				}

				ImGui::Separator();
				if (ImGui::CollapsingHeader("Resources")) {
					ImGui::BulletText("Vertex buffers %i", m_pResourceManagerRef->vertexBufferPool.size());
					ImGui::BulletText("Index buffers %i", m_pResourceManagerRef->indexBufferPool.size());
					ImGui::BulletText("Textures %i", m_pResourceManagerRef->texPool.size());
					ImGui::BulletText("Meshes %i", m_pResourceManagerRef->meshPool.size());
				}
				ImGui::End();
			}

			if (m_pUserFunction)
				m_pUserFunction();

			ImGui::End();
		}
		ImGuiEnd();
	}

	void SparkLine(const char *id, const char *overlay, const float *values, int count, float min_v, float max_v, int offset, const ImVec4 &col, const ImVec2 &size)
	{
		ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
		// ImPlot::SetNextPlotLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
		ImPlot::SetNextAxesLimits(0, count - 1, min_v, max_v, ImGuiCond_Always);
		if (ImPlot::BeginPlot(id, 0, 0, size, ImPlotFlags_CanvasOnly | ImPlotAxisFlags_NoDecorations, ImPlotAxisFlags_NoDecorations))
		{
			// ImPlot::PlotText(overlay, 10.0, 6.0, );
			ImPlot::PushStyleColor(ImPlotCol_Line, col);
			ImPlot::PlotLine(id, values, count, 1, 0, offset);
			ImPlot::PushStyleVar(ImPlotStyleVar_FillAlpha, 0.25f);
			ImPlot::PlotShaded(id, values, count, 0, 1, 0, offset);
			ImPlot::PopStyleVar();
			ImPlot::PopStyleColor();
			ImPlot::EndPlot();
		}
		ImPlot::PopStyleVar();
	}

public:
	void setResourceManager(engine::VulkanResourceManager *c);

	uint32_t addTexture(const std::string &renderpass, int index);

	uint32_t addTexture(engine::Ref<engine::Texture> texture);

	std::shared_ptr<ImageInfo> getImage(const std::string &index)
	{
		return m_pImages[index];
	}

	auto getImages()
	{
		return m_pImages;
	}

	std::shared_ptr<ImageInfo> getImageByIndex(engine::Ref<engine::Texture> index);

	void recreateDescriptorSets();

	void MainWindow()
	{
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;

		int toolbarHeight = 70;

		if (this->m_pShowDebugPerformance)
		{
			toolbarHeight = 0;
		}

		bool opt_fullscreen = true;
		if (opt_fullscreen)
		{
			ImGuiViewport *viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + toolbarHeight));
			ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - toolbarHeight));
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
		// ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Main Window", NULL, window_flags); // Create a window called "Hello, world!" and append into it.
		ImGui::PopStyleVar();
		// ImGui::PopStyleColor();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO &io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("RootDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}
		else
		{
			// ShowDockingDisabledMessage();
		}
	}

	std::unordered_map<std::string, std::shared_ptr<ImageInfo>> m_pImages;
};
