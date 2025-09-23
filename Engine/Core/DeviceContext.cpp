#include "DeviceContext.hpp"
#include "CommonExceptions.hpp"
#include <unordered_set>
#include <vector>
#include <vulkan/vulkan_core.h>

void DeviceContext::setupDevice(
    VkPhysicalDevice physicalDevice, QueueFamilyIndices& queueFamilyIndices)
{
    VkPhysicalDeviceFeatures physicalDeviceFeatures {};
    VkDeviceCreateInfo createInfo {};

    std::unordered_set<uint32_t> queueFamilitesSet
        = { *queueFamilyIndices.graphicsFamily, *queueFamilyIndices.presentationFamily };
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    queueCreateInfos.reserve(queueFamilitesSet.size());

    float queuePriority = 1.0f;

    for (const uint32_t queueFamily : queueFamilitesSet) {
        VkDeviceQueueCreateInfo queueCreateInfo {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &physicalDeviceFeatures;
    createInfo.enabledExtensionCount = 0;
    if (layers.size()) {
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();
    }

    VkResult res = vkCreateDevice(physicalDevice, &createInfo, nullptr, &this->device);
    if (res != VK_SUCCESS)
        throw VulkanExceptions::VKCallFailure("vkCreateDevice", res);
    vkGetDeviceQueue(this->device, *queueFamilyIndices.presentationFamily, 0, &presentationQueue);
}

void DeviceContext::cleanup()
{
    if (this->device)
        vkDestroyDevice(this->device, nullptr);
}

DeviceContext::DeviceContext()
    : device(nullptr)
    , presentationQueue(nullptr)
    , layers()
    , extensions()
{}

DeviceContext::~DeviceContext() { cleanup(); }