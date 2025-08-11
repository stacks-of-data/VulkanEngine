#include "Engine.hpp"
#include <cstdio>
#include <iostream>
#include <cstring>

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

    if (ENGINE_DEBUG)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;
}

static VKAPI_ATTR VkBool32 debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
)
{
    std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
)

void Engine::initGlfw()
{
    if (glfwInit() == GLFW_FALSE)
        throw EngineExceptions::GlfwInitFailure();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    this->window = glfwCreateWindow(800, 800, "VulkanEngine", nullptr, nullptr);
    if (!this->window)
    {
        glfwTerminate();
        throw EngineExceptions::GlfwInitFailure();
    }
    this->glfwInitalized = true;
}

void Engine::initVulkan()
{
    LayersCheckResult layersCheckResult;
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo createInfo{};

    try
    {
        layersCheckResult = checkLayersSupport(this->layers);
        if (!layersCheckResult.status)
        {
            std::cerr << "Error: Some layers which are required by the engine are not supported\n"
                << "Unsupported layers are:\n";
            for (const char* layer : layersCheckResult.unsupportedLayers)
                std::cerr << layer << "\n";
            std::cerr.flush();
            throw EngineExceptions::VkInitFailure();
        }
        this->extensions = getVulkanExtensions();
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
    if (this->layers.size())
    {
        createInfo.enabledLayerCount = this->layers.size();
        createInfo.ppEnabledLayerNames = this->layers.data();
    }
    createInfo.enabledExtensionCount = this->extensions.size();
    createInfo.ppEnabledExtensionNames = this->extensions.data();
    
    VkResult result = vkCreateInstance(&createInfo, nullptr, &this->vkInstance);
    if (result != VK_SUCCESS)
        throw EngineExceptions::VkInitFailure();
}

void Engine::cleanupGlfw()
{
    glfwDestroyWindow(this->window);
    glfwTerminate();
}

void Engine::cleanupVulkan()
{
    vkDestroyInstance(this->vkInstance, nullptr);
}

void Engine::loop()
{
    while (!glfwWindowShouldClose(this->window))
        glfwPollEvents();
}

void Engine::cleanup()
{
    if (this->vkInitalized)
        cleanupVulkan();
    if (this->glfwInitalized)
        cleanupGlfw();
}

Engine::Engine():
    glfwInitalized(false),
    vkInitalized(false),
    window(nullptr),
    vkInstance(NULL)
{
    if (ENGINE_DEBUG)
    {
        this->layers.push_back("VK_LAYER_KHRONOS_validation");
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