#pragma once

#include "GlfwContext.hpp"
#include "VulkanContext.hpp"

class Engine {
private:
    GlfwContext glfwContext;
    VulkanContext vkContext;
public:
    Engine();
    ~Engine();
    void loop();
};