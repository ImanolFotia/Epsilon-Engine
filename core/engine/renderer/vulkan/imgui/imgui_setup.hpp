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
#include <modules/imgui/imgui_impl_glfw.h>
#include <modules/imgui/imgui_impl_vulkan.h>
#include <stdio.h>  // printf, fprintf
#include <stdlib.h> // abort
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

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

    vk::VulkanData *m_pVkDataPtr = nullptr;
    uint32_t m_pCurrentIndex{};

public:
    void newFrame(uint32_t currentIndex, VkCommandBuffer &currentCommandBuffer)
    {
        m_pCurrentIndex = currentIndex;
        m_pCommandBuffer = currentCommandBuffer;
    }
    void
    Init(vk::VulkanData &vk_data,
         GLFWwindow *window,
         VkDescriptorPool &descriptorPool,
         VkRenderPass &renderPass,
         VkCommandPool &commandPool,
         VkCommandBuffer commandBuffer)
    {
        m_pVkDataPtr = &vk_data;
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;

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
        m_pQueue = vk_data.graphicsQueue;
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
        // ImGui::StyleColorsLight();

        ImGui_ImplVulkanH_Window *wd = &m_pMainWindowData;
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForVulkan(window, true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = vk_data.instance;
        init_info.PhysicalDevice = vk_data.physicalDevice;
        init_info.Device = vk_data.logicalDevice;
        init_info.QueueFamily = 0;
        init_info.Queue = vk_data.graphicsQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = descriptorPool;
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
            vkQueueSubmit(vk_data.graphicsQueue, 1, &end_info, VK_NULL_HANDLE);

            vkDeviceWaitIdle(vk_data.logicalDevice);
            ImGui_ImplVulkan_DestroyFontUploadObjects();
        }
    }
    void Begin()
    {
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

        // vk::endRenderPass(m_pCommandBuffer, *m_pVkDataPtr);
        //   vk::endRecording(m_pCommandBuffer);
        /*
        end render pass
        stop cmd recording
        */
    }
    void End() {}
    void Destroy() {}

    void DrawUI(glm::vec3 &data, engine::ResourcesMemory_t resources)
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplVulkanH_Window *wd = &m_pMainWindowData;

        // Our state
        static bool show_demo_window = true;
        static bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(wd->Width, wd->Height));
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("##transparentWindow", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("framerate %.1f FPS", ImGui::GetIO().Framerate);
        ImGui::Text("frametime %.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
        ImGui::End();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("PCSS");

            ImGui::SliderFloat("LIGHT WORLD SIZE", &data.x, 0.0f, 100.0f);    // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("LIGHT FRUSTUM WIDTH", &data.y, 1.0f, 100.0f); // Edit 1 float using a slider from 0.0f to 1.0f // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::SliderFloat("PENUMBRA SIZE", &data.z, 0.1f, 100.0f);
            ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color
            ImGui::Separator();

            ImGui::Text("Texture Allocation Size = %.3f MB", resources.m_pTextureBufferAllocationSize / 1000000.0f);
            ImGui::Text("Index Buffer Usage = %.3f MB", resources.m_pIndexBufferAllocationSize / 1000000.0f);
            ImGui::Text("Vertex Allocation Size = %.3f MB", resources.m_pVertexBufferAllocationSize / 1000000.0f);
            ImGui::Text("Uniforms Allocation Size = %.3f MB", resources.m_pUniformBufferAllocationSize / 1000000.0f);
            ImGui::Text("Storage Allocation Size = %.3f MB", resources.m_pStorageBufferAllocationSize / 1000000.0f);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
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

public:
    void pDemo()
    {

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplVulkanH_Window *wd = &m_pMainWindowData;

        // Our state
        static bool show_demo_window = true;
        static bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        ImGui::SetNextWindowSize(ImVec2(wd->Width, wd->Height));
        ImGui::Begin("##transparentWindow", nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");          // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);             // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window); // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        ImDrawData *draw_data = ImGui::GetDrawData();
        const bool is_minimized = (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f);
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
};
