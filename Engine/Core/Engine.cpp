#include "Engine.hpp"

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
    VkApplicationInfo appInfo{};
    VkInstanceCreateInfo createInfo{};
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "VulkanEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "VulkanEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    
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
    initGlfw();
    
}

Engine::~Engine()
{
    cleanup();
}

const char* EngineExceptions::GlfwInitFailure::what() const throw()
{
    return ("GLFW init failure");
}