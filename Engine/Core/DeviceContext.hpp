#pragma once

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentationFamily;

    bool isQueueFamiliesFound()
    {
        return graphicsFamily.has_value() && presentationFamily.has_value();
    }
};

class DeviceContext {
private:
    VkDevice device;
    VkQueue presentationQueue;
    std::vector<const char*> layers;
    std::vector<const char*> extensions;
    DeviceContext(DeviceContext&) = delete;
    DeviceContext operator=(DeviceContext&) = delete;
    void cleanup();
public:
    DeviceContext();
    ~DeviceContext();
    void setupDevice(VkPhysicalDevice physicalDevice, QueueFamilyIndices& queueFamilyIndices);
};