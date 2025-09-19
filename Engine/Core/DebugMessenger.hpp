#pragma once

#include <vulkan/vulkan.h>

class DebugMessenger {
private:
    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkDebugUtilsMessengerCreateInfoEXT createInfo;

public:
    DebugMessenger();
    void load(VkInstance vkInstance);
    void destroy();
    const VkDebugUtilsMessengerCreateInfoEXT& getCreateInfo() const;
};