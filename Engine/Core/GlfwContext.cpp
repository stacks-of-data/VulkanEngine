#include "GlfwContext.hpp"
#include "Logger.hpp"
#include <GLFW/glfw3.h>
#include <exception>
#include <stdexcept>

GlfwContext::GlfwContext()
    : m_window(nullptr)
{
    try {
        if (glfwInit() == GLFW_FALSE)
            throw std::runtime_error("glfwInit failed");
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        m_window = glfwCreateWindow(800, 800, "VulkanEngine", nullptr, nullptr);
        if (!m_window) {
            throw std::runtime_error("glfwCreateWindow failed");
        }
    } catch (const std::exception& e) {
        glfwTerminate();
        LOG_ERROR(e.what());
        throw;
    }
}

GlfwContext::~GlfwContext()
{
    if (this->m_window)
        glfwDestroyWindow(this->m_window);
    glfwTerminate();
}

GLFWwindow* GlfwContext::getWindow() { return this->m_window; }

void GlfwContext::loop()
{
    while (!glfwWindowShouldClose(this->m_window))
        glfwPollEvents();
}