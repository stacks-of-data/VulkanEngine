#pragma once

#include <vulkan/vulkan.h>

class DebugMessenger {
private:
    PFN_vkCreateDebugUtilsMessengerEXT m_vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT m_vkDestroyDebugUtilsMessengerEXT;
    VkInstance m_instance;
    VkDebugUtilsMessengerEXT m_debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT m_createInfo;

public:
    DebugMessenger();
    void load(VkInstance vkInstance);
    void destroy();
    const VkDebugUtilsMessengerCreateInfoEXT& getCreateInfo() const;
};