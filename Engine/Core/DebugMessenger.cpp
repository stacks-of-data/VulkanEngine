#include "DebugMessenger.hpp"
#include "CommonExceptions.hpp"
#include "Logger.hpp"
#include <vulkan/vulkan_core.h>

static VKAPI_ATTR VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    std::string msg = pCallbackData->pMessage;

    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
        LOG_VERBOSE(msg);
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
        LOG_INFO(msg);
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        LOG_WARNING(msg);
    else
        LOG_ERROR(msg);
    return VK_FALSE;
}

DebugMessenger::DebugMessenger()
    : vkCreateDebugUtilsMessengerEXT(nullptr)
    , vkDestroyDebugUtilsMessengerEXT(nullptr)
    , instance(nullptr)
    , debugMessenger(nullptr)
    , createInfo()

{
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
        | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void DebugMessenger::destroy()
{
    if (this->instance)
        vkDestroyDebugUtilsMessengerEXT(this->instance, this->debugMessenger, nullptr);
}

void DebugMessenger::load(VkInstance instance)
{
    vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    vkDestroyDebugUtilsMessengerEXT = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    VkResult res = vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &this->debugMessenger);
    if (res != VK_SUCCESS)
        VulkanExceptions::VKCallFailure("vkCreateDebugUtilsMessengerEXT", res);
    this->instance = instance;
}

const VkDebugUtilsMessengerCreateInfoEXT& DebugMessenger::getCreateInfo() const
{
    return this->createInfo;
}