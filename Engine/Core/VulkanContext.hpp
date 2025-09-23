#pragma once

#include "DebugMessenger.hpp"
#include "DeviceContext.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

class GlfwContext;

struct LayersCheckResult {
    bool status;
    std::vector<const char*> unsupportedLayers;
};

struct PhysicalDeviceInfo {
    VkPhysicalDevice device;
    QueueFamilyIndices queueFamilyIndices;
    int score;
};

class VulkanContext {
private:
    GlfwContext& glfwCtx;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface;
    DebugMessenger debugMessenger;
    DeviceContext deviceCtx;
    std::vector<const char*> layers;
    std::vector<const char*> extensions;
    PhysicalDeviceInfo selectPhysicalDevice();
    void init();
    void cleanup();
    void setupInstance();
    void createSurface();

    VulkanContext() = delete;
    VulkanContext& operator=(VulkanContext&) = delete;
    VulkanContext(VulkanContext&) = delete;

public:
    VulkanContext(GlfwContext& glfwCtx);
    ~VulkanContext();
};