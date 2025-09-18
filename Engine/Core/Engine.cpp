#include "Engine.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <optional>
#include <sstream>
#include <stdexcept>

#include "Logger.hpp"

LayersCheckResult checkLayersSupport(std::vector<const char*>& layers)
{
    uint32_t layersCount = 0;
    LayersCheckResult layersCheckResult;
    VkResult res;

    layersCheckResult.status = true;
    res = vkEnumerateInstanceLayerProperties(&layersCount, nullptr);
    if (res != VK_SUCCESS)
        throw EngineExceptions::VKCallFailure("vkEnumerateInstanceLayerProperties", res);
    std::vector<VkLayerProperties> layersProperties(layersCount);
    res = vkEnumerateInstanceLayerProperties(&layersCount, layersProperties.data());
    if (res != VK_SUCCESS)
        throw EngineExceptions::VKCallFailure("vkEnumerateInstanceLayerProperties", res);
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

    if (DEBUG)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

void Engine::initGlfw()
{
    if (glfwInit() == GLFW_FALSE)
        throw std::runtime_error("GLFW init failure");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(800, 800, "VulkanEngine", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("GLFW init failure");
    }
    m_glfwInitialized = true;
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isQueueFamiliesFound() { return graphicsFamily.has_value(); }
};

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices queueFamilyIndices;
    uint32_t queueFamiliesCount;
    std::vector<VkQueueFamilyProperties> queueFamiliesProps;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamiliesCount, nullptr);
    queueFamiliesProps.reserve(queueFamiliesCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        device, &queueFamiliesCount, queueFamiliesProps.data());

    for (uint32_t i = 0; i < queueFamiliesCount; i++) {
        if (queueFamiliesProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            queueFamilyIndices.graphicsFamily = i;
    }

    return queueFamilyIndices;
}

int evalPhysicalDeviceScore(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProps;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProps);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    int score = 0;

    if (!deviceFeatures.geometryShader)
        return 0;
    if (deviceProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        score *= 1.25;

    return score;
}

void Engine::selectPhysicalDevice()
{
    uint32_t devicesCount = 0;
    std::vector<VkPhysicalDevice> devices;
    std::multimap<int, VkPhysicalDevice> devicesScoreMap;
    VkResult res;

    res = vkEnumeratePhysicalDevices(this->m_instance, &devicesCount, nullptr);
    if (res != VK_SUCCESS)
        EngineExceptions::VKCallFailure("vkEnumeratePhysicalDevices", res);
    if (!devicesCount)
        std::runtime_error("Failed to find compatible GPUs with Vulkan");
    devices.reserve(devicesCount);
    res = vkEnumeratePhysicalDevices(this->m_instance, &devicesCount, devices.data());
    if (res != VK_SUCCESS)
        EngineExceptions::VKCallFailure("vkEnumeratePhysicalDevices", res);

    for (VkPhysicalDevice device : devices) {
        int score = evalPhysicalDeviceScore(device);
        devicesScoreMap.insert(std::make_pair(score, device));
    }
}

void Engine::initVulkan()
{
    LayersCheckResult layersCheckResult;
    VkApplicationInfo appInfo {};
    VkInstanceCreateInfo createInfo {};
    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};

    try {
        layersCheckResult = checkLayersSupport(m_layers);
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
        m_extensions = getVulkanExtensions();
    } catch (const std::exception& e) {
        LOG_ERROR(e.what());
        throw std::runtime_error("Vulkan init failure");
    }

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (m_layers.size()) {
        createInfo.enabledLayerCount = m_layers.size();
        createInfo.ppEnabledLayerNames = m_layers.data();
    }
    if (DEBUG)
        createInfo.pNext = &m_debugMessenger.createInfo;
    createInfo.enabledExtensionCount = m_extensions.size();
    createInfo.ppEnabledExtensionNames = m_extensions.data();

    VkResult res = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (res != VK_SUCCESS)
        throw std::runtime_error("Vulkan init failure");
    if (DEBUG)
        m_debugMessenger.load(m_instance);
    this->m_vkInitialized = true;
}

void Engine::cleanupGlfw()
{
    glfwDestroyWindow(m_window);
    glfwTerminate();
}

void Engine::cleanupVulkan()
{
    if (DEBUG)
        m_debugMessenger.destroy();
    vkDestroyInstance(m_instance, nullptr);
}

void Engine::loop()
{
    while (!glfwWindowShouldClose(m_window))
        glfwPollEvents();
}

void Engine::cleanup()
{
    if (m_vkInitialized)
        cleanupVulkan();
    if (m_glfwInitialized)
        cleanupGlfw();
}

Engine::Engine()
    : m_glfwInitialized(false)
    , m_vkInitialized(false)
    , m_window(nullptr)
    , m_instance(NULL)
{
    if (DEBUG) {
        m_layers.push_back("VK_LAYER_KHRONOS_validation");
        LOG_VERBOSE("Debugging mode enabled!");
    }
    initGlfw();
    initVulkan();
}

Engine::~Engine() { cleanup(); }

EngineExceptions::VKCallFailure::VKCallFailure(const char* funcName, VkResult errCode)
{
    std::snprintf(this->msgBuffer.data(), this->msgBuffer.size(),
        "Vulkan API Error: %s call returned %d", funcName, errCode);
}

const char* EngineExceptions::VKCallFailure::what() const throw() { return this->msgBuffer.data(); }