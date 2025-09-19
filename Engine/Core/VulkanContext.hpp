#pragma once

#include "DebugMessenger.hpp"
#include <array>
#include <exception>
#include <optional>
#include <vector>
#include <vulkan/vulkan_core.h>

class GlfwContext;

namespace VulkanExceptions {
class VKCallFailure : public std::exception {
private:
    std::array<char, 1024> msgBuffer;

public:
    VKCallFailure(const char* funcName, VkResult errCode);
    const char* what() const throw();
};
}

struct LayersCheckResult {
    bool status;
    std::vector<const char*> unsupportedLayers;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    bool isQueueFamiliesFound()
    {
        return graphicsFamily.has_value() && presentationFamily.has_value();
    }
};

struct PhysicalDeviceInfo {
    VkPhysicalDevice device;
    QueueFamilyIndices queueFamilyIndices;
    int score;
};

class VulkanContext {
private:
    GlfwContext& glfwContext;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    QueueFamilyIndices queueFamilyIndices;
    VkDevice device;
    VkSurfaceKHR surface;
    DebugMessenger debugMessenger;
    VkQueue presentationQueue;
    std::vector<const char*> layers;
    std::vector<const char*> extensions;
    void selectPhysicalDevice();
    void init();
    void cleanup();
    void setupInstance();
    void setupDevice();
    void createSurface();

    VulkanContext() = delete;
    VulkanContext& operator=(VulkanContext&) = delete;
    VulkanContext(VulkanContext&) = delete;

public:
    VulkanContext(GlfwContext& glfwContext);
    ~VulkanContext();
};