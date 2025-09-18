#include "Core/Engine.hpp"
#include <iostream>
#include "Core/Logger.hpp"

int main()
{
    Engine* engine = nullptr;

    try
    {
        engine = new Engine();
        engine->loop();
    }
    catch (const std::exception& e)
    {
        LOG_ERROR(e.what());
        delete engine;
        return EXIT_FAILURE;
    }
    delete engine;
    return EXIT_SUCCESS;
}