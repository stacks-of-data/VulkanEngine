#include "Engine.hpp"
#include "Logger.hpp"
#include <stdexcept>

void Engine::loop() { this->m_glfwContext.loop(); }

Engine::Engine()
    : m_glfwContext()
    , m_vkContext(m_glfwContext)
{
}

Engine::~Engine() { }