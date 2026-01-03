// Minimal Vulkan+GLFW IDE scaffold (prototype)
// Requires MSYS2 packages: glfw, glm, vulkan-devel

#include <iostream>
#include <vector>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main(int argc, char** argv) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return 1;
    }

    if (!glfwVulkanSupported()) {
        std::cerr << "Warning: Vulkan not supported by this GLFW build or system.\n";
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Displexity IDE (Vulkan)", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return 1;
    }

    // Query Vulkan instance extensions (if available)
    uint32_t extCount = 0;
    VkResult r = vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    if (r == VK_SUCCESS) {
        std::vector<VkExtensionProperties> exts(extCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extCount, exts.data());
        std::cout << "Vulkan instance extensions available: " << extCount << "\n";
    } else {
        std::cout << "Could not enumerate Vulkan extensions (vk call failed)\n";
    }

    // Main loop (no rendering yet)
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // TODO: integrate Dear ImGui + Vulkan renderer and editor widget
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
