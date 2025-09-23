#include "VulkanContext.hpp"
#include "Config.hpp"
#include "GlfwContext.hpp"
#include "Logger.hpp"
#include "CommonExceptions.hpp"
#include <cstdint>
#include <exception>
#include <string>
#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstring>
#include <map>
#include <stdexcept>
#include <vulkan/vulkan.h>

LayersCheckResult checkLayersSupport(std::vector<const char*>& layers)
{
    uint32_t layersCount = 0;
    LayersCheckResult layersCheckResult;
    VkResult res;

    layersCheckResult.status = true;
    res = vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
    if (res != VK_SUCCESS)
        throw VulkanExceptions::VKCallFailure("vkEnumerateInstanceLayerProperties", res);
    std::vector<VkLayerProperties> layersProperties(layersCount);
    res = vkEnumerateInstanceLayerProperties(&layersCount, layersProperties.data());
    if (res != VK_SUCCESS)
        throw VulkanExceptions::VKCallFailure("vkEnumerateInstanceLayerProperties", res);
    for (const char* layer : layers) {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperty : layersProperties) {
            if (!strcmp(layer, layerProperty.layerName)) {
                layerFound = true;
                break;
            }
        }
        if (!layerFound) {
            layersCheckResult.status = false;
            layersCheckResult.unsupportedLayers.push_back(layer);
        }
    }

    return layersCheckResult;
}

std::vector<const char*> getVulkanExtensions()
{
    uint32_t extensionsCount;
    const char** extensionsArr;

    extensionsArr = glfwGetRequiredInstanceExtensions(&extensionsCount);
    std::vector<const char*> extensions(extensionsArr, extensionsArr + extensionsCount);

#ifdef ENGINE_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    return extensions;
}

QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice)
{
    QueueFamilyIndices queueFamilyIndices;
    uint32_t queueFamiliesCount;
    std::vector<VkQueueFamilyProperties> queueFamiliesProps;

    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
    queueFamiliesProps.resize(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamiliesCount, queueFamiliesProps.data());

    for (uint32_t i = 0; i < queueFamiliesCount; i++) {
        VkBool32 presentationSupport = false;
        VkResult res = vkGetPhysicalDeviceSurfaceSupportKHR(
            physicalDevice, i, surface, &presentationSupport);
        if (res != VK_SUCCESS)
            throw VulkanExceptions::VKCallFailure("vkGetPhysicalDeviceSurfaceSupportKHR", res);
        if (queueFamiliesProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            queueFamilyIndices.graphicsFamily = i;
        if (presentationSupport)
            queueFamilyIndices.presentationFamily = i;

        if (queueFamilyIndices.isQueueFamiliesFound())
            break;
    }

    return queueFamilyIndices;
}

PhysicalDeviceInfo evaluatePhysicalDevice(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice)
{
    PhysicalDeviceInfo physicalDeviceInfo {};
    VkPhysicalDeviceProperties deviceProps;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProps);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

    physicalDeviceInfo.device = physicalDevice;
    physicalDeviceInfo.queueFamilyIndices = findQueueFamilies(surface, physicalDevice);

    if (!deviceFeatures.geometryShader
        || !physicalDeviceInfo.queueFamilyIndices.isQueueFamiliesFound())
        return physicalDeviceInfo;
    physicalDeviceInfo.score += deviceProps.limits.maxImageDimension2D;
    if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        physicalDeviceInfo.score *= 1.25;

    return physicalDeviceInfo;
}

PhysicalDeviceInfo VulkanContext::selectPhysicalDevice()
{
    uint32_t devicesCount = 0;
    std::vector<VkPhysicalDevice> physicalDevices;
    std::multimap<int, PhysicalDeviceInfo> devicesScoreMap;
    VkResult res;

    res = vkEnumeratePhysicalDevices(this->instance, &devicesCount, nullptr);
    if (res != VK_SUCCESS)
        throw VulkanExceptions::VKCallFailure("vkEnumeratePhysicalDevices", res);
    if (!devicesCount)
        throw std::runtime_error("Failed to find compatible GPUs with Vulkan");
    physicalDevices.resize(devicesCount);
    res = vkEnumeratePhysicalDevices(this->instance, &devicesCount, physicalDevices.data());
    if (res != VK_SUCCESS)
        throw VulkanExceptions::VKCallFailure("vkEnumeratePhysicalDevices", res);

    for (VkPhysicalDevice physicalDevice : physicalDevices) {
        PhysicalDeviceInfo physicalDeviceInfo
            = evaluatePhysicalDevice(this->surface, physicalDevice);
        devicesScoreMap.insert(std::make_pair(physicalDeviceInfo.score, physicalDeviceInfo));
    }

    if (devicesScoreMap.empty())
        throw std::runtime_error("Failed to find suitable GPUs for the Engine");

    std::multimap<int, PhysicalDeviceInfo>::reverse_iterator rit = devicesScoreMap.rbegin();

    if (rit->first < 1)
        throw std::runtime_error("Failed to find suitable GPUs for the Engine");

    return rit->second;
}

void VulkanContext::setupInstance()
{
    VkApplicationInfo appInfo {};
    VkInstanceCreateInfo createInfo {};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 3, 0);
    appInfo.pEngineName = "VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 3, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (layers.size()) {
        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();
    }
#ifdef ENGINE_DEBUG
    createInfo.pNext = &debugMessenger.getCreateInfo();
#endif
    createInfo.enabledExtensionCount = extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkResult res = vkCreateInstance(&createInfo, nullptr, &instance);
    if (res != VK_SUCCESS)
        throw VulkanExceptions::VKCallFailure("vkCreateInstance", res);
}

void VulkanContext::createSurface()
{
    if (glfwCreateWindowSurface(
            this->instance, this->glfwCtx.getWindow(), nullptr, &this->surface))
        std::runtime_error("glfwCreateWindowSurface failed");
}

void VulkanContext::init()
{
    LayersCheckResult layersCheckResult;
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};

    layersCheckResult = checkLayersSupport(layers);
    if (!layersCheckResult.status) {
        std::string msg = "Error: Some layers which are required by the engine "
                          "are not supported\n"
                          "Unsupported layers are:\n";
        for (const char* layer : layersCheckResult.unsupportedLayers) {
            msg.append(layer);
            msg.push_back('\n');
        }
        throw std::runtime_error(msg);
    }
    extensions = getVulkanExtensions();

    setupInstance();

#ifdef ENGINE_DEBUG
    this->debugMessenger.load(instance);
#endif

    createSurface();
    PhysicalDeviceInfo deviceInfo = selectPhysicalDevice();
    this->deviceCtx.setupDevice(deviceInfo.device, deviceInfo.queueFamilyIndices);
}

VulkanContext::VulkanContext(GlfwContext& glfwCtx)
    : glfwCtx(glfwCtx)
    , instance(nullptr)
    , physicalDevice(nullptr)
    , surface(nullptr)
    , debugMessenger()
    , deviceCtx()
{
#ifdef ENGINE_DEBUG
    this->layers.push_back("VK_LAYER_KHRONOS_validation");
    LOG_VERBOSE("Debugging mode enabled!");
#endif
    try {
        init();
    } catch (const std::exception& e) {
        cleanup();
        throw;
    }
}

void VulkanContext::cleanup()
{
    if (this->instance && this->surface)
        vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
#ifdef ENGINE_DEBUG
    debugMessenger.destroy();
#endif
    if (this->instance)
        vkDestroyInstance(this->instance, nullptr);
}

VulkanContext::~VulkanContext() { cleanup(); }