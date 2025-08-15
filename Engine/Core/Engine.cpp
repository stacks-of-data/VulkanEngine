#include "Engine.hpp"
#include <cstdio>
#include <iostream>
#include <cstring>
#include <vulkan/vulkan_core.h>

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
    for (const char* layer : layers)
    {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperty : layersProperties)
        {
            if (!strcmp(layer, layerProperty.layerName))
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
        {
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
        throw EngineExceptions::GlfwInitFailure();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    m_window = glfwCreateWindow(800, 800, "VulkanEngine", nullptr, nullptr);
    if (!m_window)
    {
        glfwTerminate();
        throw EngineExceptions::GlfwInitFailure();
    }
    m_glfwInitialized = true;
}

void Engine::selectPhysicalDevice()
{
	uint32_t devicesCount = 0;

	vkEnumeratePhysicalDevices(this->m_instance, &devicesCount, nullptr);
	if (!devicesCount)
		
}

void Engine::initVulkan()
{
    LayersCheckResult layersCheckResult;
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo createInfo{};
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

    try
    {
        layersCheckResult = checkLayersSupport(m_layers);
        if (!layersCheckResult.status)
        {
            std::cerr << "Error: Some layers which are required by the engine are not supported\n"
                << "Unsupported layers are:\n";
            for (const char* layer : layersCheckResult.unsupportedLayers)
                std::cerr << layer << "\n";
            std::cerr.flush();
            throw EngineExceptions::VkInitFailure();
        }
        m_extensions = getVulkanExtensions();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        throw EngineExceptions::VkInitFailure();
    }

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (m_layers.size())
    {
        createInfo.enabledLayerCount = m_layers.size();
        createInfo.ppEnabledLayerNames = m_layers.data();
    }
	if (DEBUG)
		createInfo.pNext = &m_debugMessenger.createInfo;
    createInfo.enabledExtensionCount = m_extensions.size();
    createInfo.ppEnabledExtensionNames = m_extensions.data();
    
    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
    if (result != VK_SUCCESS)
        throw EngineExceptions::VkInitFailure();
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

Engine::Engine():
    m_glfwInitialized(false),
    m_vkInitialized(false),
    m_window(nullptr),
    m_instance(NULL)
{
    if (DEBUG)
    {
        m_layers.push_back("VK_LAYER_KHRONOS_validation");
        std::cout << "Debugging mode enabled!" << std::endl;
    }
    initGlfw();
    initVulkan();
}

Engine::~Engine()
{
    cleanup();
}

const char* EngineExceptions::GlfwInitFailure::what() const throw()
{
    return "GLFW init failure";
}

const char* EngineExceptions::VkInitFailure::what() const throw()
{
    return "Vulkan init failure";
}

EngineExceptions::VKCallFailure::VKCallFailure(const char* funcName, VkResult errCode)
{
    std::snprintf(this->msgBuffer.data(),
        this->msgBuffer.size(),
        "Vulkan API Error: %s call returned %d",
        funcName, errCode);
}

const char* EngineExceptions::VKCallFailure::what() const throw()
{
    return this->msgBuffer.data();
}