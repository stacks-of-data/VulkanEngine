#include "Engine.hpp"
#include "Logger.hpp"
#include <stdexcept>

void Engine::loop() { this->glfwContext.loop(); }

Engine::Engine()
    : glfwContext()
    , vkContext(glfwContext)
{
}

Engine::~Engine() { }