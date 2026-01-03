// Vulkan + GLFW + Dear ImGui scaffold for DISP IDE
// Requires: GLFW, Vulkan SDK, Dear ImGui (core + backends/imgui_impl_glfw.cpp + imgui_impl_vulkan.cpp)

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>

#include <GLFW/glfw3.h>
#define GLFW_INCLUDE_VULKAN
#include <vulkan/vulkan.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

#include "outline.h"
#include "syntax.h"
#include "suggestions.h"
#include "terminal.h"

static void check_vk_result(VkResult err)
{
    if (err == 0) return;
    fprintf(stderr, "Vulkan error: %d\n", err);
    if (err < 0)
        abort();
}

int main(int argc, char** argv)
{
    // Init GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    if (!glfwVulkanSupported()) {
        fprintf(stderr, "Vulkan not supported on this system (glfwVulkanSupported == false)\n");
        // Fall back: still create a window to display message
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1200, 800, "DISP IDE - Vulkan (Prototype)", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    // Create Vulkan instance (minimal)
    VkInstance instance = VK_NULL_HANDLE;
    {
        VkApplicationInfo app_info{};
        app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        app_info.pApplicationName = "DISP IDE";
        app_info.applicationVersion = VK_MAKE_VERSION(1,0,0);
        app_info.pEngineName = "No Engine";
        app_info.engineVersion = VK_MAKE_VERSION(1,0,0);
        app_info.apiVersion = VK_API_VERSION_1_0;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        VkInstanceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        create_info.pApplicationInfo = &app_info;
        create_info.enabledExtensionCount = (uint32_t)extensions.size();
        create_info.ppEnabledExtensionNames = extensions.data();

    VkResult err = vkCreateInstance(&create_info, nullptr, &instance);
    if (err != VK_SUCCESS) {
        fprintf(stderr, "Failed to create Vulkan instance (vkCreateInstance returned %d)\n", err);
        // continue to show basic window with message
        instance = VK_NULL_HANDLE;
    }
    }

    // Enumerate physical devices and select one
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkQueue graphicsQueue = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    uint32_t graphicsQueueFamily = UINT32_MAX;

    if (instance != VK_NULL_HANDLE) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            fprintf(stderr, "No Vulkan physical devices found\n");
        } else {
            std::vector<VkPhysicalDevice> devices(deviceCount);
            vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
            physicalDevice = devices[0]; // Select first device

            // Find graphics queue family
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
            for (uint32_t i = 0; i < queueFamilyCount; ++i) {
                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    graphicsQueueFamily = i;
                    break;
                }
            }
            if (graphicsQueueFamily == UINT32_MAX) {
                fprintf(stderr, "No graphics queue family found\n");
            } else {
                // Create logical device
                float queuePriority = 1.0f;
                VkDeviceQueueCreateInfo queueCreateInfo{};
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = graphicsQueueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;

                VkDeviceCreateInfo deviceCreateInfo{};
                deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                deviceCreateInfo.queueCreateInfoCount = 1;
                deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
                deviceCreateInfo.enabledExtensionCount = 0;

                err = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
                if (err != VK_SUCCESS) {
                    fprintf(stderr, "Failed to create Vulkan device\n");
                } else {
                    vkGetDeviceQueue(device, graphicsQueueFamily, 0, &graphicsQueue);

                    // Create command pool
                    VkCommandPoolCreateInfo poolInfo{};
                    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    poolInfo.queueFamilyIndex = graphicsQueueFamily;
                    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
                    err = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
                    if (err != VK_SUCCESS) {
                        fprintf(stderr, "Failed to create command pool\n");
                    }

                    // Create descriptor pool
                    VkDescriptorPoolSize poolSizes[] = {
                        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
                        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
                        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
                        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
                        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
                        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
                    };
                    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
                    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                    descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
                    descriptorPoolInfo.maxSets = 1000;
                    descriptorPoolInfo.poolSizeCount = (uint32_t)std::size(poolSizes);
                    descriptorPoolInfo.pPoolSizes = poolSizes;
                    err = vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &descriptorPool);
                    if (err != VK_SUCCESS) {
                        fprintf(stderr, "Failed to create descriptor pool\n");
                    }
                }
            }
        }
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);

    // Create swapchain and render pass
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> swapchainImages;
    VkFormat swapchainImageFormat;
    VkExtent2D swapchainExtent;
    std::vector<VkImageView> swapchainImageViews;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> swapchainFramebuffers;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    uint32_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;

    bool imgui_vulkan_ready = false;

    if (device != VK_NULL_HANDLE) {
        // Create swapchain
        VkSurfaceKHR surface;
        VkResult err = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if (err != VK_SUCCESS) {
            fprintf(stderr, "Failed to create window surface\n");
        } else {
            // Get surface capabilities
            VkSurfaceCapabilitiesKHR capabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

            // Get surface formats
            uint32_t formatCount;
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
            std::vector<VkSurfaceFormatKHR> formats(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());

            VkSurfaceFormatKHR surfaceFormat = formats[0];
            for (const auto& availableFormat : formats) {
                if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                    surfaceFormat = availableFormat;
                    break;
                }
            }

            // Get present modes
            uint32_t presentModeCount;
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
            std::vector<VkPresentModeKHR> presentModes(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());

            VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
            for (const auto& availablePresentMode : presentModes) {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    presentMode = availablePresentMode;
                    break;
                }
            }

            // Choose swap extent
            VkExtent2D extent;
            if (capabilities.currentExtent.width != UINT32_MAX) {
                extent = capabilities.currentExtent;
            } else {
                int width, height;
                glfwGetFramebufferSize(window, &width, &height);
                extent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
                extent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extent.width));
                extent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extent.height));
            }

            uint32_t imageCount = capabilities.minImageCount + 1;
            if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
                imageCount = capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR swapchainCreateInfo{};
            swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainCreateInfo.surface = surface;
            swapchainCreateInfo.minImageCount = imageCount;
            swapchainCreateInfo.imageFormat = surfaceFormat.format;
            swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
            swapchainCreateInfo.imageExtent = extent;
            swapchainCreateInfo.imageArrayLayers = 1;
            swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            swapchainCreateInfo.preTransform = capabilities.currentTransform;
            swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchainCreateInfo.presentMode = presentMode;
            swapchainCreateInfo.clipped = VK_TRUE;

            err = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
            if (err != VK_SUCCESS) {
                fprintf(stderr, "Failed to create swapchain\n");
            } else {
                vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
                swapchainImages.resize(imageCount);
                vkGetSwapchainImagesKHR(device, swapchain, &imageCount, swapchainImages.data());
                swapchainImageFormat = surfaceFormat.format;
                swapchainExtent = extent;

                // Create image views
                swapchainImageViews.resize(swapchainImages.size());
                for (size_t i = 0; i < swapchainImages.size(); ++i) {
                    VkImageViewCreateInfo viewInfo{};
                    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    viewInfo.image = swapchainImages[i];
                    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    viewInfo.format = swapchainImageFormat;
                    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    viewInfo.subresourceRange.baseMipLevel = 0;
                    viewInfo.subresourceRange.levelCount = 1;
                    viewInfo.subresourceRange.baseArrayLayer = 0;
                    viewInfo.subresourceRange.layerCount = 1;

                    err = vkCreateImageView(device, &viewInfo, nullptr, &swapchainImageViews[i]);
                    if (err != VK_SUCCESS) {
                        fprintf(stderr, "Failed to create image view\n");
                    }
                }

                // Create render pass
                VkAttachmentDescription colorAttachment{};
                colorAttachment.format = swapchainImageFormat;
                colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
                colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                VkAttachmentReference colorAttachmentRef{};
                colorAttachmentRef.attachment = 0;
                colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

                VkSubpassDescription subpass{};
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &colorAttachmentRef;

                VkRenderPassCreateInfo renderPassInfo{};
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.attachmentCount = 1;
                renderPassInfo.pAttachments = &colorAttachment;
                renderPassInfo.subpassCount = 1;
                renderPassInfo.pSubpasses = &subpass;

                err = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
                if (err != VK_SUCCESS) {
                    fprintf(stderr, "Failed to create render pass\n");
                } else {
                    // Create framebuffers
                    swapchainFramebuffers.resize(swapchainImageViews.size());
                    for (size_t i = 0; i < swapchainImageViews.size(); ++i) {
                        VkImageView attachments[] = {swapchainImageViews[i]};
                        VkFramebufferCreateInfo framebufferInfo{};
                        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                        framebufferInfo.renderPass = renderPass;
                        framebufferInfo.attachmentCount = 1;
                        framebufferInfo.pAttachments = attachments;
                        framebufferInfo.width = swapchainExtent.width;
                        framebufferInfo.height = swapchainExtent.height;
                        framebufferInfo.layers = 1;

                        err = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchainFramebuffers[i]);
                        if (err != VK_SUCCESS) {
                            fprintf(stderr, "Failed to create framebuffer\n");
                        }
                    }

                    // Create command buffers
                    commandBuffers.resize(swapchainFramebuffers.size());
                    VkCommandBufferAllocateInfo allocInfo{};
                    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    allocInfo.commandPool = commandPool;
                    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

                    err = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
                    if (err != VK_SUCCESS) {
                        fprintf(stderr, "Failed to allocate command buffers\n");
                    } else {
                        // Create sync objects
                        imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
                        renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
                        inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

                        VkSemaphoreCreateInfo semaphoreInfo{};
                        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

                        VkFenceCreateInfo fenceInfo{};
                        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

                        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
                            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
                            vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
                            vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]);
                        }

                        // Initialize ImGui Vulkan backend
                        ImGui_ImplVulkan_InitInfo init_info = {};
                        init_info.Instance = instance;
                        init_info.PhysicalDevice = physicalDevice;
                        init_info.Device = device;
                        init_info.QueueFamily = graphicsQueueFamily;
                        init_info.Queue = graphicsQueue;
                        init_info.PipelineCache = VK_NULL_HANDLE;
                        init_info.DescriptorPool = descriptorPool;
                        init_info.Subpass = 0;
                        init_info.MinImageCount = 2;
                        init_info.ImageCount = (uint32_t)swapchainImages.size();
                        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
                        init_info.Allocator = nullptr;
                        init_info.CheckVkResultFn = check_vk_result;

                        ImGui_ImplVulkan_Init(&init_info, renderPass);

                        // Upload fonts
                        VkCommandBuffer commandBuffer = commandBuffers[0];
                        vkResetCommandBuffer(commandBuffer, 0);
                        VkCommandBufferBeginInfo beginInfo{};
                        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                        beginInfo.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
                        vkBeginCommandBuffer(commandBuffer, &beginInfo);

                        ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);

                        vkEndCommandBuffer(commandBuffer);

                        VkSubmitInfo submitInfo{};
                        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                        submitInfo.commandBufferCount = 1;
                        submitInfo.pCommandBuffers = &commandBuffer;

                        vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
                        vkQueueWaitIdle(graphicsQueue);

                        ImGui_ImplVulkan_DestroyFontUploadObjects();

                        imgui_vulkan_ready = true;
                    }
                }
            }
            vkDestroySurfaceKHR(instance, surface, nullptr);
        }
    }

    // Setup style
    ImGui::StyleColorsDark();

    // Editor state
    std::string currentFile = "examples/hello.dis";
    std::string editorText = "#!/bin/bash\necho \"Installing DISP Programming Language...\"";
    std::string buildOutput = "";

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Docking layout
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGui::Begin("MainDockSpace", nullptr, window_flags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));

        // Menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Open")) {
                    // TODO: File dialog
                }
                if (ImGui::MenuItem("Save")) {
                    std::ofstream out(currentFile);
                    if (out) out << editorText;
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Build")) {
                if (ImGui::MenuItem("Compile")) {
                    buildOutput = run_command_capture("./disp " + currentFile + " -o " + currentFile + ".disp");
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Left: Explorer
        ImGui::Begin("Explorer");
        if (ImGui::TreeNode("Project")) {
            if (ImGui::Selectable("examples/")) {
                // TODO: Expand directory
            }
            if (ImGui::Selectable("src/")) {
                // TODO: Expand directory
            }
            if (ImGui::Selectable("build.sh")) {
                currentFile = "build.sh";
                std::ifstream in(currentFile);
                if (in) {
                    std::ostringstream ss;
                    ss << in.rdbuf();
                    editorText = ss.str();
                }
            }
            ImGui::TreePop();
        }
        ImGui::End();

        // Center: Editor
        ImGui::Begin("Editor");
        ImGui::Text("File: %s", currentFile.c_str());
        ImGui::InputTextMultiline("##editor", &editorText, ImVec2(-1, -1));
        ImGui::End();

        // Right: Outline
        ImGui::Begin("Outline");
        auto outline = get_outline_from_file(currentFile);
        for (const auto& item : outline) {
            ImGui::BulletText("%s", item.c_str());
        }
        ImGui::End();

        // Bottom: Build
        ImGui::Begin("Build");
        if (ImGui::Button("Compile")) {
            buildOutput = run_command_capture("./disp " + currentFile + " -o " + currentFile + ".disp");
        }
        ImGui::SameLine();
        if (ImGui::Button("Run")) {
            buildOutput += "\n" + run_command_capture("./" + currentFile + ".disp");
        }
        ImGui::InputTextMultiline("##build", &buildOutput, ImVec2(-1, -1));
        ImGui::End();

        ImGui::End(); // MainDockSpace

        // Render
        ImGui::Render();

        if (imgui_vulkan_ready) {
            // Vulkan rendering
            vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

            uint32_t imageIndex;
            VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
            if (result == VK_ERROR_OUT_OF_DATE_KHR) {
                // TODO: Recreate swapchain
                continue;
            } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
                fprintf(stderr, "Failed to acquire swapchain image\n");
            }

            vkResetFences(device, 1, &inFlightFences[currentFrame]);

            vkResetCommandBuffer(commandBuffers[currentFrame], 0);
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo);

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapchainFramebuffers[imageIndex];
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapchainExtent;
            VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffers[currentFrame]);
            vkCmdEndRenderPass(commandBuffers[currentFrame]);

            vkEndCommandBuffer(commandBuffers[currentFrame]);

            VkSubmitInfo submitInfo{};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
            VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = waitSemaphores;
            submitInfo.pWaitDstStageMask = waitStages;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
            VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = signalSemaphores;

            vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

            VkPresentInfoKHR presentInfo{};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = signalSemaphores;
            VkSwapchainKHR swapchains[] = {swapchain};
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = swapchains;
            presentInfo.pImageIndices = &imageIndex;

            vkQueuePresentKHR(graphicsQueue, &presentInfo);

            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        } else {
            // Fallback: just poll events
            glfwSwapBuffers(window);
        }
    }

    // Cleanup
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (instance != VK_NULL_HANDLE) vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
