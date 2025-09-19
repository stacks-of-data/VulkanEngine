#pragma once

#include "GlfwContext.hpp"
#include "VulkanContext.hpp"

class Engine {
private:
    GlfwContext m_glfwContext;
    VulkanContext m_vkContext;
public:
    Engine();
    ~Engine();
    void loop();
};